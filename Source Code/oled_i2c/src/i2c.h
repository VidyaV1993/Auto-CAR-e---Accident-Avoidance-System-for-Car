#define BCM2835_I2C_REASON_OK 0x20

uint32_t bcm2835_i2c_write(const char * buf, uint32_t len);
uint8_t i2c_write_data_block(uint8_t i2c_addr, uint8_t reg, uint8_t * buff, uint8_t len);
void bcm2835_i2c_setSlaveAddress(uint8_t SLA);