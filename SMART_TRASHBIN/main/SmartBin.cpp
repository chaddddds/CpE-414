#include "SmartBin.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

SmartBin::SmartBin(
    int gasLED, int gasAnalog,
    int irPin, int irButton,
    int buzzerPin,
    int servoPin, int trigPin, int echoPin
)
    : gasSensor(gasLED, gasAnalog),
      irCounter(irPin, irButton),
      proxLid(servoPin, trigPin, echoPin),
      buzzerPin(buzzerPin) {}

void SmartBin::setup() {
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(" Smart Bin Ready ");
    delay(1500);
    lcd.clear();

    pinMode(buzzerPin, OUTPUT);

    gasSensor.setup();
    irCounter.setup();
    proxLid.setup();
}

void SmartBin::loop() {
    unsigned long now = millis();

    switch (currentMode) {

        case AUTO: {

            gasSensor.update();
            irCounter.update();
            proxLid.update(irCounter.binFull);

            gasSensorValue = gasSensor.value;
            distanceValue = proxLid.lastDistance;
            lidState = proxLid.getState();

            if (gasSensor.alert) {
                proxLid.myServo.write(180);
                lcd.setCursor(0, 0);
                lcd.print("GAS ALERT!     ");

                digitalWrite(buzzerPin, HIGH);
            }

            else if (irCounter.alert) {
                if (now - irCounter.lastBeepTime < 500) {
                    digitalWrite(buzzerPin, HIGH);
                } else {
                    digitalWrite(buzzerPin, LOW);
                    irCounter.alert = false;
                }
            }

            else {
                digitalWrite(buzzerPin, LOW);
            }

            if (now - lastLogTime >= 600000) {
                logs[logIndex].timestamp = now;
                logs[logIndex].gasValue = gasSensor.value;
                logs[logIndex].count = irCounter.count;
                logs[logIndex].distance = proxLid.lastDistance;
                logs[logIndex].binFull = irCounter.binFull;

                logIndex = (logIndex + 1) % 100;
                lastLogTime = now;
                Serial.println("Log recorded.");
            }

            break;
        }

        case MAINTENANCE: {

            gasSensor.update();
            irCounter.update();
            proxLid.update(irCounter.binFull);

            digitalWrite(buzzerPin, LOW);

            break;
        }

        case SLEEP: {
            gasSensor.update();     
            irCounter.binFull = false;
            irCounter.alert = false;

            proxLid.myServo.write(0);

            lcd.clear();
            lcd.noBacklight();

            digitalWrite(buzzerPin, LOW);

            break;
        }
    }
}
