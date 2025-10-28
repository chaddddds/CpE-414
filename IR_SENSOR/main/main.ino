#include "IRCounter.h"

int IRPin = 14;          
int buzzerPin = 27;       
int buttonPin = 26;       

IRCounter irCounter(IRPin, buzzerPin, buttonPin);

void setup() {
  Serial.begin(115200);
  irCounter.init();
}

void loop() {
  irCounter.update();
}
