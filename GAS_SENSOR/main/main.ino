#include "GasSensor.h"  

#define LED 23        
#define Buzzer 14     
#define Sensor 34     

GasSensor gasSensor(LED, Buzzer, Sensor);  

void setup() {
  Serial.begin(115200);
  gasSensor.setup();  
}

void loop() {
  gasSensor.loop();  
}
