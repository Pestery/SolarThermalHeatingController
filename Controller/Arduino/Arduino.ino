#include <math.h>

int temperaturePin = A0; // Analog pin which the sensor is connected to

int sentRequests = 0; // Temporary. Just used to make the id different for each POST

void setup() {
  Serial.begin(9600); // For PC connection
  Serial3.begin(9600); // For ESP8266 connection
}

void loop() {

  // Get current temperature
  float temperature = readThermistor(temperaturePin);

  // Send to PC
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  // Send to ESP8266
  Serial3.write(0); // Send an end character to reset the serial timeout and make sure the ESP8266 gets the whole message
  String body;
  body = "{\"id\":" + String(++sentRequests);
  body += ",\"value\":" + String(temperature);
  body += ",\"readDate\":\"2005-09-01T13:00:00\"}";
  String request = buildHttpPostRequest(body);
  Serial3.print(request);
  Serial3.write(0); // Signal to the ESP8266 that this is the end of the transmission (a zero/null character)


  delay(2000);
}

// Convert the raw analog input from a thermistor temperature sensor to a temperature (C) value
float readThermistor(int analogPin) {

  // Get raw reading from analog pin
  // Is a 10-bit number and will be in the range 0-1023
  float temp = analogRead(analogPin);

  // Convert the 0-1023 to a temperature value in Kelvin
  // Taken from product manual
  temp = log((10240000.0f/temp) - 10000.0f);
  temp = 1.0f / (0.001129148f + (0.000234125f + (0.0000000876741f * temp * temp)) * temp);

  // Return the temperature in Celcuis
  return temp - 273.15;
}

// Build a HTTP POST request string which contains JSON data
// TODO: Once better communication between Arduino and ESP8266 is done, this method should be moved to the ESP8266
String buildHttpPostRequest(String jsonData) {

  /***** Example POST request *******
  POST /some/page/address HTTP/1.1
  Host: website.com:80
  Accept: application/json
  Content-Type: application/json
  Content-Length: 10
  
  {"id":123}
  ****************************
  * Don't need Accept header *
  * Do need Host header      *
  * Line ending must be \r\n *
  ***************************/
  
  String request = "POST /api/TemperatureSensors/ HTTP/1.1\r\n";
    request += "Host: localhost:5000\r\n"; // TODO: Need to change this line to list the actual server address and port
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + String(jsonData.length()) + "\r\n";
    request += "\r\n" + jsonData;
  return request;
}
