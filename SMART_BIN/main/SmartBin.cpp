#include "SmartBin.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// GAS SENSOR
GasSensor::GasSensor(int ledPin, int buzzerPin, int sensorPin)
: LED(ledPin), Buzzer(buzzerPin), Sensor(sensorPin) {}

void GasSensor::setup() {
  pinMode(LED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
}

void GasSensor::update() {
  int value = analogRead(Sensor);

  printScreen(value);

  if (value > 3500) {
    digitalWrite(LED, HIGH);
    digitalWrite(Buzzer, HIGH);
    Serial.println("GAS ALERT!");
  } else {
    digitalWrite(LED, LOW);
    digitalWrite(Buzzer, LOW);
  }
}

void GasSensor::printScreen(int value) {
  lcd.setCursor(0, 0);
  lcd.print("Gas:");
  lcd.print(value);
  lcd.print("    ");
}
// IR COUNTER

// PROXIMITY 

SmartBin::SmartBin(
  int gasLED, int gasBuzz, int gasAnalog,
  int irPin, int irBuzz, int irButton,
  int servoPin, int trigPin, int echoPin
)
: gasSensor(gasLED, gasBuzz, gasAnalog),
  irCounter(irPin, irBuzz, irButton),
  proxLid(servoPin, trigPin, echoPin)
{
}

void SmartBin::setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Smart Bin Ready ");
  delay(1500);
  lcd.clear();

  gasSensor.setup();
  irCounter.setup();
  proxLid.setup();
}

void SmartBin::loop() {
  gasSensor.update();
  irCounter.update();
  proxLid.update();

  delay(150); 
}

