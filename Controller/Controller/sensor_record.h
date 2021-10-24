#ifndef SENSOR_RECORD_H
#define SENSOR_RECORD_H

// Include headers
#include "misc.h"
#include "date_time.h"
#include "byte_queue.h"
#include "time_keeper.h"

// Holds information about all sensors at a single moment in time.
struct SensorRecord {

	// Data
	DateTime dateTime;
	float temperatureInlet;
	float temperatureOutlet;
	float temperatureRoof;
	float solarIrradiance;

	// Reset all values to default
	void reset() {
		dateTime.reset();
		temperatureInlet = 0;
		temperatureOutlet = 0;
		temperatureRoof = 0;
		solarIrradiance = 0;
	}

	// Initialise this class by decoding a CSV-string representation of the data.
	// The string should have been generated using a previous call to toString().
	// Returns true on success, or false on failure
	bool fromCsv(const String& data) {
		int start = 0;
		String s;

		// Enter a loop just for the scope
		do {

			// Get dateTime
			s = Misc::getNextSubString(data, start, ',');
			s.trim();
			if (s.length() == 0) break;
			dateTime = DateTime(s);
			if (dateTime == 0) break; // A dateTime of zero is invalid

			// Get temperatureInlet
			s = Misc::getNextSubString(data, start, ',');
			s.trim();
			temperatureInlet = (s.length() > 0) ? s.toFloat() : 0;

			// Get temperatureOutlet
			s = Misc::getNextSubString(data, start, ',');
			s.trim();
			temperatureOutlet = (s.length() > 0) ? s.toFloat() : 0;

			// Get temperatureRoof
			s = Misc::getNextSubString(data, start, ',');
			s.trim();
			temperatureRoof = (s.length() > 0) ? s.toFloat() : 0;

			// Get solarIrradiance
			s = Misc::getNextSubString(data, start, ',');
			s.trim();
			solarIrradiance = (s.length() > 0) ? s.toFloat() : 0;

			// If here then success
			return true;
		} while (false);

		// If here then there was an error
		reset();
		return false;
	}

	// Generate the headers for the CSV-string representation of the data within this class
	static String toCsvHeaders() {
		return String(F("DateTime,TempInlet,TempOutlet,TempRoof,Solar"));
	}

	// Generate a CSV-string representation of the data within this class
	String toCsv() const {
		String result;
		result.reserve(20); // Adjust this to best guess of number of bytes required
		result += dateTime.toString();
		result += ',';
		result += String(temperatureInlet);
		result += ',';
		result += String(temperatureOutlet);
		result += ',';
		result += String(temperatureRoof);
		result += ',';
		result += String(solarIrradiance);
		return result;
	}

	// Generate a JSON-string representation of the data within this class
	ByteQueue toJson() const {
		ByteQueue result;
		toJson(result);
		return result;
	}

	// Generate a JSON-string representation of the data within this class
	// The generated JSON data will be added to the end of the string 'outAppend'
	void toJson(ByteQueue& outAppend) const {

		outAppend.print(F("{\"Date\":\""));
		outAppend.print(dateTime.toString());
		outAppend.print('\"');

		outAppend.print(F(",\"Tin\":"));
		outAppend.print(temperatureInlet);

		outAppend.print(F(",\"Tout\":"));
		outAppend.print(temperatureOutlet);

		outAppend.print(F(",\"Troof\":"));
		outAppend.print(temperatureRoof);

		outAppend.print(F(",\"Solar\":"));
		outAppend.print(solarIrradiance);

		outAppend.print('}');
	}

	// Read the value of all connected sensors and store it within this class
	// This will also increment the index value
	void readAll() {readAll(nullptr);}
	void readAll(TimeKeeper& timeKeeper) {readAll(&timeKeeper);}
	void readAll(TimeKeeper* timeKeeper) {

		// Record current time within record
		if (timeKeeper) dateTime = timeKeeper->current();

		// TODO: For testing only!!!
		// Sin wave with period of 12 hours (Pi every 6 hours)
		float t = 0.5f * (1.0f + sinf(3.14159265359f * (float)dateTime.data() / (float)(6ul * 60ul * 60ul)));

		// Record sensor values
		temperatureInlet = readThermistor(A0); // A0 seems to be defined
		temperatureOutlet = readThermistor(A1);
		temperatureRoof = 20.0f + (t * 10.0f);
		solarIrradiance = 2000.0f * t;
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

	// Boolean cast operator
	// Returns true if the record is valid, or false if not
	operator bool() const {return dateTime != 0;}

	// Constructors and operators
	SensorRecord() {}
	SensorRecord(const String& data) {
		fromCsv(data);
	}
	SensorRecord(const SensorRecord& rhs) {
		*this = rhs; // Redirect to below: operator = (...)
	}
	SensorRecord& operator = (const SensorRecord& rhs) {
		if (this != &rhs) {
			dateTime = rhs.dateTime;
			temperatureInlet = rhs.temperatureInlet;
			temperatureOutlet = rhs.temperatureOutlet;
			temperatureRoof = rhs.temperatureRoof;
			solarIrradiance = rhs.solarIrradiance;
		}
		return *this;
	}
};

#endif
