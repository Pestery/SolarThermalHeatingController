#ifndef SENSOR_RECORD_H
#define SENSOR_RECORD_H

// Include headers
#include "misc.h"
#include "date_time.h"

// Holds information about all sensors at a single moment in time.
struct SensorRecord {

	// The type of value used for the sensor index
	typedef uint32_t IndexType;

	// Data
	IndexType index;
	DateTime dateTime;
	float temperatureIn;

	// Reset all values to default
	void reset() {
		index = 0;
		dateTime.reset();
		temperatureIn = 0;
	}

	// Initialise this class by decoding a CSV-string representation of the data.
	// The string should have been generated using a previous call to toString().
	void fromCSV(const String& data) {
		int start = 0;
		index = Misc::getNextSubString(data, start).toInt();
		dateTime = DateTime(Misc::getNextSubString(data, start));
		temperatureIn = Misc::getNextSubString(data, start).toFloat();
	}

	// Generate a CSV-string representation of the data within this class
	String toCSV() const {
		String result;
		result.reserve(20); // Adjust this to best guess of number of bytes required
		result = String(index);
		result += ',';
		result += dateTime.toString();
		result += ',';
		result += String(temperatureIn);
		return result;
	}

	// Generate a JSON-string representation of the data within this class
	String toJson() const {
		String result;
		toJson(result);
		return result;
	}

	// Generate a JSON-string representation of the data within this class
	// The generated JSON data will be added to the end of the string 'outAppend'
	void toJson(String& outAppend) const {

		outAppend += F("{\"Id\":");
		outAppend += String(index);

		outAppend += F(",\"Date\":\"");
		outAppend += dateTime.toString();
		outAppend += '\"';

		outAppend += F(",\"Tin\":");
		outAppend += String(temperatureIn);

		outAppend += F(",\"Tout\":");
		outAppend += String(temperatureIn);

		outAppend += F(",\"Troof\":");
		outAppend += String(temperatureIn);

		outAppend += '}';
	}

	// Read the value of all connected sensors and store it within this class
	// This will also increment the index value
	void readAll() {
		temperatureIn = readThermistor(A0); // A0 seems to be defined
	}

	// Convert the raw analogue input from a thermistor temperature sensor to a temperature value in Celsius
	static float readThermistor(int analogPin) {

		// Get raw reading from analogue pin
		// Is a 10-bit number and will be in the range 0-1023
		float temp = analogRead(analogPin);

		// Convert the 0-1023 value to a temperature value in Kelvin
		// Taken from product manual
		temp = log((10240000.0f/temp) - 10000.0f);
		temp = 1.0f / (0.001129148f + (0.000234125f + (0.0000000876741f * temp * temp)) * temp);

		// Return the temperature in Celsius
		return temp - 273.15;
	}

	// Constructors and operators
	SensorRecord() {}
	SensorRecord(const String& data) {
		fromCSV(data);
	}
	SensorRecord(const SensorRecord& rhs) {
		*this = rhs; // Redirect to below: operator = (...)
	}
	SensorRecord& operator = (const SensorRecord& rhs) {
		if (this != &rhs) {
			index = rhs.index;
			dateTime = rhs.dateTime;
			temperatureIn = rhs.temperatureIn;
		}
		return *this;
	}
};

#endif
