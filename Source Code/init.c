#include "init.h"


void timer_ADC()
{
  SYSCTL_RCGCTIMER_R = 0x01; // enable timer 0
  TIMER0_CTL_R &= ~0x01; //disable counting
  TIMER0_CFG_R = 0x00; // using 32-bit timer
  TIMER0_TAMR_R = 0x02; // configure tmer mode
  TIMER0_TAILR_R = 0xF42400; // counting value every second
}

void adc_MQ3()
{
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x10;         // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
  GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE2 input
  GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog function on PE2
  SYSCTL_RCGC0_R |= 0x00010000;   // 6) activate ADC0
  delay = SYSCTL_RCGC2_R;        
  SYSCTL_RCGC0_R &= ~0x00000300;  // 7) configure for 125K
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xFFFF;
  ADC0_EMUX_R |= 0x5000;          // 10) seq3 is timer trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  ADC0_SSMUX3_R += 1;             //    set channel Ain9 (PE2)
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R = 0x08;                // interrupt mask for SS3
  NVIC_EN0_R = (NVIC_EN0_R & ~0x00020000) + 0x00020000; // enable interrupt 17 for ADC
  TIMER0_CTL_R |= (1 << 5);       // timer trigger interrupt 
  ADC0_ISC_R = 0x08;              // clear interrupt status
  ADC0_ACTSS_R |= 0x0008;         // 13) enable sample sequencer 3
  TIMER0_CTL_R |= 0x01;          // start counting
}


void PWM0_Init(){   //(unsigned short period, unsigned short duty){
 volatile unsigned long delay;
 SYSCTL_RCGC0_R |= SYSCTL_RCGC0_PWM0; // 1)activate PWM
 SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // 2)activate port B
 delay = SYSCTL_RCGC2_R; // allow time to finish activating
 GPIO_PORTB_AFSEL_R |= 0xC0; // enable alt funct on PB6,7
 GPIO_PORTB_DIR_R |= (1<<3)|(1<<2); // enable PB2 and PB3 as GPIO control for IC motor as output
 GPIO_PORTB_DEN_R |= (1<<3)|(1<<2);
 GPIO_PORTB_PCTL_R  = (4<<28)|(4<<24); // put 4 in PMB7 and PMB6 so that it works as PWM interface
 SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
 SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; // clear PWM divider field
 SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2; // configure for /2 divider
 PWM0_CTL_R = 0; // 4) re-loading mode
 PWM0_0_GENA_R = 0x8C;//(PWM_0_GENA_ACTCMPAD_ONE|PWM_0_GENA_ACTLOAD_ZERO);
 PWM0_0_GENB_R = 0x80C; 
 PWM0_0_LOAD_R = 0x13F; // 5) cycles needed to count to 0; for 25 KHZ; 320-1
 PWM0_0_CMPA_R = 0x4F; // 6) count value when output rises
 PWM0_0_CMPB_R = 0xEF; // 6) count value when output rises
 PWM0_CTL_R |= PWM_0_CTL_ENABLE; // 7) start PWM0
 PWM0_ENABLE_R |= 0x03; // enable PWM0 pin 
}

void IR_sensor()
{
  SYSCTL_RCGC2_R |= 0x28;
 // SYSCTL_RCGCGPIO_R |= 0x28;      //enable clock on port F and port D
  GPIO_PORTD_DIR_R = 0x00;       //set Port D as input  (0 for input) 
  GPIO_PORTD_DEN_R = 0x02;       //set digital enable for Port D
  GPIO_PORTF_DIR_R = 0x0D;       //set Port F as output (1 for output)
  GPIO_PORTF_DEN_R = 0x0D;       //set digital enable for Port F
}  
