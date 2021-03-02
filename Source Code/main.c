#include "i2c_oled.h"
#include "I2C_SSD1306Z.h"
#include "init.h"
#include <stdio.h>

#define   EyeBlink       0x02    //PD1
#define   Blue           0x04
#define   Green          0x08
#define   Red            0x02

//void ESP8266InitUART(void);
extern volatile float alchole;

int main()
{
 unsigned int BlinkMask; 
  
 PWM0_Init();
 GPIO_PORTB_DATA_R |= 1<<3;     //  these two pins control clockwise or counter-clockwise
 GPIO_PORTB_DATA_R &= ~(1<<2);

  init_i2c();
  Init_LCD();
  clear_LCD();
  timer_ADC();
  
  print_Line(0,"Testing"); 
  IR_sensor();
  adc_MQ3();

  while(1){    
  
    BlinkMask = GPIO_PORTD_DATA_R & EyeBlink;
    
    if (BlinkMask == EyeBlink) {       
      GPIO_PORTF_DATA_R = Green;
    }
    else {
      GPIO_PORTF_DATA_R = Red;         
    }   
    
    if((BlinkMask == 0x0) || (alchole >= 0.3))
    {
       GPIO_PORTB_DATA_R &= 0<<3;   
       GPIO_PORTB_DATA_R &= 0<<2;    
    }else if((BlinkMask == 0x02) && (alchole < 0.3)){
        GPIO_PORTB_DATA_R |= 1<<3;
        GPIO_PORTB_DATA_R &= ~(1<<2);
    
    }
    

 }
 
  return 0;
}

/*void ESP8266InitUART(void)
{
  volatile int delay;
  SYSCTL_RCGCUART_R |= 0x02; // Enable UART1
  while((SYSCTL_PRUART_R&0x02)==0){};
  SYSCTL_RCGCGPIO_R |= 0x02; // Enable PORT B clock gating
  while((SYSCTL_PRGPIO_R&0x02)==0){}; 
  GPIO_PORTB_AFSEL_R |= 0x03;
  GPIO_PORTB_DIR_R |= 0x20; // PB5 output to reset
  GPIO_PORTB_PCTL_R =(GPIO_PORTB_PCTL_R&0xFF0FFF00)|0x00000011;
  GPIO_PORTB_DEN_R   |= 0x23; //23 
  GPIO_PORTB_DATA_R |= 0x20; // reset high
  UART1_CTL_R &= ~UART_CTL_UARTEN;                  // Clear UART1 enable bit during config
  UART1_IBRD_R = 43;                    // IBRD = int(80,000,000 / (16 * 115,200)) = int(43.403)
  UART1_FBRD_R = 26;                    // FBRD = round(0.4028 * 64 ) = 26
  UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);  // 8 bit word length, 1 stop, no parity, FIFOs enabled
  UART1_IFLS_R &= ~0x3F;                            // Clear TX and RX interrupt FIFO level fields
  UART1_IFLS_R += UART_IFLS_RX1_8 ;                 // RX FIFO interrupt threshold >= 1/8th full
  UART1_IM_R  |= UART_IM_RXIM | UART_IM_RTIM;       // Enable interupt on RX and RX transmission end
  UART1_CTL_R |= UART_CTL_UARTEN;                   // Set UART1 enable bit    
}*/