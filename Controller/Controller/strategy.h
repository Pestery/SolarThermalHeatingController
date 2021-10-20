#ifndef STRATEGY_H
#define STRATEGY_H

// Include headers
#include "settings.h"
#include "timer.h"
#include "sensor_record.h"
#include "time_keeper.h"

// Show pump status using buzzer and/or power LED
#define USE_BUZZER_PUMP_STATUS 0
#define USE_POWER_LED_PUMP_STATUS 1

// The Strategy class is the brains for the controller
class Strategy {
public:

	// The frequency which the controller should update the pump status
	static constexpr uint32_t updateFrequencySeconds = 20; // 600;

	// Pin for pump relay / LED
	static constexpr int pumpPin = 13;

	// Solar irradiance threshold (W/m^2)
	// If the solar irradiance is less than this value then the pump will shut off
	static constexpr int solarThreshold = 950;

	// Roof-Inlet temperature difference for pump on
	// If the roof temperature is greater than the inlet temperature plus this amount, then the pump will turn on
	static constexpr float roofTemperatureDifferencePumpOn = 7.0f;

	// Roof-Inlet temperature difference for pump off
	// If the roof temperature is less than the inlet temperature plus this amount, then the pump will turn off
	static constexpr float roofTemperatureDifferencePumpOff = 2.0f;

	#if USE_BUZZER_PUMP_STATUS
	static constexpr int buzzerPin = 8; // TODO: Remove me
	#endif

	// Set the pump to on (true) or off (false)
	void setPump(bool on) {

		// Check if the pump is already set to the requested value
		if (m_settings.pumpStatus() != on) {
			m_settings.pumpStatus(on);
			if (on) {

				// Turn pump on
				digitalWrite(pumpPin, HIGH);

				#if USE_POWER_LED_PUMP_STATUS
				digitalWrite(LED_BUILTIN, HIGH);
				#endif

				#if USE_BUZZER_PUMP_STATUS
				for (int i=400; i<=800; i+=200) {
					tone(buzzerPin, i);
					delay(100);
				}
				noTone(buzzerPin);
				#endif

			} else {

				// Turn pump off
				digitalWrite(pumpPin, LOW);

				#if USE_POWER_LED_PUMP_STATUS
				digitalWrite(LED_BUILTIN, LOW);
				#endif

				#if USE_BUZZER_PUMP_STATUS
				for (int i=800; i>=400; i-=200) {
					tone(buzzerPin, i);
					delay(100);
				}
				noTone(buzzerPin);
				#endif
			}
		}
	}

	// Setup stuff used by Strategy class
	void begin() {

		// Set pin outputs
		pinMode(pumpPin, OUTPUT);

		#if USE_POWER_LED_PUMP_STATUS
		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, LOW);
		#endif

		#if USE_BUZZER_PUMP_STATUS
		pinMode(buzzerPin, OUTPUT);
		#endif

		// Reset the timer so that the controller will update once immediately
		m_updateFrequency.reset(millis());
	}

	// Update the controller strategy
	void update(TimeKeeper& timeKeeper) {

		// Get current time in milliseconds
		// This value will overflow (go back to zero) roughly every 50 days
		// It will also reset (go back to zero) if the Arduino is powered off
		unsigned long currentTime = millis();

		// Check if time to run the update
		if (m_updateFrequency.triggered(currentTime)) {

			// Get current sensor values
			SensorRecord sr;
			sr.readAll(timeKeeper);

			// Check if the system is in automatic mode
			if (m_settings.modeAutomatic()) {

				// System is in automatic mode
				// Check if the pump is currently running
				if (m_settings.pumpStatus()) {

					// Pump is currently on
					// Check if the pump should stop running
					if ((sr.temperatureInlet > m_settings.targetTemperature()) ||
						(sr.temperatureRoof < (sr.temperatureInlet + roofTemperatureDifferencePumpOff)) ||
						(sr.solarIrradiance < solarThreshold)) {
							setPump(false);
					}

				} else {

					// Pump is currently off
					// Check if the pump should start running
					if ((sr.temperatureInlet < m_settings.targetTemperature()) &&
						(sr.temperatureRoof > (sr.temperatureInlet + roofTemperatureDifferencePumpOn)) &&
						(sr.solarIrradiance > solarThreshold)) {
							setPump(true);
					}
				}

			} else {

				// System is in manual mode
				// Change the pump state to match the requested state
				setPump(m_settings.manualPumpOn());
			}
		}
	}

	// Default constructor
	Strategy(Settings& settings) :
		m_updateFrequency(updateFrequencySeconds * (uint32_t)1000), // Milliseconds
		m_settings(settings) {
	}

private:

	// Private data
	Timer m_updateFrequency;
	Settings& m_settings;
};

#endif
