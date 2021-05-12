#include <ESP8266WiFi.h>
#include "connection_info.h"

#ifndef STASSID
#define STASSID "wifi-network-name"
#define STAPSK  "wifi-network-password"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "server-address"; // Server address where the data is sent
const uint16_t port = 80;

void setup() {
  Serial.begin(9600);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set timeout used with readStringUntil()
  Serial.setTimeout(10000);
}

void loop() {

  // Get next string to send
  String message = Serial.readStringUntil('\n');
  if (message.length() < 1) return;
  
  // Use WiFiClient class to create TCP connection
  WiFiClient client;
  if (!client.connect(host, port)) return;

  // Send message to the server
  if (client.connected()) client.println(message);

  // Close the connection
  client.stop();
}
