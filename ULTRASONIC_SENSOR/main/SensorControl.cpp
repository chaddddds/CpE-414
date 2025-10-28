#include "SensorControl.h"

SensorControl::SensorControl(int servoPin, int trigPin, int echoPin) 
  : lcd(0x27, 16, 2), servoPin(servoPin), trigPin(trigPin), echoPin(echoPin), distance(0) {}

void SensorControl::setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("System Ready.");
  delay(3000);
  lcd.clear();
  
  myServo.attach(servoPin);  
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);   
}

void SensorControl::loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2;
  
  lcd.setCursor(0, 0);
  lcd.print("Distance:     ");
  lcd.setCursor(10, 0);
  lcd.print(distance);
  lcd.print("cm ");
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance <= 25) {
    myServo.write(90);
    lcd.setCursor(0, 1);
    lcd.print("Status: OPEN   ");
    Serial.println("Servo: OPEN");
    delay(5000);
  } else {
    myServo.write(0);
    lcd.setCursor(0, 1);
    lcd.print("Status: CLOSED ");
    Serial.println("Servo: CLOSED");
  }

  delay(500);
}
