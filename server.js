const aedes = require("aedes")();
const server = require("net").createServer(aedes.handle);
const port = 1883;

server.listen(port, function () {
  console.log("MQTT Mock Server running on port", port);
});

// Event: Client connected
aedes.on("client", function (client) {
  console.log("Client connected:", client.id);
});

// Event: Client disconnected
aedes.on("clientDisconnect", function (client) {
  console.log("Client disconnected:", client.id);
});

// Event: Message received
aedes.on("publish", function (packet, client) {
  console.log("Received message:", packet.payload.toString());

  // Example: Forward messages to a specific topic
  if (packet.topic === "water-pump/control") {
    // Forward control messages to a topic for ESP32
    aedes.publish({
      topic: "water-pump/status",
      payload: Buffer.from("Pump control command received"),
    });
  }

  if (packet.topic === "sensor/data") {
    // Forward sensor data to a topic for mobile app
    aedes.publish({ topic: "mobile/sensor-data", payload: packet.payload });
  }
});
