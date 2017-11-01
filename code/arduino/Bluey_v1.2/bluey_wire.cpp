#include "bluey_wire.h"

// Reads num bytes starting from address register on device in to _buff array
void readRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t num_bytes, uint8_t data_buff[])
{
  Wire.beginTransmission(dev_addr); // start transmission to device
  Wire.write(reg_addr); // sends address to read from
  Wire.endTransmission(false); // end transmission

  Wire.requestFrom(dev_addr, num_bytes); // request 6 bytes from device Registers: DATAX0, DATAX1, DATAY0, DATAY1, DATAZ0, DATAZ1

  int i = 0;
  while(Wire.available()) // device may send less than requested (abnormal)
  {
    data_buff[i++] = Wire.read(); // receive a byte
  }

  Wire.endTransmission(); // end transmission
}

// Writes num bytes starting from address register on device
void writeRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *val)
{
  uint8_t byte1, byte2;

  byte1 = *val >> 8;      // higher byte
  byte2 = *val & 0xff;    // lower byte

  Wire.beginTransmission(dev_addr); // start transmission to device
  Wire.write(reg_addr); // send register address
  Wire.write(byte1); // send value to write
  Wire.write(byte2);
  Wire.endTransmission(); // end transmission
}
