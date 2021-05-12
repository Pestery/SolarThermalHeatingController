#include <math.h>

int temperaturePin = A0; // Analog pin which the sensor is connected to

void setup() {
  Serial.begin(74880); // For PC connection
  Serial3.begin(74880); // For ESP8266 connection
}

void loop() {

  // Get current temperature
  float temperature = readThermistor(temperaturePin);

  // Send to PC
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  // Send to ESP8266
  Serial3.print("GET /newdata.php?value=");
  Serial3.print(temperature);
  Serial3.println(" HTTP/1.1");
  
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
