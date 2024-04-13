#include "HomeSpan.h" 
#include "DEV_LED.h"          

#define LED_PIN 10 //GPIO10

void setup() {
  
  Serial.begin(115200);

  homeSpan.begin(Category::Lighting,"nightLED");

  new SpanAccessory(); 
  
    new Service::AccessoryInformation();    
      new Characteristic::Identify();                       
  
    new DEV_DimmableLED(LED_PIN);

} // end of setup()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // end of loop()
