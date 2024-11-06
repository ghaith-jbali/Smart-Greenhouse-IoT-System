---

# Smart Greenhouse IoT Project

This project integrates IoT technologies, AI-driven insights, and real-time data visualization to create an intelligent greenhouse management system. It features environmental monitoring, automatic adjustments to maintain optimal conditions, and an interactive AI assistant for user queries. The system is built using Python, Node-RED, and various sensors.

## Project Overview

The Smart Greenhouse project is designed to:
- Monitor temperature, humidity, soil moisture, and light intensity using sensors.
- Automatically control equipment like pumps, humidifiers, lights, and fans based on sensor data.
- Provide an interactive dashboard built with Node-RED for real-time control and visualization.
- Respond to user questions and offer plant care insights using an AI assistant integrated with the Google Gemini API.

## Features

### 1. Environmental Monitoring
- **Sensors**: Reads temperature, humidity, soil moisture, and light intensity.
- **Data Logging**: Stores sensor data in CSV files (`temp.csv`, `hum.csv`, `shum.csv`, `lum.csv`) for analysis.

### 2. Automation and Control
- **Actuators**: Controls pumps, fans, lights, and a humidifier.
- **Automated Triggers**: Node-RED flows enable automatic responses to sensor readings, maintaining optimal greenhouse conditions.
- **Manual Overrides**: Users can override automation using UI controls on the Node-RED dashboard.

### 3. AI Assistant Integration
- **AI Model**: Uses Google Gemini for generating user-friendly responses and plant care suggestions.
- **Flask Server**: Hosts the AI assistant API, handles user questions, and stores conversation history in `conversation_history.json`.

## File Structure

### Node-RED Flows
- `weather_dashboard_flow.json`: Manages weather data collection, visualization, and environmental monitoring.
- `greenhouse_automation_flow.json`: Contains flows for automating equipment based on sensor data and user controls.

### Python Server
- `smart_planter_server.py`: The main server code for processing questions, generating AI responses, and managing data.

### Data Files
- **CSV Files**: Store sensor readings for temperature (`temp.csv`), humidity (`hum.csv`), soil moisture (`shum.csv`), and light intensity (`lum.csv`).
- **Plant Type** (`plant.txt`): Contains the type of plant being monitored. This file should have a single line with the plant name, allowing the AI assistant to provide context-specific care advice.

### Conversation History
- **Conversation Log** (`conversation_history.json`): Records each question and response in JSON format with timestamps for session-based memory. Each entry is structured as:
  ```json
  {
    "timestamp": "YYYY-MM-DD HH:MM:SS",
    "question": "What is the current humidity?",
    "response": "The current humidity is 60%."
  }
  ```

## Setup Guide

### Prerequisites
- **Python 3.7+**
- **Node-RED**
- **MQTT Broker** (e.g., Mosquitto)
- **OpenWeatherMap API Key**

### Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/yourusername/smart-greenhouse.git
   cd smart-greenhouse
   ```

2. **Install Python Dependencies**:
   ```bash
   pip install -r requirements.txt
   ```

3. **Set Up Environment Variables**:
   Create a `.env` file and add:
   ```bash
   API_KEY=your_google_gemini_api_key
   ```

4. **Create `plant.txt`**:
   - Add the type of plant being monitored (e.g., "Tomato").

5. **Start the Flask Server**:
   ```bash
   python smart_planter_server.py
   ```

6. **Import Node-RED Flows**:
   - Open Node-RED and import `weather_dashboard_flow.json` and `greenhouse_automation_flow.json`.

### Configuring Node-RED

1. **HTTP Node Setup**:
   - **Install HTTP Node**: Ensure that the Node-RED HTTP nodes are installed. Go to `Manage Palette` > `Install` and search for relevant HTTP nodes if not already available.
   - **Configure Endpoint**: Add an HTTP input node and set the method to `POST`. Define the endpoint (e.g., `/api/data`) and connect it to your function nodes or processing flow.

2. **OpenWeatherMap API Integration**:
   - **Register and Get API Key**: Create an account at [OpenWeatherMap](https://openweathermap.org/) and obtain an API key.
   - **Configure the API Node**:
     - Use the `openweathermap` node in Node-RED for current weather and forecast data.
     - Input your API key in the node's configuration.
     - Set the location parameters (city, latitude/longitude) for your greenhouse.
   - **Connect to Dashboard**: Link the weather node to a dashboard UI element, such as text or chart nodes, to visualize weather conditions.

### Usage

- **Access the Dashboard**: Open Node-RED's UI to control the system and view sensor data.
- **Interact with the AI**: Send POST requests to the `/ask` endpoint of the Flask server to receive plant care tips or ask about greenhouse conditions.

### Example API Request
```json
POST /ask
{
  "question": "How is my plant doing?"
}
```
---
