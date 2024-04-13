/*
 * housework count for husband and wife
 * 
*/
#include <LilyGo_AMOLED.h>      //To use LilyGo AMOLED series screens, please include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include "EEPROM.h"
#include <lvgl.h>
#include <AceButton.h>
#include "gui.h"

#define EEPROM_SIZE 4 // 2bytes per person
#define TOUCH_THRESHOLD 5000 /* Lower the value, more the sensitivity */
#define BATTERY_ADC 4
#define BATT_HIGH 3.700
#define BATT_LOW 2.750


LilyGo_Class amoled;

const uint8_t touch_pin = 10;

int wife_addr = 0;
int husband_addr = 2;
unsigned long lastTime = 0;
uint8_t time_count = TIMEOUT;


void battery2percent(float battery) {
  battery_percent = (battery - BATT_LOW) / (BATT_HIGH - BATT_LOW) * 100;
  if (battery_percent >= 100) battery_percent = 100.0;
  if (battery_percent <= 0) battery_percent = 0.0;
}

void batteryMeasure(void) {
  int i = 0;
  for (i=0;i<16;i++) {
    battery += analogReadMilliVolts(BATTERY_ADC);
  }
  battery = battery/8/1000;
  battery2percent(battery);
}

void touchSensorTask(void* ptr)
{
  while (1) {
    if (touchRead(touch_pin) > 4*TOUCH_THRESHOLD) {
      nextPage();
      vTaskDelay(1000);
      time_count = TIMEOUT;
    }
    vTaskDelay(5);
  }

  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  // fetch stored data run once
  EEPROM.begin(EEPROM_SIZE);
  delay(100);
  wife = EEPROM.readUShort(wife_addr);
  husband = EEPROM.readUShort(husband_addr);

  // buttons
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);

  // battery read run once
  pinMode(BATTERY_ADC, INPUT);
  batteryMeasure();

  // setup display
  bool rslt = false;
  rslt = amoled.begin();
  beginLvglHelper(amoled);
  
  // display main gui
  mainGUI();

  // default values
  time_count = TIMEOUT;

  // Enable Watchdog
  enableLoopWDT();

  // create touch task
  xTaskCreate(touchSensorTask, "touch", 10 * 1024, NULL, 12, NULL);

  // sleep mode touch
  touchSleepWakeUpEnable(touch_pin, TOUCH_THRESHOLD);
}

void loop(){

  lv_task_handler();
  delay(1);
  
  unsigned long nowTime = millis();
  if (nowTime - lastTime > 1000){
    lastTime = nowTime;
    if (time_count > 0) time_count -= 1; 
  }

  if (time_count == 0){
    EEPROM.writeUShort(husband_addr, husband);
    EEPROM.writeUShort(wife_addr, wife);
    EEPROM.commit();
    esp_deep_sleep_start();
  }
}