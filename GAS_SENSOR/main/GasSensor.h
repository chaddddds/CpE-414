#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#include <LiquidCrystal_I2C.h>

class GasSensor {
  public:
    GasSensor(int ledPin, int buzzerPin, int sensorPin);
    
    void setup();
    void loop();

  private:
    int LED;
    int Buzzer;
    int Sensor;
    LiquidCrystal_I2C lcd;

    void printGasValue(int value);
};

#endif
