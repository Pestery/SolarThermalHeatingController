#include <ESP8266WiFi.h>

// TODO:
// Need to convert the ESP8266 into a passthrough card for the Arduino
// Communication both ways via Serial
// An escape character should be used to allow commands to be sent amongst the data transmission
// Arduino should be able to change the wifi SSID and password, as well as know the status of the wifi


// This file includes the connection info for the Arduino, but it is not included in the GitHub source
// This is to allow different developers with different local wifi networks to setup the project accordingly
// To create this file, make a copy of "connection_info_template.h.txt" and rename it to "connection_info.h"
// Within the copy, fill out the information as desired
#include "connection_info.h"

// Get connection info from macros in related file
const char*    wifiSSID        = WIFI_SSID;
const char*    wifiPassword    = WIFI_PASSWORD;
const char*    databaseAddress = DATABASE_ADDRESS;
const uint16_t databasePort    = DATABASE_PORT;

// Runs once at Arduino power-up
// This function is used to setup all data before loop() is called
void setup() {

  // Open a serial connection to the Arduino chip
  // Set timeout used with readStringUntil()
  Serial.begin(9600);
  Serial.setTimeout(5000);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);

  // Setup the wifi connection to the local network
  Serial.println();
  Serial.print("connecting...");
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
}

// Runs repeatedly once the Arduino has finished startup
// This function will only run after setup() has completed
void loop() {

  // Get next string to send
  String message = Serial.readStringUntil(0);
  if (message.length() > 0) {

    // Use WiFiClient class to create TCP connection
    WiFiClient client;
    if (client.connect(databaseAddress, databasePort)) {

      // Send message to the server
      if (client.connected()) client.println(message);

      // Close the connection
      client.stop();
    }
  }
}
