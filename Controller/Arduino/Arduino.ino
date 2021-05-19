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
  Serial3.println("POST /api/TemperatureSensors HTTP/1.1");
  Serial3.println("Content-Type: application/json");
  String body;
  body = "{\"id\":";
  body += ++sentRequests;
  body += ", \"value\":";
  body += temperature;
  body += ", \"readDate\":\"2005-09-01T13:00:00\"}";
  Serial3.print("Content-Length: ");
  Serial3.println(body.length());
  Serial3.println();
  Serial3.println(body);
  Serial3.write(0); // Signal to the ESP8266 that this is the end of the transmission (a zero/null character)

/***** Example POST request *******
POST /echo/post/json HTTP/1.1
Host: website.com
Accept: application/json
Content-Type: application/json
Content-Length: 81

{
    "id":9,
    "value":50.0,
    "readDate":"2005-09-01T13:00:20"
}
***********************/

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
