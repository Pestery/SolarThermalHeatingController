#ifndef STATUS_H
#define STATUS_H

// Include headers
#include "json_decoder.h"
#include "utility.h"

// The Status class holds information about the controller settings and status.
class Status {
public:

	// Minimum and maximum allowed values for the target temperature
	constexpr float targetTemperatureMin = 10;
	constexpr float targetTemperatureMax = 40;

	// Controller automatic-manual mode toggle
	// True if the controller should be running in automatic mode
	void modeAutomatic(bool newValue) {
		m_modeAutomatic = newValue;
	}
	bool modeAutomatic() const {
		return m_modeAutomatic;
	}

	// Manual mode, pump on toggle
	// True if the pump should be running if the controller is in manual mode (not automatic)
	void manualPumpOn(bool newValue) {
		m_manualPumpOn = newValue;
	}
	bool manualPumpOn() const {
		return m_manualPumpOn;
	}

	// Controller automatic temperature set value
	void targetTemperature(float newValue) {
		m_targetTemperature = Misc::clamp(newValue, targetTemperatureMin, targetTemperatureMax);
	}
	float targetTemperature() const {
		return m_targetTemperature;
	}

	// Generate a JSON-string representation of the data within this class
	String toJSON() const {
		String out;
		out.reserve(200); // Adjust this to best guess of number of bytes required

		out = F("{\"auto\":");
		out += m_modeAutomatic ? F("true") : F("false");

		out += F(",\"setTemp\":");
		out += String(m_targetTemperature);

		out += '}';
		return out;
	}

	// Setup the data within this class using a JSON-string
	bool fromJSON(const String& in) {
		JsonDecoder decoder(in);
		while (decoder.fetch()) {

			if (decoder.name() == F("\"auto\"")) {
				modeAutomatic(decoder.value() == F("true"));

			} else if (decoder.name() == F("\"setTemp\"")) {
				targetTemperature(decoder.value().toFloat());
			}
		}
		return !decoder.hadError();
	}

	// Default constructor
	DateTime() :
		m_targetTemperature(26),
		m_modeAutomatic(true) {
	}

private:
	float m_targetTemperature;
	bool m_modeAutomatic;
	bool m_manualPumpOn;
};

#endif
