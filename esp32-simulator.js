const mqtt = require("mqtt");
const logger = require("./logger"); // Import the logger utility

// MQTT server details
const mqttServer = "mqtt://localhost:1883"; // Adjust if your server is running on a different IP or port
const client = mqtt.connect(mqttServer);

// Simulated sensor data
function generateSensorData() {
  const temperature = Math.random() * 55 + 5; // Temperature between 5°C and 60°C
  const pressure = Math.random() * 300 + 900; // Pressure between 900 and 1200 hPa
  const voltage = Math.random() * 1.5 + 11; // Voltage between 11V and 12.5V
  const current = Math.random() * 10; // Current between 0A and 10A

  return { temperature, pressure, voltage, current };
}

// Connect to the MQTT broker
client.on("connect", function () {
  logger.log("Connected to MQTT broker");

  // Subscribe to control messages
  client.subscribe("water-pump/control", function (err) {
    if (!err) {
      logger.log("Subscribed to water-pump/control");
    }
  });

  // Publish sensor data every 10 seconds
  // setInterval(() => {
  //   const data = generateSensorData();
  //   const payload = JSON.stringify(data);
  //   client.publish("sensor/data", payload);
  //   logger.log("Published sensor data:", payload);

  //   // Check for abnormal conditions
  //   if (data.temperature > 60 || data.temperature < 5) {
  //     client.publish("alerts/temperature", "Temperature out of range!");
  //     logger.log("Published alert: Temperature out of range!");
  //   }
  //   if (data.pressure > 1200 || data.pressure < 900) {
  //     client.publish("alerts/pressure", "Pressure out of range!");
  //     logger.log("Published alert: Pressure out of range!");
  //   }
  //   if (data.voltage > 12.5 || data.voltage < 11.0) {
  //     client.publish("alerts/voltage", "Voltage out of range!");
  //     logger.log("Published alert: Voltage out of range!");
  //   }
  //   if (data.current > 10.0) {
  //     client.publish("alerts/current", "Current too high!");
  //     logger.log("Published alert: Current too high!");
  //   }
  // }, 10000);
});

// Handle received messages
client.on("message", function (topic, message) {
  logger.log(`Received message on ${topic}: ${message.toString()}`);

  if (topic === "water-pump/control") {
    if (message.toString() === "ON") {
      logger.log("Simulated action: Water pump turned ON");
      client.publish("water-pump/status", "Pump is ON");
    } else if (message.toString() === "OFF") {
      logger.log("Simulated action: Water pump turned OFF");
      client.publish("water-pump/status", "Pump is OFF");
    }
  }
});
