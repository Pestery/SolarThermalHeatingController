
// TODO:
// Review: https://github.com/arduino-libraries/NTPClient

// This folder contains the code for both the Arduino and the ESP8266 (wifi chip)
// Simply set the Arduino IDE to the correct board, via menu: Tools -> Board -> ...
// The board selected should be one of:
//  - "Generic ESP8266 Module"
//  - "Arduino Mega or Mega 2560"

#if defined(ESP8266)
#include "main_esp8266.h"
#else
#include "main_arduino.h"
#endif
