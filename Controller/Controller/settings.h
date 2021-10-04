#ifndef SETTINGS_H
#define SETTINGS_H

// Compile target check
#if defined(ESP8266)
#error "This file should NOT be included with ESP8266 code"
#endif

// Include headers
#include <EEPROM.h>
#include "json_decoder.h"
#include "byte_queue.h"

// The Settings class holds information about the controller settings and current status.
class Settings {
public:

	// Minimum and maximum allowed values for the target temperature
	static constexpr float targetTemperatureMin = 10;
	static constexpr float targetTemperatureMax = 40;

	// Maximum length of the system GUID string
	static constexpr int guidMaxLength = 39;

	// The EEPROM address used for storing controller settings
	static constexpr int EepromAddress = 0;

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

		// Check if any changes are being made
		if (strcmp(m_systemGuid, newValue.c_str())) {
			m_unsavedSettings = true;

			// Copy new value to internal buffer
			if (newValue.length() < guidMaxLength) {
				const char* src = newValue.c_str();
				char* dest = m_systemGuid;
				while (*src) {
					*dest = *src;
					src++;
					dest++;
				}
				*dest = 0;
			}
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
			m_unsavedSettings = true;
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
			m_unsavedSettings = true;
		}

	}
	bool manualPumpOn() const {
		return m_manualPumpOn;
	}

	// Pump status
	// True if the pump is currently running, or false if not
	void pumpStatus(bool newValue) {
		if (m_pumpStatus != newValue) {
			m_pumpStatus = newValue;
			m_isServerInformed = false;
		}

	}
	bool pumpStatus() const {
		return m_pumpStatus;
	}

	// Controller automatic temperature set value
	void targetTemperature(float newValue) {

		// Record the old value
		// Change the current value
		float oldValue = m_targetTemperature;
		if (newValue < targetTemperatureMin) {
			m_targetTemperature = targetTemperatureMin;
		} else if (newValue > targetTemperatureMax) {
			m_targetTemperature = targetTemperatureMax;
		} else {
			m_targetTemperature = newValue;
		}

		// Check if any changes were made
		if (oldValue == m_targetTemperature) {
			m_unsavedSettings = true;
		}
	}
	float targetTemperature() const {
		return m_targetTemperature;
	}

	// Automatic data upload
	// True if the controller will try to send data to the server, or false if not
	void autoUpload(bool newValue) {
		if (m_autoUpload != newValue) {
			m_autoUpload = newValue;
			m_unsavedSettings = true;
		}

	}
	bool autoUpload() const {
		return m_autoUpload;
	}

	// Generate a JSON-string representation of the data within this class
	ByteQueue toJson(bool getAll = false) const {
		ByteQueue result;
		toJson(result, getAll);
		return result;
	}

	// Generate a JSON-string representation of the data within this class
	// The generated JSON data will be added to the end of the string 'outAppend'
	// Note: This is the data which is sent to the server, from the controller
	void toJson(ByteQueue& outAppend, bool getAll = false) const {

		outAppend.print(F("{\"pumpOn\":"));
		outAppend.print(m_pumpStatus ? F("true") : F("false"));

		if (getAll) {

			outAppend.print(F(",\"auto\":"));
			outAppend.print(m_modeAutomatic ? F("true") : F("false"));

			outAppend.print(F(",\"setTemp\":"));
			outAppend.print(m_targetTemperature);

			outAppend.print(F(",\"guid\":\""));
			outAppend.print(m_systemGuid);
			outAppend.print('\"');

			outAppend.print(F(",\"upload\":\""));
			outAppend.print(m_autoUpload);
			outAppend.print('\"');
		}

		outAppend.print('}');
		return outAppend;
	}

	// Update the settings class using a key and value pair
	// Returns true if the key-value was used, or false if it does not apply to this class
	bool updateWithKeyValue(String& key, String& value) {

		if (key == F("\"auto\"")) {
			modeAutomatic(value == F("true"));

		} else if (key == F("\"pumpOn\"")) {
			manualPumpOn(value == F("true"));

		} else if (key == F("\"setTemp\"")) {
			targetTemperature(value.toFloat());

		} else {
			return false;
		}
		return true;
	}

	// Setup the data within this class using a JSON-string
	// Note: This is the data which the server may send, to the controller
	bool fromJson(const ByteQueue& in) {
		JsonDecoder decoder(in);
		while (decoder.fetch()) {
			updateWithKeyValue(decoder.name(), decoder.value());
		}
		if (decoder.hadError()) {
			return false;
		} else {
			save();
			return true;
		}
	}

	// Default constructor
	Settings() :
		m_targetTemperature(26),
		m_modeAutomatic(false),
		m_manualPumpOn(false),
		m_isServerInformed(false),
		m_unsavedSettings(false),
		m_autoUpload(true) {
	}


	// Initialise the settings data
	// Copy the settings data from the EEPROM (non-volatile memory)
	void init() {

		// Get amount of data to copy
		uint8_t* dataBegin = (uint8_t*)this;          // Location of this class in memory
		uint8_t* dataEnd = (uint8_t*)(&m_pumpStatus); // Location of first non-saved value within this class
		const int amount = dataEnd - dataBegin;

		// Get data from EEPROM
		for (int i=0; i<amount; i++) {
			EEPROM.get(EepromAddress + i, dataBegin[i]);
		}
	}

	// Record the settings data
	// Copy the settings data to the EEPROM (non-volatile memory)
	// Note: Avoid calling this if unneeded, as it will wear the EEPROM
	void save() {
		if (m_unsavedSettings) {
			m_unsavedSettings = false;

			// Get amount of data to copy
			uint8_t* dataBegin = (uint8_t*)this;          // Location of this class in memory
			uint8_t* dataEnd = (uint8_t*)(&m_pumpStatus); // Location of first non-saved value within this class
			const int amount = dataEnd - dataBegin;

			// Get data from EEPROM
			for (int i=0; i<amount; i++) {
				EEPROM.put(EepromAddress + i, dataBegin[i]);
			}
		}
	}

private:

	// Settings which need to be saved to EEPROM
	// The order of these settings should be kept the same, if possible
	char  m_systemGuid[guidMaxLength]; // Leave in first slot
	bool  m_modeAutomatic;
	bool  m_manualPumpOn;
	float m_targetTemperature;
	bool  m_autoUpload;

	// Settings which do NOT need to be saved to EEPROM
	bool m_pumpStatus; // This value MUST be first in the not-saved values. If changed then update init() and save()
	bool m_isServerInformed;
	bool m_unsavedSettings;
};

#endif
