import os
import pandas as pd
import google.generativeai as genai  # Google Gemini API
from flask import Flask, request, jsonify
from datetime import datetime
import json


app = Flask(__name__)

# Store conversation history (session-based memory)
conversation_history = []

# Load CSV data
def load_csv_data():
    try:
        temp_data = pd.read_csv('temp.csv')
        hum_data = pd.read_csv('hum.csv')
        shum_data = pd.read_csv('shum.csv')
        lum_data = pd.read_csv('lum.csv')
        return {
            'temperature': temp_data,
            'humidity': hum_data,
            'soil_humidity': shum_data,
            'light_intensity': lum_data
        }
    except FileNotFoundError as e:
        print(f"Error loading data: {e}")
        return None

# Read the plant type from a text file
def read_plant_type():
    try:
        with open('plant.txt', 'r') as file:
            plant_type = file.readline().strip()  # Read the first line and strip whitespace
            return plant_type
    except FileNotFoundError:
        print("Error: plant.txt file not found.")
        return None

# Configure Google Gemini API
genai.configure(api_key=os.environ["API_KEY"])
model = genai.GenerativeModel("gemini-1.5-flash")

# Custom prompt template with conversation memory
def generate_prompt(data, question, conversation_history):
    # Join past questions and responses for context
    conversation_context = "\n".join([f"Q: {entry['question']}\nA: {entry['response']}" for entry in conversation_history])

    prompt_data = "\n".join([f"{key}: {value}" for key, value in data.items() if value is not None])

    return f"""
    You are a smart planter assistant. You have access to sensor readings and plant information:
    {prompt_data}

    Here is the previous conversation history:
    {conversation_context}

    You are an AI assistant integrated into an IoT smart planter system. You are a bit funny. When the user asks about plant status, use this data to give them an overview of their plant's health. You can also answer general plant care questions using your knowledge.

    Question: {question}
    """

# Retrieve data or indicate empty status
def get_data_info(data_frame, category_name):
    if not data_frame.empty:
        return data_frame.to_dict(orient='records')  # Convert to list of records
    else:
        print(f"No data available for {category_name}.")
        return None  # Indicate that the data is empty

# Save conversation history to a file with UTF-8 encoding


def save_conversation_history(question, response):
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    entry = {
        'timestamp': timestamp,
        'question': question,
        'response': response
    }
    # Append the entry to the conversation history file
    with open('conversation_history.json', 'a') as file:
        file.write(json.dumps(entry) + '\n')  # Save each entry as a new line


# API endpoint to handle questions
@app.route('/ask', methods=['POST'])
def ask_question():
    global conversation_history  # Declare it as a global variable
    data = request.get_json()
    question = data.get('question')

    # Load the CSV data
    csv_data = load_csv_data()
    if csv_data is None:
        return jsonify({'error': 'Failed to load data.'}), 500

    # Read the plant type
    plant_type = read_plant_type()
    if plant_type is None:
        return jsonify({'error': 'Failed to read plant type.'}), 500

    # Extract data for each category
    latest_data = {
        'temperature': get_data_info(csv_data['temperature'], 'Temperature'),
        'humidity': get_data_info(csv_data['humidity'], 'Humidity'),
        'soil_humidity': get_data_info(csv_data['soil_humidity'], 'Soil Humidity'),
        'light_intensity': get_data_info(csv_data['light_intensity'], 'Light Intensity'),
        'plant_type': plant_type  # Add plant type to latest_data
    }

    # Generate the prompt using the latest data and conversation history
    prompt = generate_prompt(latest_data, question, conversation_history)

    # Run the Gemini model to generate a response
    response = model.generate_content(prompt)
    print("Response from Gemini:")
    print(response.text)

    # Save the current question and response to the conversation history
    response_text = response.text.strip()
    conversation_history.append({'question': question, 'response': response_text})

    # Save to file
    save_conversation_history(question, response_text)

    # Limit the conversation memory to a reasonable length (e.g., 5 previous exchanges)
    if len(conversation_history) > 5:
        conversation_history.pop(0)  # Remove the oldest exchange to maintain size

    return jsonify({'response': response_text})  # Stripping whitespace from the response

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5000)
