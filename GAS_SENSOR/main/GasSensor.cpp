#include <Arduino.h>
#include "GasSensor.h"

GasSensor::GasSensor(int ledPin, int buzzerPin, int sensorPin)
  : LED(ledPin), Buzzer(buzzerPin), Sensor(sensorPin), lcd(0x27, 16, 2) {
}

void GasSensor::setup() {
  lcd.init();
  lcd.backlight();
  pinMode(LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
}

void GasSensor::loop() {
  int value = analogRead(Sensor);
  printGasValue(value);

  if (value > 3500) { 
    digitalWrite(LED, HIGH);
    digitalWrite(Buzzer, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("GAS Detected!   ");
    Serial.println("GAS Detected!");
  } else {
    digitalWrite(LED, LOW);
    digitalWrite(Buzzer, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Safe            ");
    Serial.println("Safe");
  }

  delay(200);
}

void GasSensor::printGasValue(int value) {
  lcd.setCursor(0, 0);
  lcd.print("GAS Value: ");
  lcd.print(value);
  lcd.print("   ");

  Serial.print("Gas Value: ");
  Serial.println(value);
}

void GasSensor::update() {
    value = analogRead(Sensor);
    Serial.print("Gas Reading: ");
    Serial.println(value);

    if (value > 3500) {
        digitalWrite(LED, HIGH);
        alert = true;
    } else {
        digitalWrite(LED, LOW);
        alert = false;
    }
}

