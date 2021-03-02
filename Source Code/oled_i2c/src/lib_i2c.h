#ifndef _LIB_I2C_H
#define _LIB_I2C_H

#ifdef __cplusplus
extern "C" {
#endif // def __cplusplus
#include<stdint.h>

void i2c_start(void);
void i2c_stop(void);

uint8_t i2c_read_data_block(uint8_t i2c_addr, uint8_t reg, uint8_t * buff, uint8_t len);
uint8_t i2c_write_data_block(uint8_t i2c_addr, uint8_t reg, uint8_t * buff, uint8_t len);
int i2c_readReg(uint8_t i2c_addr, uint8_t reg);
int i2c_writeReg(uint8_t i2c_addr, uint8_t reg, uint8_t value);

#endif 
#ifdef __cplusplus
} // extern "C"
#endif // def __cplusplus