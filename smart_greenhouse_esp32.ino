#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

#define pinDATA 32 // SDA, or almost any other I/O pin
#define SOL_H 33
#define PR 35

#define LED_B 21
#define PUMP 18
#define VENT 15
#define HUMIDIFIER 19

#define knownResistor 10000.0

DHTesp dht;

int ledState = LOW;
int pumpState = LOW;
int humidifierState = LOW;
int ventState = LOW;

int low_power_mode = 0;

const char* ssid = "Airbox-1AB5";  
const char* password = "8rD84S4P"; 
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espclient;

String temp_str, hum_str, shum_str, lm_str; 
char temp[50], hum[50], shum[50], lm[50];

// MQTT callback to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");

    if (length == 0) {
        Serial.println("Received an empty payload, ignoring.");
        return;
    }

    String message = "";

    // Convert payload to string
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(message);

    // Try to convert message to integer
    int command = message.toInt();
    
    if (command == 0 && message != "0") {  // If conversion fails, report an error
        Serial.println("Error: Not a valid number");
        return;
    }

    // Handle incoming MQTT messages
    switch (command) {
        case 21:
            digitalWrite(LED_B, HIGH); 
            ledState = HIGH;   
            Serial.println("LED ON");
            break;
        case 20:
            digitalWrite(LED_B, LOW);
            ledState = LOW;   
            Serial.println("LED OFF");
            break;
        case 31:
            digitalWrite(PUMP, HIGH);
            pumpState = HIGH;
            Serial.println("PUMP ON");
            break;
        case 30:
            digitalWrite(PUMP, LOW);
            pumpState = LOW;
            Serial.println("PUMP OFF");
            break;
        case 41:
            digitalWrite(HUMIDIFIER, HIGH);
            humidifierState = HIGH;
            Serial.println("HUMIDIFIER ON");
            break;
        case 40:
            digitalWrite(HUMIDIFIER, LOW);
            humidifierState = LOW;
            Serial.println("HUMIDIFIER OFF");
            break;
        case 51:
            digitalWrite(VENT, HIGH);
            ventState = HIGH;
            Serial.println("VENT ON");
            break;
        case 50:
            digitalWrite(VENT, LOW);
            ventState = LOW;
            Serial.println("VENT OFF");
            break;
        case 1:
            low_power_mode = 1;
            Serial.println("Low power mode ON");
            break;
        case 0:
            low_power_mode = 0;
            Serial.println("Low power mode OFF");
            break;
        default:
            Serial.println("Error: Unrecognized command");
            break;
    }
}

PubSubClient client(mqtt_server, 1883, callback, espclient);

unsigned long lastSendTime = 0; // Variable to store last send time
unsigned long lastSleepTime = 0;
const unsigned long sendInterval = 5000; // 2 minutes in milliseconds
const unsigned long powerofInterval = 2 * 60 * 1000; // 1 minute

void setup() {
    pinMode(LED_B, OUTPUT);
    pinMode(PUMP, OUTPUT);
    pinMode(HUMIDIFIER, OUTPUT);  
    pinMode(VENT, OUTPUT);

    Serial.begin(115200);

    // Setup DHT sensor
    dht.setup(pinDATA, DHTesp::DHT22); // Initialize the DHT22 sensor

    // Connect to Wi-Fi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" Connected to Wi-Fi.");
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32test1")) {
            Serial.println("Connected to MQTT broker.");
            client.subscribe("commandgh");
            client.publish("ESPstatus", "on"); // Indicate the ESP is online
        } else {
            Serial.print("Failed to connect, rc=");
            Serial.println(client.state());
            delay(500);
        }
    }
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }

    client.loop();

    // Check if it's time to send the readings
    if (millis() - lastSendTime >= sendInterval) {
        lastSendTime = millis(); // Update last send time

        delay(2000); // Optional delay before taking readings

        //client.publish("ESPstatus", "on"); // Publish status before sending data

        // Get sensor readings
        float t = dht.getTemperature();
        float h = dht.getHumidity();
        float sh = map(analogRead(SOL_H), 0, 4095, 100, 0);
        float lux = analogRead(PR);

        Serial.print("Soil Humidity: "); Serial.println(sh);
        Serial.print("Humidity: "); Serial.println(h);
        Serial.print("Temperature: "); Serial.println(t);
        Serial.print("Lux: "); Serial.println(lux);

        if (dht.getStatus() != DHTesp::ERROR_NONE) {
            Serial.print("DHT22 Error: ");
            Serial.println(dht.getStatusString());
        }

        // Prepare data for MQTT
        temp_str = String(t);
        temp_str.toCharArray(temp, temp_str.length() + 1);

        hum_str = String(h);
        hum_str.toCharArray(hum, hum_str.length() + 1);

        shum_str = String(sh);
        shum_str.toCharArray(shum, shum_str.length() + 1);

        lm_str = String(lux);
        lm_str.toCharArray(lm, lm_str.length() + 1);

        // Publish sensor data
        if (client.connect("ESP32test1")) {
            client.subscribe("commandgh");

            client.publish("tempg", temp);
            client.publish("humg", hum);
            client.publish("shumg", shum);
            client.publish("luxg", lm);
            client.publish("ESPstatus", "on"); // Publish status after sending data

            // Publish actuator status
            client.publish("statusg", ledState == HIGH ? "21" : "20"); // LED
            client.publish("statusg", pumpState == HIGH ? "31" : "30"); // PUMP
            client.publish("statusg", humidifierState == HIGH ? "41" : "40"); // HUMIDIFIER
            client.publish("statusg", ventState == HIGH ? "51" : "50"); // VENT

            // Sleep mode handling
            if (low_power_mode == 1 && millis() - lastSleepTime >= powerofInterval) {
                lastSleepTime = millis();
                Serial.println("Going to deep sleep for 5 minutes");
                client.publish("ESPstatus", "off");
                esp_sleep_enable_timer_wakeup(15 * 60 * 1000000); // 5 minutes
                esp_deep_sleep_start();
            }
        } else {
            Serial.print("Failed to connect, rc=");
            Serial.println(client.state());
            delay(500);
        }
    }
}
