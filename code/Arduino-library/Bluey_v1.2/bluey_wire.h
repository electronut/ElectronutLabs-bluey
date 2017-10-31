#include "Arduino.h"
#include "Wire.h"


// Reads num bytes starting from address register on device in to _buff array
void readRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t num_bytes, uint8_t data_buff[]);

// Writes num bytes starting from address register on device
void writeRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *val);
