IRCounter::IRCounter(int irPin, int buttonPin)
    : IRPin(irPin), buttonPin(buttonPin) {
    oldValue = 1;
    lastBeepTime = 0;
}

void IRCounter::setup() {
    pinMode(IRPin, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);
}

void IRCounter::update() {
    int irValue = digitalRead(IRPin);

    if (irValue == 0 && oldValue == 1) {
        oldValue = 0;
        Count();
        detectStart = millis();
    } 
    else if (irValue == 1 && oldValue == 0) {
        oldValue = 1;
        Serial.println("IR: No Object");
        detectStart = 0;
    }

    if (irValue == 0 && (millis() - detectStart >= 5000)) {
        binFull = true;
        Serial.println("BIN FULL DETECTED");
    }

    if (digitalRead(buttonPin) == LOW) {
        resetAll();
    }

    lcd.setCursor(0, 1);
    if (binFull) {
        lcd.print("FULL ");
    } else {
        lcd.print("Trash Count: ");
        lcd.print(count);
        lcd.print(" ");
    }
}

void IRCounter::Count() {
    count++;
    alert = true;
    lastBeepTime = millis();

    Serial.println("IR: Object Detected");
    Serial.print("Trash Count: ");
    Serial.println(count);
}

void IRCounter::resetAll() {
    count = 0;
    binFull = false;
    alert = false;
    detectStart = 0;

    Serial.println("RESET -> Normal Mode");
}
