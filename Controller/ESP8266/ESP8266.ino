#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "TelstraD13601"
#define STAPSK  "hftrjwfh43"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.0.156";
const uint16_t port = 27015;

void setup() {
  Serial.begin(74880);

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
