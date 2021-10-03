#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

// This file contains configuration information for the target Arduino board
// This may include which pins are used for connecting which modules and so forth

// Bit-rate for serial communication between Arduino, ESP8266 and PC
// Must be kept the same for all serial connections because otherwise buffer overflows may occur
// when forwarding data from one serial to another, if one bit-rate is higher than the another
#define SERIAL_BITRATE 9600

// Serial connections to use, depending on board type
#if defined(ESP8266)
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

	// For the Arduino Mega:
	#define SERIAL_PC        Serial  // TX/RX   - Serial 0. USB connection
	#define SERIAL_WIFI      Serial3 // TX3/RX3 - Serial 3. Use dip switch to select RXD3/TXD3 (not TXD0/RXD0)
	#define SERIAL_BLUETOOTH Serial1 // TX1/RX1 - Serial 1

#else

	// For the Arduino Uno
	// Need to define a software-serial connection because the Uno does not have enough hardware-serial connections
	// Also, the PC and Wifi connections both use serial 0, which is not ideal
	SoftwareSerial serialBluetooth(2, 3); // TODO: Pick the correct pins for the Uno
	#define SERIAL_PC        Serial
	#define SERIAL_WIFI      Serial
	#define SERIAL_BLUETOOTH serialBluetooth

#endif

// SD card configuration
// Depending on the type of board used, the pins used maybe different
// The below information is both used for the code, and as a guide for the user when connecting the SD card to the board
// Source: https://lastminuteengineers.com/arduino-micro-sd-card-module-tutorial/
#if defined(ESP8266)
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

	// For the Arduino Mega:
	#define SD_PIN_MOSI 51
	#define SD_PIN_MISO 50
	#define SD_PIN_SCK  52
	#define SD_PIN_CS   53 // Chip select

#else

	// For the Arduino Uno
	#define SD_PIN_MOSI 11
	#define SD_PIN_MISO 12
	#define SD_PIN_SCK  13
	#define SD_PIN_CS   10 // Chip select

#endif


#endif
