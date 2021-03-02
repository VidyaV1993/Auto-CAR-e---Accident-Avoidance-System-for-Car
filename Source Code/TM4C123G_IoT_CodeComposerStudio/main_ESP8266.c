/*
 * main.h
 *
 *  Created on: Aug 14, 2015
 *      Author: secakmak
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"

#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "DelayTimer.h"

static char COMMAND[128];
static char ReceivedData[512];
static char str[128];

bool process = true;
bool ProcessRoutine();

char *Substring(char *src, char *dst, int start, int stop);
int SearchIndexOf(char src[], char str[]);
char* itoa(int i, char b[]);
void ftoa(float f,char *buf);
void ESP8266();

#define Period  320000 //(16000000/50) 50Hz
#define SERVO_STEPS         180     // Maximum amount of steps in degrees (180 is common)
#define SERVO_MIN           9500     // The minimum duty cycle for this servo
#define SERVO_MAX           35100    // The maximum duty cycle

unsigned int servo_lut[SERVO_STEPS+1];

void InitUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200, 16000000);
}

void InitESPUART(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
	UARTConfigSetExpClk(UART1_BASE, 16000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTEnable(UART1_BASE);

}

void SendATCommand(char *cmd)
{
	while(UARTBusy(UART1_BASE));
	while(*cmd != '\0')
	{
		UARTCharPut(UART1_BASE, *cmd++);
	}
	UARTCharPut(UART1_BASE, '\r'); //CR
	UARTCharPut(UART1_BASE, '\n'); //LF

}

int recvString(char *target, char *data, int timeout, bool check)
{
	int i=0;
	char a;
	unsigned long start = millis();

    while (millis() - start < timeout)
    {
    	while(UARTCharsAvail(UART1_BASE))
    	{
              a = UARTCharGet(UART1_BASE);
              if(a == '\0') continue;
              data[i]= a;
              i++;
    	}

    	if(check)
    	{
    		if (SearchIndexOf(data, target) != -1)
    		{
    			break;
    		}
    	}
    }

    return 0;
}

bool recvFind(char *target, int timeout,bool check)
{
	recvString(target, ReceivedData, timeout, check);

	if (SearchIndexOf(ReceivedData, target) != -1)
	{
		return true;
	}
	return false;
}

bool recvFindAndFilter(char *target, char *begin, char* end, char *data, int timeout)
{
	recvString(target, ReceivedData, timeout, true);

    if (SearchIndexOf(ReceivedData, target) != -1)
    {
         int index1 = SearchIndexOf(ReceivedData, begin);
         int index2 = SearchIndexOf(ReceivedData, end);

         if (index1 != -1 && index2 != -1)
         {
             index1 += strlen(begin);
             Substring(ReceivedData,data,index1, index2);
             return true;
         }
     }
     data = "";
     return false;
}

bool ATesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT");
	return recvFind("OK",5000, true);
}

bool RSTesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+RST");
	return recvFind("OK",5000, false);
}

bool CWMODEesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CWMODE=1");
	return recvFind("OK",5000, true);
}

bool CWJAPesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CWJAP=\"NetworkName\",\"Password\""); //Your Wifi: NetworkName, Password
	return recvFind("OK",10000, true);
}

bool CWQAPesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CWQAP");
	return recvFind("OK",10000, true);
}

bool CIPMUXesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CIPMUX=0");
	return recvFind("OK",5000, true);
}

bool ATGMResp(char *version)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+GMR");
	return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", version,10000);
}

bool aCWMODEesp(char *list)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CWMODE=?");
	return recvFindAndFilter("OK", "+CWMODE:(", ")\r\n\r\nOK", list,10000);
}

bool aCWLAPesp(char *list)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CWLAP");
	return recvFindAndFilter("OK","\r\r\n", "\r\n\r\nOK", list,15000);
}

bool aCIFSResp(char *list)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CIFSR");
	return recvFindAndFilter("OK","\r\r\n", "\r\n\r\nOK", list,15000);
}

bool CIPSTOesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CIPSTO=10000");
	return recvFind("OK",2000, true);
}

bool CIPSTARTesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CIPSTART=\"TCP\",\"XXX.XXX.XXX.XXX\",XXXX"); //Server IP and Port: such as 192.255.0.100, 9999
	return recvFind("OK",2000, true);
}

bool CIPCLOSEesp(void)
{
	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand("AT+CIPCLOSE");
	return recvFind("OK",5000, true);
}

bool CIPSENDesp(char *text)
{
	int len = strlen(text)+2;

	itoa(len,str);

	char* AT_CMD_SEND = "AT+CIPSEND=";
	char CMD_TEXT[128];
	strcpy(CMD_TEXT,AT_CMD_SEND);
	strcat(CMD_TEXT,str);

	memset(ReceivedData, 0, sizeof(ReceivedData));
	SendATCommand(CMD_TEXT);
	delay(5);
	SendATCommand(text);
	return recvFind("SEND OK",2000, true);
}

void HardwareReset()
{
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0); //Output
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_4, 0x00); //LOW ->Reset to ESP8266
    delay(50);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0); //Output ->// Open drain; reset -> GND
    delay(10);
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0); //Input ->// Back to high-impedance pin state
    delay(3000);
}

void ProcessCommand(char *CommandText)
{
	long Status;
	char *array[10];
	int i=0;

	array[i] = strtok(CommandText,":");

	while(array[i]!=NULL)
	{
		array[++i] = strtok(NULL,":");
	}

	memset(COMMAND, 0, sizeof(COMMAND));
	strncpy(COMMAND, array[1], (strlen(array[1])-1));

	UARTprintf("CMD->%s\n",COMMAND);

	Status = CmdLineProcess(COMMAND);

	if(Status == CMDLINE_BAD_CMD)
	{
		UARTprintf("Bad command!\n");
	}
}

void QuitProcess(void)
{
	process = false;
}


//-------------SERVO DRIVER--------------------------------------------------------------------------------------------------------
void InitServo(void) //PC5
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC5_WT0CCP1);
	GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_B_PWM);
	TimerLoadSet(WTIMER0_BASE, TIMER_B, (Period-1));
	TimerMatchSet(WTIMER0_BASE, TIMER_B, (Period-9600));

	TimerMatchSet(WTIMER0_BASE, TIMER_B, 1);
	TimerEnable(WTIMER0_BASE, TIMER_B);
}

void ConfigureServo(void)
{
    unsigned int servo_stepval, servo_stepnow;
    unsigned int i;

    servo_stepval   = ( (SERVO_MAX - SERVO_MIN) / SERVO_STEPS );
    servo_stepnow   = SERVO_MIN;

    for (i = 0; i < (SERVO_STEPS+1); i++)
    {
    	servo_lut[i] = (Period-servo_stepnow);
    	servo_stepnow += servo_stepval;
    }
}

void SetServoPosition(uint32_t position)
{
	TimerMatchSet(WTIMER0_BASE, TIMER_B, position);
}

void SetServoAngle(uint32_t angle)
{
	SetServoPosition(servo_lut[angle]);
}
//------------------------------------------------------------------------------------------------------------------------------------

void ESP8266(void)
{
	bool TaskStatus;

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x00);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);

    timerInit();

    HardwareReset();

	InitUART();
	InitESPUART();

	InitServo();
	ConfigureServo();

	SetServoAngle(0);

	UARTprintf("Execute!\n");

	delay(1000);

	TaskStatus = ATesp();
	TaskStatus = RSTesp();
	TaskStatus = CWMODEesp();

	while(true)
	{
		UARTprintf("Trying to connect wi-fi\n");
		TaskStatus = CWJAPesp();
		if(TaskStatus)
		{
			UARTprintf("Connection is established!\n");
			break;
		}
	}

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);

	TaskStatus = CIPMUXesp();

	while(true)
	{
		TaskStatus = ProcessRoutine();
		if(TaskStatus)
		{
			CIPCLOSEesp();
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00); //Turn off Green Led
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04); //Turn on Blue Led
		}
		else
		{
			delay(10);
			CWQAPesp();
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00); //Turn off Blue Led
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02); //Turn on Red Led
		}
	}

}

bool ProcessRoutine()
{
	bool status;
	process = true;

	UARTprintf("Waiting Server...\n");

	while(true)
	{
		status = CIPSTARTesp();
		if(status)
		{
			UARTprintf("Communication is established!\n");
			break;
		}

		delay(50);
	}

	SetServoAngle(90);

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08); //TurnOn Green Led-> READY

	int i=0;
	char a;
	memset(ReceivedData, 0, sizeof(ReceivedData)); //Clear
	unsigned long start;

	while(process)
	{
		if(UARTCharsAvail(UART1_BASE))
		{
			if (SearchIndexOf(ReceivedData, "+IPD,") != -1)
			{
				i=0;
				memset(ReceivedData, 0, sizeof(ReceivedData));

				start = millis();

			    while (millis() - start < 5000)
			    {
			    	while(UARTCharsAvail(UART1_BASE))
			    	{
			    		a = UARTCharGet(UART1_BASE);
			    		if(a == '\0') continue;
			    		ReceivedData[i]= a;
			    		i++;
			    	}

		    		if (SearchIndexOf(ReceivedData, "\n") != -1)
		    		{
		    			break;
		    		}
			    }

				ProcessCommand(ReceivedData);

				i=0;
				memset(ReceivedData, 0, sizeof(ReceivedData));
			}
			else
			{
				a = UARTCharGet(UART1_BASE);
				if(a == '\0') continue;
				ReceivedData[i]= a;
				i++;
			}
		}
	}

	return true;
}

char *Substring(char *src, char *dst, int start, int stop)
{
	int len = stop - start;
	strncpy(dst, src + start, len);

	return dst;
}

int SearchIndexOf(char src[], char str[])
{
   int i, j, firstOcc;
   i = 0, j = 0;

   while (src[i] != '\0')
   {

      while (src[i] != str[0] && src[i] != '\0')
         i++;

      if (src[i] == '\0')
         return (-1);

      firstOcc = i;

      while (src[i] == str[j] && src[i] != '\0' && str[j] != '\0')
      {
         i++;
         j++;
      }

      if (str[j] == '\0')
         return (firstOcc);
      if (src[i] == '\0')
         return (-1);

      i = firstOcc + 1;
      j = 0;
   }

   return (-1);
}

char* itoa(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

void ftoa(float f,char *buf)
{
    int pos=0,ix,dp,num;
    if (f<0)
    {
        buf[pos++]='-';
        f = -f;
    }
    dp=0;
    while (f>=10.0)
    {
        f=f/10.0;
        dp++;
    }
    for (ix=1;ix<8;ix++)
    {
            num = f;
            f=f-num;
            if (num>9)
                buf[pos++]='#';
            else
                buf[pos++]='0'+num;
            if (dp==0) buf[pos++]='.';
            f=f*10.0;
            dp--;
    }
}
