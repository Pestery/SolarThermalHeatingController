#ifndef STRATEGY_H
#define STRATEGY_H

// Include headers
#include "settings.h"
#include "timer.h"
#include "sensor_record.h"
#include "time_keeper.h"

// Show pump status using buzzer
#define USE_BUZZER_PUMP_STATUS 0

// The Strategy class is the brains for the controller
class Strategy {
public:

  //pin for pump relay / LED
  static constexpr int pumpPin = 13;

  static constexpr int solarValue = 950; //set value w/m^2 for irradiance acceptance

  #if USE_BUZZER_PUMP_STATUS
  static constexpr int buzzerPin = 8; // TODO: Remove me
  #endif

  //add pinMode(Strategy::pumpPin, OUTPUT); to main_arduino.h in future

	void setPump(bool x) { //fix this
		m_settings.pumpStatus(x);
		if(x) {
			digitalWrite(pumpPin, 1);

			#if USE_BUZZER_PUMP_STATUS
			tone(buzzerPin, 500);
			delay(200);
			tone(buzzerPin, 800);
			delay(200);
			noTone(buzzerPin);
			#endif

		} else {

			digitalWrite(pumpPin, 0);

			#if USE_BUZZER_PUMP_STATUS
			tone(buzzerPin, 800);
			delay(200);
			tone(buzzerPin, 500);
			delay(200);
			noTone(buzzerPin);
			#endif
		}
	}

  // Setup stuff used by Strategy class
  void begin() {
  	pinMode(pumpPin, OUTPUT);
  	#if USE_BUZZER_PUMP_STATUS
  	pinMode(buzzerPin, OUTPUT);
  	#endif
  }

  // Update the controller strategy
  void update(TimeKeeper& timeKeeper) {

    // Get current time in milliseconds
    // This value will overflow (go back to zero) roughly every 50 days
    // It will also reset (go back to zero) if the Arduino is powered off
    unsigned long currentTime = millis();

    // Check if time to run the update (5mins)
    if (m_updateFrequency.triggered(currentTime)) {

      SensorRecord sr;

      sr.readAll(timeKeeper); //getting all sensor data

      /*
      Serial.print("modeAutomatic: ");
      Serial.println(m_settings.modeAutomatic());

      Serial.print("targetTemperature: ");
      Serial.println(m_settings.targetTemperature());

      Serial.print("temperatureInlet: ");
      Serial.println(sr.temperatureInlet);

      Serial.print("temperatureRoof: ");
      Serial.println(sr.temperatureRoof);

      Serial.print("solarIrradiance: ");
      Serial.println(sr.solarIrradiance);

      Serial.print("solarValue: ");
      Serial.println(solarValue);
      //*/

      // roof temp to inlet temp, > 7oC diff turn on, < 2oC diff turn off

      // m_settings.pumpStatus() //reads current pump status
      if(m_settings.modeAutomatic()) {// if mode is in AUTO

          if((m_settings.targetTemperature() > sr.temperatureInlet) && (sr.temperatureRoof > sr.temperatureInlet) && (sr.solarIrradiance > solarValue)){
              setPump(1); //pump is turned on as conditions for control strategy are met
          }
          else setPump(0); //requirements not met to run pump, pump is turned off
      }
      else setPump(m_settings.manualPumpOn()); // if Pump is not in Auto must be in manual thus pump is turned off


    //temperatureInput
    //temperatureOutput
    //temperatureRoof
    //solarIrradiance

    }
  }

  // Default constructor
  Strategy(Settings& settings) :
    m_updateFrequency(20 * 1000), // Milliseconds Update every 300s (5min)
    m_settings(settings) {
  }

private:

  // Private data
  Timer m_updateFrequency;
  Settings& m_settings;
};

#endif
