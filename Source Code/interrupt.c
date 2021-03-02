#include "tm4c123gh6pm.h"
#include "i2c_oled.h"
#include "I2C_SSD1306Z.h"
#include <stdio.h>

 volatile float alchole;

void ADC0_Handler(void)
{
  
  char buffer[20] = {0};

  while((ADC0_RIS_R & 0x8) == 0);
  alchole = ADC0_SSFIFO3_R;
  alchole = ((alchole * 3.3/4095.0)*2)*0.21;
  ADC0_ISC_R = 0x8;
  TIMER0_ICR_R = 0x01;
  sprintf(buffer, "BAC : %2.2f %%", alchole);
  print_Line(0,buffer); 
}