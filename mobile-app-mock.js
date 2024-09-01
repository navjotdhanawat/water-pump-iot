const mqtt = require("mqtt");
const readline = require("readline");
const logger = require("./logger"); // Import the logger utility

// MQTT server details
const options = {
  host: "localhost",
  port: 1883,
  protocol: "mqtt",
  username: "navjot",
  password: "navjot",
};
const client = mqtt.connect(options);

// Command-line interface for user input
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

// Handle connection to the MQTT broker
client.on("connect", function () {
  logger.log("Connected to MQTT broker");

  // Subscribe to sensor data topics
  client.subscribe("sensor/data", function (err) {
    if (!err) {
      logger.log("Subscribed to sensor/data");
    }
  });

  // Subscribe to alert topics
  client.subscribe("alerts/#", function (err) {
    if (!err) {
      logger.log("Subscribed to all alert topics");
    }
  });

  // Subscribe to water pump status
  client.subscribe("water-pump/status", function (err) {
    if (!err) {
      logger.log("Subscribed to water-pump/status");
    }
  });

  // Start command-line interface for controlling the pump
  askForCommand();
});

// Function to ask user for input
function askForCommand() {
  rl.question("Enter command (ON/OFF): ", function (command) {
    command = command.trim().toUpperCase();
    if (command === "ON" || command === "OFF") {
      client.publish("water-pump/control", command);
      logger.log(`Sent command: ${command}`);
    } else {
      logger.log('Invalid command. Please enter "ON" or "OFF".');
    }
    askForCommand(); // Recur to keep asking for commands
  });
}

// Handle incoming messages from the MQTT broker
client.on("message", function (topic, message) {
  logger.log(`Received on ${topic}: ${message.toString()}`);
});

// Handle client errors
client.on("error", function (err) {
  console.error("Connection error: ", err);
  client.end();
});
