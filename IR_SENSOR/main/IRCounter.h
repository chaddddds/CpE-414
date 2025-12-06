class IRCounter {
public:
    IRCounter(int irPin, int buttonPin);

    void setup();
    void update();

    bool alert = false;
    bool binFull = false;

    unsigned long lastBeepTime;
    unsigned long detectStart = 0;

    int count = 0;

    void resetAll();

    int IRPin, buttonPin;

private:
    int oldValue;
    void Count();
};
