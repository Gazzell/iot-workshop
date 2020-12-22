
#include "DHT.h"
#define BLYNK_PRINT Serial
#include "arduino_secrets.h"

#include <Arduino_LSM6DS3.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

#define DHTPIN 6
#define DHTTYPE DHT11   // DHT 11

char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
char auth[] = AUTH_TOKEN;

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void myTimerEvent() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, t);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C"));
}

void setup() {
  Serial.begin(9600);

  dht.begin();
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(2000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();
}
