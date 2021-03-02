#include  <tm4c123gh6pm.h>
#include  <stdint.h>

#define   EyeBlink       0x02    //PD1
#define   Blue           0x04
#define   Green          0x08
#define   Red            0x02


int main()
{
  SYSCTL_RCGCGPIO_R = 0x28;      //enable clock on port F and port D
  GPIO_PORTD_DIR_R = 0x00;       //set Port D as input  (0 for input) 
  GPIO_PORTD_DEN_R = 0x0F;       //set digital enable for Port D
  GPIO_PORTF_DIR_R = 0x0F;       //set Port F as output (1 for output)
  GPIO_PORTF_DEN_R = 0x0F;       //set digital enable for Port F
  
  unsigned int BlinkMask;
  
  while(1) {
  
    BlinkMask = GPIO_PORTD_DATA_R & EyeBlink;
    
    if (BlinkMask == EyeBlink) {       
      GPIO_PORTF_DATA_R = Green;
    }
    else {
      GPIO_PORTF_DATA_R = Red;         
    }   
}
return 0;
}
