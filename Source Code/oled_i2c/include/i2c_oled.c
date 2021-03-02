#include "i2c_oled.h"
#include "tm4c123gh6pm.h"


uint8_t i2c_write_data_block(uint8_t i2c_addr, uint8_t reg, uint8_t * buff, uint8_t len)
{
    int res, i ;
    uint8_t _buff[len+1];

    bcm2835_i2c_setSlaveAddress(i2c_addr);
    _buff[0] = reg;
    for(i=1;i <= len ;i++) _buff[i] = buff[i-1];

    res = bcm2835_i2c_write(_buff,len+1);

    if(res != BCM2835_I2C_REASON_OK) return res;
    return BCM2835_I2C_REASON_OK;
}

uint32_t bcm2835_i2c_write(const char * buf, uint32_t len)
{
  int i = 0;
  I2C1_MSA_R &= ~(0x01); // transmit byte / Write
  I2C1_MDR_R = buf[i];
  I2C1_MCS_R = (1<<0) | (1 << 1); // start run bit
  i++;
  
  while(i < len){
    while(I2C1_MCS_R & (1<<0) !=0); // check if i2C is busy
    
    if(I2C1_MCS_R & (1<<1) != 0){
      
      if(I2C1_MCS_R & (1<<4) != 0){      
        return (I2C1_MCS_R & (1<<1)); // return error
      }else{
        I2C1_MCS_R = (1<<2);// send stop bit
        return (I2C1_MCS_R & (1<<1)); // return error
      }  
    }else{
        I2C1_MDR_R = buf[i];
        I2C1_MCS_R = (1<<0);// keep running
        i++;
      }
    } 
  
  I2C1_MCS_R = (1<<0) | (1 << 2); // send stop bit
  while(I2C1_MCS_R & (1<<0) !=0); // check if i2C is busy
  
  if(I2C1_MCS_R & (1<<1) != 0){
      
      if(I2C1_MCS_R & (1<<4) != 0){      
        return (I2C1_MCS_R & (1<<1)); // return error
      }else{
        I2C1_MCS_R = (1<<2);// send stop bit
        return (I2C1_MCS_R & (1<<1)); // return error
      }
  }else
    return I2C1_MCS_R & (1<<5);
}

void bcm2835_i2c_setSlaveAddress(uint8_t SLA)
{
  I2C1_MSA_R = (SLA<<1);
}

void init_i2c()
{
  SYSCTL_RCGCI2C_R = 0x02; // using I2C 1;
  SYSCTL_RCGCGPIO_R = 0x01; // using port A
  GPIO_PORTA_AFSEL_R = (1<<6)|(1<<7); // AFSEL for these two pins
  GPIO_PORTA_DEN_R = (1<<6)|(1<<7); // digital enable
  GPIO_PORTA_ODR_R = (1<<7); // I2C SDA open drain
  GPIO_PORTA_PCTL_R  = (3<<28)|(3<<24); // put 3 in PMC7 and PMC6 so that it works as I2C interface // PA6 SCL PA7 SDA
  I2C1_MCR_R = (1<<4);//Initialize the I2C Master by writing the I2CMCR register with a value of 0x0000.0010.
  I2C1_MTPR_R = 0x00000007; // since we are using 16MHz
  
}
