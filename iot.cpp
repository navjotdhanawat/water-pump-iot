#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define DHTPIN 4        // DHT sensor pin
#define DHTTYPE DHT22   // DHT 22 (AM2302)
#define RELAY_PIN 5     // Relay module pin

const char* ssid = "your_SSID";      // Replace with your Wi-Fi SSID
const char* password = "your_PASSWORD";  // Replace with your Wi-Fi Password
const char* mqtt_server = "192.168.1.100"; // Replace with your MQTT server IP address

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp; // I2C address 0x76

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Initially turn off the pump

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.begin();
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  // Control the pump based on the message
  if (strcmp(topic, "water-pump/control") == 0) {
    if (strcmp(message, "ON") == 0) {
      digitalWrite(RELAY_PIN, HIGH); // Turn on the pump
      client.publish("water-pump/status", "Pump is ON");
    } else if (strcmp(message, "OFF") == 0) {
      digitalWrite(RELAY_PIN, LOW); // Turn off the pump
      client.publish("water-pump/status", "Pump is OFF");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("water-pump/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read sensor data
  float temperature = dht.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
  float voltage = analogRead(34) * (3.3 / 4095.0) * (10 + 1); // Example: Voltage divider for ADC pin 34
  float current = analogRead(35) * (5.0 / 4095.0); // Example: Using ACS712 on ADC pin 35
  
  // Check if any reads failed and publish the data
  if (isnan(temperature) || isnan(pressure) || isnan(voltage) || isnan(current)) {
    Serial.println("Failed to read from sensor(s)");
  } else {
    char payload[256];
    snprintf(payload, sizeof(payload), "{\"temperature\": %.2f, \"pressure\": %.2f, \"voltage\": %.2f, \"current\": %.2f}", temperature, pressure, voltage, current);
    client.publish("sensor/data", payload);

    // Example: Check for abnormal conditions
    if (temperature > 60 || temperature < 5) {
      client.publish("alerts/temperature", "Temperature out of range!");
    }
    if (pressure > 1200 || pressure < 900) {
      client.publish("alerts/pressure", "Pressure out of range!");
    }
    if (voltage > 12.5 || voltage < 11.0) {
      client.publish("alerts/voltage", "Voltage out of range!");
    }
    if (current > 10.0) {
      client.publish("alerts/current", "Current too high!");
    }
  }
  
  delay(10000); // Send data every 10 seconds
}