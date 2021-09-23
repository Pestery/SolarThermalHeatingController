#ifndef SETTINGS_H
#define SETTINGS_H

// Include headers
#include "json_decoder.h"

// The Settings class holds information about the controller settings and current status.
class Settings {
public:

	// Minimum and maximum allowed values for the target temperature
	static constexpr float targetTemperatureMin = 10;
	static constexpr float targetTemperatureMax = 40;

	// Maximum length of the system GUID string
	static constexpr int guidMaxLength = 39;

	// Check if the server has been informed of the latest state information
	// Returns true if the server has been informed, or false if there is new data
	bool isServerInformed() const {
		return m_isServerInformed;
	}

	// Set that the server has been informed
	// This will be called when the latest status information is sent to the server
	void setServerInformed() {
		m_isServerInformed = true;
	}

	// System GUID value
	void systemGuid(const String& newValue) {
		if (newValue.length() < guidMaxLength) {
			const char* src = newValue.c_str();
			char* dest = m_systemGuid;
			while (*src) {
				*dest = *in;
				src++;
				dest++;
			}
			*dest = 0;
		}
	}
	const char* systemGuid() const {
		return m_systemGuid;
	}

	// Controller automatic-manual mode toggle
	// True if the controller should be running in automatic mode
	void modeAutomatic(bool newValue) {
		if (m_modeAutomatic != newValue) {
			m_modeAutomatic = newValue;
			m_isServerInformed = false;
		}
	}
	bool modeAutomatic() const {
		return m_modeAutomatic;
	}

	// Manual mode, pump on toggle
	// True if the pump should be running if the controller is in manual mode (not automatic)
	void manualPumpOn(bool newValue) {
		if (m_manualPumpOn != newValue) {
			m_manualPumpOn = newValue;
			m_isServerInformed = false;
		}
	}
	bool manualPumpOn() const {
		return m_manualPumpOn;
	}

	// Controller automatic temperature set value
	void targetTemperature(float newValue) {
		if (newValue < targetTemperatureMin) {
			m_targetTemperature = targetTemperatureMin;
		} else if (newValue > targetTemperatureMax) {
			m_targetTemperature = targetTemperatureMax;
		} else {
			m_targetTemperature = newValue;
		}
	}
	float targetTemperature() const {
		return m_targetTemperature;
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

		outAppend += F("{\"auto\":");
		outAppend += m_modeAutomatic ? F("true") : F("false");

		outAppend += F(",\"setTemp\":");
		outAppend += String(m_targetTemperature);

		outAppend += '}';
		return outAppend;
	}

	// Setup the data within this class using a JSON-string
	bool fromJson(const String& in) {
		JsonDecoder decoder(in);
		while (decoder.fetch()) {

			if (decoder.name() == F("\"auto\"")) {
				modeAutomatic(decoder.value() == F("true"));

			} else if (decoder.name() == F("\"pumpOn\"")) {
				manualPumpOn(decoder.value() == F("true"));

			} else if (decoder.name() == F("\"setTemp\"")) {
				targetTemperature(decoder.value().toFloat());
			}
		}
		return !decoder.hadError();
	}

	// Default constructor
	Settings() :
		m_targetTemperature(26),
		m_modeAutomatic(false),
		m_manualPumpOn(false),
		m_isServerInformed(false) {
	}

private:
	float m_targetTemperature;
	bool m_modeAutomatic;
	bool m_manualPumpOn;
	bool m_isServerInformed;
	char m_systemGuid[guidMaxLength];
};

#endif
