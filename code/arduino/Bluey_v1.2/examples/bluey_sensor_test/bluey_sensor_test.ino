#include <Arduino.h>
#include <Wire.h>

#include "HDC1010.h"
#include "APDS9301.h"
#include "LSM6DS3.h"

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  HDC1010_init(TEMP_OR_HUMID);  
  APDS9301_init();
  LSM6DS3_init();
}

void loop()
{
  volatile float lux = getlux();
  volatile float temp = HDC1010_get_temp();
  volatile float humid = HDC1010_get_humid();
  static uint8_t who_am_i = LSM6DS3_who_am_i();
  
  Serial.println(lux);  
  Serial.println();
  Serial.println(temp);  
  Serial.println(humid);  
  Serial.println();
  Serial.println(who_am_i);
  delay(1000);
}
