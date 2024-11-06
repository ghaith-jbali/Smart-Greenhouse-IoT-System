# Smart Greenhouse IoT System

This repository contains the Smart Greenhouse IoT System, which integrates IoT technologies, AI-driven insights, and real-time data visualization to manage greenhouse conditions. The system can monitor environmental factors, automate responses, and offer interactive plant care advice using an AI assistant. 

## Project Overview

The Smart Greenhouse project is designed to:
- Monitor temperature, humidity, soil moisture, and light intensity using sensors.
- Automatically control pumps, fans, lights, and a humidifier to maintain optimal conditions.
- Provide an interactive dashboard via Node-RED for real-time control and data visualization.
- Respond to user questions with plant care insights using an AI assistant integrated with the Google Gemini API.

## Repository Contents

- **Node-RED Flows**:
  - `weather_dashboard_flow.json`: Manages weather data collection, visualization, and environmental monitoring.
  - `greenhouse_automation_flow.json`: Automates equipment based on sensor data and user settings.

- **Python Server**:
  - `smart_planter_server.py`: The server code for processing questions, generating AI responses, and managing sensor data.

- **ESP32 Code**:
  - `smart_greenhouse_esp32.ino`: Code for the ESP32 to read sensors and control actuators, designed to interact with the MQTT broker.

- **Data Files**:
  - `temp.csv`, `hum.csv`, `shum.csv`, `lum.csv`: Store sensor readings for temperature, humidity, soil moisture, and light intensity.
  - `plant.txt`: Contains the type of plant being monitored.
  - `conversation_history.json`: Logs questions and responses for the AI assistant.

## Setup Guide

### Prerequisites
- **Python 3.7+**
- **Node-RED**
- **MQTT Broker** (e.g., Mosquitto)
- **OpenWeatherMap API Key**

### Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/ghaith-jbali/Smart-Greenhouse-IoT-System.git
   cd Smart-Greenhouse-IoT-System
   ```

2. **Install Python Dependencies**:
   ```bash
   pip install -r requirements.txt
   ```

3. **Set Up Environment Variables**:
   - Create a `.env` file in the root directory and add your Google Gemini API key:
     ```bash
     API_KEY=your_google_gemini_api_key
     ```

4. **Create `plant.txt`**:
   - Add the type of plant being monitored (e.g., "Tomato").

5. **Start the Flask Server**:
   ```bash
   python smart_planter_server.py
   ```

6. **Upload ESP32 Code**:
   - Flash `smart_greenhouse_esp32.ino` to your ESP32 board, ensuring the Wi-Fi and MQTT broker settings are configured.

7. **Import Node-RED Flows**:
   - Open Node-RED and import `weather_dashboard_flow.json` and `greenhouse_automation_flow.json`.

### Configuring Node-RED

1. **HTTP Node Setup**:
   - **Install HTTP Node**: Go to `Manage Palette` > `Install` in Node-RED, and install the HTTP nodes if not already available.
   - **Configure Endpoint**: Add an HTTP input node with the method `POST`. Define the endpoint (e.g., `/api/data`) and connect it to the necessary nodes for data processing.

2. **OpenWeatherMap API Integration**:
   - **Register for API Key**: Create an account at [OpenWeatherMap](https://openweathermap.org/) and obtain an API key.
   - **Set Up Node-RED OpenWeatherMap Node**: Add your API key and location details to the `openweathermap` node in Node-RED, linking it to UI components for real-time weather data display.

### Usage

- **Access the Dashboard**: Open the Node-RED UI to view and control the system.
- **Interact with the AI**: Send POST requests to the `/ask` endpoint on the Flask server to receive plant care tips or inquire about greenhouse conditions.

### Example API Request
```json
POST /ask
{
  "question": "How is my plant doing?"
}
```

## Security Notes

- **API Key Security**: Store API keys and sensitive information in environment variables, and exclude them from the repository using `.gitignore`.
- **Data Files**: Keep `conversation_history.json` and other log files private or ensure they are properly managed if shared.
