# ESP32 Water Pump Starter IoT Project

## Overview

This project simulates an IoT-based water pump starter system using an ESP32 microcontroller. The system can be controlled via a mobile device using MQTT protocol and provides real-time monitoring of temperature, pressure, voltage, and current. This repository includes:

1. **Node.js MQTT Mock Server**: A server that handles communication between the ESP32 (or its simulator) and the mobile app.
2. **ESP32 Simulator**: A Node.js script that simulates the behavior of the ESP32, allowing you to test the system on macOS without needing actual hardware.

## Features

- **Remote Control**: Start and stop the water pump via MQTT commands.
- **Real-time Monitoring**: Monitor temperature, pressure, voltage, and current in real-time.
- **Alerts**: Receive alerts when sensor readings exceed predefined thresholds.
- **Simulation**: Test the entire setup on macOS using a simulated ESP32.

## Tech Stack

- **Node.js**: Backend for the MQTT mock server and ESP32 simulator.
- **MQTT**: Protocol for message communication between devices.
- **ESP32**: IoT microcontroller for the actual hardware implementation (simulated here).

## Getting Started

### Prerequisites

Ensure you have the following installed on your system:

- **Node.js**: [Download Node.js](https://nodejs.org/)
- **NPM**: Comes with Node.js
- **MQTT Client**: Optionally, use an MQTT client like [MQTT Explorer](http://mqtt-explorer.com/) to monitor MQTT messages.

### Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/yourusername/esp32-water-pump-starter.git
   cd esp32-water-pump-starter
   ```

2. **Install Dependencies:**
   ```bash
   npm install
   ```

### Running the MQTT Mock Server

The MQTT mock server simulates an MQTT broker that handles the communication between the ESP32 (or its simulator) and the mobile app.

1. **Start the MQTT Server:**

   ```bash
   node server.js
   ```

2. **Output:**
   - The server will start on the default MQTT port `1883`.
   - You will see log messages when a client connects, disconnects, or sends a message.

### Running the ESP32 Simulator

The ESP32 simulator mimics the behavior of an actual ESP32 device, publishing sensor data and responding to control commands.

1. **Start the ESP32 Simulator:**

   ```bash
   node esp32-simulator.js
   ```

2. **Output:**
   - The simulator will publish simulated sensor data every 10 seconds.
   - It listens for pump control commands on the `water-pump/control` topic and responds accordingly.

### Testing the System

- **Control the Pump:**

  - Send `ON` or `OFF` messages to the `water-pump/control` topic to simulate starting or stopping the pump.
  - The simulator will respond and publish the status to the `water-pump/status` topic.

- **Monitor Sensor Data:**

  - Sensor data is published to the `sensor/data` topic every 10 seconds.
  - Alerts are triggered and published to specific topics (e.g., `alerts/temperature`) if sensor readings are out of range.

- **Use MQTT Explorer (Optional):**
  - Connect to the MQTT server using MQTT Explorer or any other MQTT client to monitor and interact with the topics.

## ESP32 Code (For Actual Device)

If you later want to deploy this project on actual ESP32 hardware, the code for the ESP32 is provided in the repository under `esp32-code/`.

### Requirements

- **Arduino IDE**: [Download Arduino IDE](https://www.arduino.cc/en/software)
- **ESP32 Board Library**: Install the ESP32 board library in the Arduino IDE.
- **Required Libraries**: Install libraries for MQTT, DHT sensor, BMP280 sensor, and ACS712 current sensor.

### Uploading to ESP32

1. **Open the `esp32-water-pump.ino` file** in Arduino IDE.
2. **Configure Wi-Fi credentials** in the code.
3. **Upload the code** to the ESP32 using the Arduino IDE.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

This README file should help guide users and contributors through the process of setting up and testing your water pump starter IoT project. It also provides the necessary context for anyone looking to understand or extend the project.
