#include <ArduinoJson.h>

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
char weather_api_key[] = WEATHER_API_KEY;

char server[] = "api.openweathermap.org";
int status = WL_IDLE_STATUS;

WiFiClient client;

void setup() {
  Serial.begin(9600);      // initialize serial communication

  while (!Serial) {
    ;
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting server connection");
  if (client.connect(server, 80)) {
    char getString[] = "GET /data/2.5/onecall?lat=43.329791&lon=-2.993267&exclude=hourly,minutely,daily&units=metric&appid=";
    strcat(getString, weather_api_key);
    strcat(getString, " HTTP/1.1");

    client.println(F(getString));
    client.println(F("Host: api.openweathermap.org"));
    client.println(F("Connection: close"));
    client.println();
  }

  // Check HTTP status
  char status[32] = { 0 };
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(F(status));
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  StaticJsonDocument<128> filter;
  filter["current"]["weather"][0]["description"] = true;
  filter["current"]["temp"] = true;
  filter["current"]["humidity"] = true;

  const size_t capacity = 256;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client, DeserializationOption::Filter(filter));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Extract values
  Serial.println(F("Response:"));
  Serial.print(F("Weather: "));
  Serial.println(F(doc["current"]["weather"][0]["description"].as<char*>()));
  Serial.print(F("Temperature: "));
  Serial.print(doc["current"]["temp"].as<float>());
  Serial.println(F("°C"));
  Serial.print(F("Humidity: "));
  Serial.print(doc["current"]["humidity"].as<int>());
  Serial.println(F("%"));

  client.stop();
}


void loop() {
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print(F("SSID: "));
  Serial.println(F(WiFi.SSID()));

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print(F("IP Address: "));
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print(F("signal strength (RSSI):"));
  Serial.print(rssi);
  Serial.println(F(" dBm"));
}
