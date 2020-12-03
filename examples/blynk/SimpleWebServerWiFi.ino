#define BLYNK_PRINT Serial
#include "arduino_secrets.h"

#include <Arduino_LSM6DS3.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <BlynkSimpleWiFiNINA.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = AUTH_TOKEN;

BlynkTimer timer;

void myTimerEvent()
{
  float x, y, z;
  if (!IMU.accelerationAvailable()) {
    return;
  }

  IMU.readAcceleration(x, y, z);
  Blynk.virtualWrite(V1, x);
  Blynk.virtualWrite(V2, y);
  Blynk.virtualWrite(V3, z);
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, myTimerEvent);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
  timer.run();
}
