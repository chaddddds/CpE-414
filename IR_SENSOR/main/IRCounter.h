#ifndef IRCOUNTER_H
#define IRCOUNTER_H

#include <LiquidCrystal_I2C.h>

class IRCounter {
  public:
    IRCounter(int irPin, int buzzerPin, int buttonPin);
    void init();
    void update();
    
  private:
    int IRPin;
    int buzzerPin;
    int buttonPin;
    int n;
    int oldValue;
    LiquidCrystal_I2C lcd;

    void Count();
    void Reset();
};

#endif
