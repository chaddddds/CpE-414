#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

class SensorControl {
  public:
    SensorControl(int servoPin, int trigPin, int echoPin);  
    void setup();  
    void loop();   
  
  private:
    LiquidCrystal_I2C lcd;  
    Servo myServo;          
    int distance;
    int servoPin;           
    int trigPin;           
    int echoPin;            
};

#endif
