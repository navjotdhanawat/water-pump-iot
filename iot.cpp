#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Preferences.h>
#include <WebServer.h>

// Libraries for the 4G module (these may vary depending on the module)
#include <SoftwareSerial.h>

#define DHTPIN 4        // DHT sensor pin
#define DHTTYPE DHT22   // DHT 22 (AM2302)
#define RELAY_PIN 5     // Relay module pin
#define MODEM_RST 26    // Pin to reset the 4G module (if applicable)
#define MODEM_PWRKEY 27 // Pin to power on the 4G module (if applicable)
#define MODEM_TX 32     // ESP32 TX pin connected to the 4G module RX
#define MODEM_RX 33     // ESP32 RX pin connected to the 4G module TX

// Global objects
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp; // I2C address 0x76
Preferences preferences;
WebServer server(80);
SoftwareSerial modemSerial(MODEM_RX, MODEM_TX);

unsigned long delayTime = 10000; // Default delay time (10 seconds)
bool use4G = false;              // Flag to indicate if 4G should be used

// Function prototypes
void setup_ap_mode();
void setup_wifi();
void setup_4g();
void handle_root();
void handle_save();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();
void saveWiFiCredentials(const char *ssid, const char *password);

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Initially turn off the pump

  Serial.begin(115200);
  modemSerial.begin(9600); // 4G module baud rate (adjust if necessary)

  preferences.begin("config", false);
  delayTime = preferences.getUInt("delayTime", 10000); // Load delay time from preferences

  if (preferences.getString("ssid").isEmpty())
  {
    setup_ap_mode(); // Start in Access Point mode if no SSID is saved
  }
  else
  {
    setup_wifi(); // Try to connect to saved Wi-Fi
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.begin();
  if (!bmp.begin())
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1)
      ;
  }
}

void setup_ap_mode()
{
  WiFi.softAP("ESP32_AP");

  Serial.println("Access Point started. Connect to 'ESP32_AP' and set your Wi-Fi credentials.");

  server.on("/", handle_root);
  server.on("/save", handle_save);
  server.begin();

  while (true)
  {
    server.handleClient();
  }
}

void handle_root()
{
  String html = "<html><body>"
                "<h1>ESP32 Wi-Fi Configuration</h1>"
                "<form action='/save' method='POST'>"
                "SSID: <input type='text' name='ssid'><br>"
                "Password: <input type='text' name='password'><br>"
                "<input type='submit' value='Save'>"
                "</form>"
                "</body></html>";
  server.send(200, "text/html", html);
}

void handle_save()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  if (ssid.length() > 0 && password.length() > 0)
  {
    saveWiFiCredentials(ssid.c_str(), password.c_str());
    server.send(200, "text/html", "Credentials saved. Restarting...");
    delay(2000);
    ESP.restart(); // Restart ESP32 to connect to the new Wi-Fi
  }
  else
  {
    server.send(200, "text/html", "Invalid input. Please try again.");
  }
}

void setup_wifi()
{
  char ssid[32] = {0};
  char password[64] = {0};

  preferences.getString("ssid", ssid, sizeof(ssid));
  preferences.getString("password", password, sizeof(password));

  WiFi.begin(ssid, password);

  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20)
  { // Max 20 attempts
    delay(500);
    Serial.print(".");
    wifi_attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected");
  }
  else
  {
    Serial.println("\nWiFi connection failed. Switching to 4G.");
    use4G = true;
    setup_4g();
  }
}

void setup_4g()
{
  // Example setup for a 4G module (this will vary depending on your specific module)
  // Send AT commands to initialize the 4G module and connect to the network
  modemSerial.println("AT+CPIN?"); // Check SIM card
  delay(1000);
  modemSerial.println("AT+CREG?"); // Check network registration
  delay(1000);
  modemSerial.println("AT+CIPSHUT"); // Shut down any previous connections
  delay(1000);
  modemSerial.println("AT+CSTT=\"your_APN\",\"your_USER\",\"your_PASS\""); // Set APN
  delay(1000);
  modemSerial.println("AT+CIICR"); // Bring up wireless connection
  delay(1000);
  modemSerial.println("AT+CIFSR"); // Get local IP address
  delay(1000);
  modemSerial.println("AT+CIPSTART=\"TCP\",\"192.168.1.100\",\"1883\""); // Start TCP connection to MQTT server
  delay(2000);
}

void saveWiFiCredentials(const char *ssid, const char *password)
{
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++)
  {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  // Control the pump based on the message
  if (strcmp(topic, "water-pump/control") == 0)
  {
    if (strcmp(message, "ON") == 0)
    {
      digitalWrite(RELAY_PIN, HIGH); // Turn on the pump
      client.publish("water-pump/status", "Pump is ON");
    }
    else if (strcmp(message, "OFF") == 0)
    {
      digitalWrite(RELAY_PIN, LOW); // Turn off the pump
      client.publish("water-pump/status", "Pump is OFF");
    }
  }
  else if (strcmp(topic, "sensor/delay") == 0)
  {
    delayTime = atol(message);                   // Convert message to integer for delay time
    preferences.putUInt("delayTime", delayTime); // Save delay time to preferences
    client.publish("sensor/delay/status", "Delay time updated");
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("connected");
      client.subscribe("water-pump/control");
      client.subscribe("sensor/delay"); // Subscribe to delay time changes
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // Read sensor data
  float temperature = dht.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
  float voltage = analogRead(34) * (3.3 / 4095.0) * (10 + 1); // Example: Voltage divider for ADC pin 34
  float current = analogRead(35) * (5.0 / 4095.0);            // Example: Using ACS712 on ADC pin 35

  // Check if any reads failed and publish the data
  if (isnan(temperature) || isnan(pressure) || isnan(voltage) || isnan(current))
  {
    Serial.println("Failed to read from sensor(s)");
  }
  else
  {
    char payload[256];
    snprintf(payload, sizeof(payload), "{\"temperature\": %.2f, \"pressure\": %.2f, \"voltage\": %.2f, \"current\": %.2f}", temperature, pressure, voltage, current);
    client.publish("sensor/data", payload);

    // Example: Check for abnormal conditions
    if (temperature > 60 || temperature < 5)
    {
      client.publish("alerts/temperature", "Temperature out of range!");
    }
    if (pressure > 1200 or pressure < 900)
    {
      client.publish("alerts/pressure", "Pressure out of range!");
    }
    if (voltage > 12.5 || voltage < 11.0)
    {
      client.publish("alerts/voltage", "Voltage out of range!");
    }
    if (current > 10.0)
    {
      client.publish("alerts/current", "Current too high!");
    }
  }

  delay(delayTime); // Use the configurable delay time
}
