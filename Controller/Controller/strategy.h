#ifndef STRATEGY_H
#define STRATEGY_H

// Include headers
#include "settings.h"
#include "timer.h"
#include "sensor_record.h"

// The Strategy class is the brains for the controller
class Strategy {
public:
  
  //pin for pump relay / LED
  static constexpr int pumpPin = D13;  

  static constexpr int solarValue = 950; //set value w/m^2 for irradiance acceptance

  //add pinMode(Strategy::pumpPin, OUTPUT); to main_arduino.h in future

  void setPump(bool x) { //fix this
      m_settings.pumpStatus(x);
      if(x)
      {
         
          digitalWrite(pumpPin, 1);
                            
      }
      else digitalWrite(pumpPin, 0);

  }

  // Update the controller strategy
  void update() {

    // Get current time in milliseconds
    // This value will overflow (go back to zero) roughly every 50 days
    // It will also reset (go back to zero) if the Arduino is powered off
    unsigned long currentTime = millis();

    // Check if time to run the update (5mins)
    if (m_updateFrequency.triggered(currentTime)) {

      SensorRecord sr;

      sr.readAll(); //getting all sensor data

      // m_settings.pumpStatus() //reads current pump status      

      If(m_settings.modeAutomatic()) {// if mode is in AUTO
      
          if((m_settings.setTemp() > sr.temperatureInlet) && (sr.temperatureRoof > sr.temperatureInlet) && (sr.solarIrradiance > solarValue)){
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
    m_updateFrequency(300 * 1000), // Milliseconds Update every 300s (5min)
    m_settings(settings) {
  }

private:

  // Private data
  Timer m_updateFrequency;
  Settings& m_settings;
};

#endif