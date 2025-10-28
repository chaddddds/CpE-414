#include <Arduino.h>
#include "IRCounter.h"


IRCounter::IRCounter(int irPin, int buzzerPin, int buttonPin) 
  : IRPin(irPin), buzzerPin(buzzerPin), buttonPin(buttonPin), lcd(0x27, 16, 2) {
  n = 0;
  oldValue = 1;
}

void IRCounter::init() {
  lcd.init();
  lcd.backlight();
  pinMode(IRPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
}

void IRCounter::update() {
  lcd.setCursor(0, 0);
  lcd.print("Count : ");
  lcd.setCursor(7, 0);
  lcd.print(n);

  if (digitalRead(IRPin) == 0 && oldValue == 1) {
    oldValue = 0;
    Count();
  } else if (digitalRead(IRPin) == 1 && oldValue == 0) {
    oldValue = 1;
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }

  if (digitalRead(buttonPin) == 0) {
    Reset();
  }

  delay(100);
}

void IRCounter::Count() {
  n = n + 1;

  lcd.setCursor(7, 0);
  lcd.print("   ");
  lcd.setCursor(7, 0);
  lcd.print(n);
  lcd.setCursor(0, 1);
  lcd.print("Object Detected");

  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(buzzerPin, LOW);

  Serial.print("Object detected. Count: ");
  Serial.println(n);
}

void IRCounter::Reset() {
  n = 0;
  lcd.setCursor(0, 0);
  lcd.print("Count : ");
  lcd.setCursor(7, 0);
  lcd.print("   ");
  lcd.setCursor(7, 0);
  lcd.print(n);
  lcd.setCursor(0, 1);
  lcd.print("                "); 

  Serial.println("Counter reset.");
}
