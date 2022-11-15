
#ifndef INA226_H
#define INA226_H

#include <Arduino.h>
#include <SlowSoftI2CMaster.h>
#include <inttypes.h>

extern SlowSoftI2CMaster arciic;

short INA226_read(unsigned char reg);
short INA226_Write(uint8_t reg_addr,uint16_t reg_data);

#endif