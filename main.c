#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"


#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/rom_map.h"

#include "utils/uartstdio.h"
#include "timerdelay.h"

#define DATA GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3

	uint32_t data_doam[1];
	uint32_t data_anhsang[1];
	uint32_t voltage;
	uint32_t Resistor;
	char sms[100];
	int i=0, flag_u=0;

	void SIM_Start(void);

//configure and caculate do am

void ADC0_Init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

//	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC0_BASE,1,0,ADC_CTL_CH2|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
}

void Doam()
{
	ADCIntClear(ADC0_BASE, 1);
				ADCProcessorTrigger(ADC0_BASE, 1);

				while(!ADCIntStatus(ADC0_BASE, 1, false))
				{
				}

				ADCSequenceDataGet(ADC0_BASE,1,(uint32_t*)&data_doam);

				voltage= (float)((data_doam[0])*5/1024);
}

//configure and caculate lux

void ADC1_Init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);

	ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

//	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC1_BASE,1,0,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC1_BASE, 1);
}

void Anhsang()
{
	ADCIntClear(ADC1_BASE, 1);
				ADCProcessorTrigger(ADC1_BASE, 1);

				while(!ADCIntStatus(ADC1_BASE, 1, false))
				{
				}

				ADCSequenceDataGet(ADC1_BASE,1,(uint32_t*)&data_anhsang);

				Resistor= (float)(data_anhsang[0]);
}


// manual
void UART1IntHandler(void)
{
	uint32_t ulStatus;
	ulStatus = UARTIntStatus(UART1_BASE, true); //get interrupt status
	UARTIntClear(UART1_BASE, ulStatus); //clear the asserted interrupts
	while(UARTCharsAvail(UART1_BASE))
	{
		sms[i]=UARTCharGetNonBlocking(UART1_BASE);
		if(((i>=5)&&(sms[i-1]=='T')&&(sms[i-2]=='S')&&(sms[i-3]=='E')&&(sms[i-4]=='T'))||((i>=5)&&(sms[i-1]=='O')&&(sms[i-2]=='E')&&(sms[i-3]=='K'))||((i>=5)&&(sms[i-1]=='1')&&(sms[i-2]=='N')&&(sms[i-3]=='O')))
		{
			flag_u=1;
		}
		else i++;
	}
}

void UARTB_Init()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
	UARTStdioConfig(1, 115200, 16000000);
}

void ConfigureUARTB_Interrupt(void)
{
	IntEnable(INT_UART1);
	IntMasterEnable();
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

void Analyse_Command()
{
	if (flag_u==1)
	{
		SIM_Start();
	}
}

void SIM_Start()
{
	if((i>=5)&&(sms[i-1]=='T')&&(sms[i-2]=='S')&&(sms[i-3]=='E')&&(sms[i-4]=='T'))
	{
		GPIOPinWrite(GPIO_PORTF_BASE,DATA, 0X02);
		UARTprintf("AT+CMGS=\"+84914895340\"\n");
		SysCtlDelay(3000000);
		UARTprintf("Do am: ");
		UARTprintf("%d",voltage);
		UARTprintf("Anh sang: ");
		UARTprintf("%d",Resistor);
		SysCtlDelay(300000);
		UARTCharPut(UART1_BASE, 26);
		SysCtlDelay(300000);
		for (i=0; i<=5; i++)
		{
			sms[i]=0x00;
		}
		i=0;
		flag_u=0;

	}
	else if((i>=5)&&(sms[i-1]=='F')&&(sms[i-2]=='F')&&(sms[i-3]=='O'))
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);

	}
	for (i=0; i<=100; i++)
	{
		sms[i]=0x00;
	}
	i=0;
	flag_u=0;

}


int main(void)
{

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, DATA);
	GPIOPinWrite(GPIO_PORTF_BASE, DATA, 0x00);


	ADC0_Init();
	ADC1_Init();
	UARTB_Init();
	ConfigureUARTB_Interrupt();

	SysCtlDelay(SysCtlClockGet()/9);
	UARTprintf("AT+CMGF=1\r\n");
	UARTprintf("AT+CNMI=2,2,0,0,0\r\n");

	SysCtlDelay(SysCtlClockGet()/9);





	while(1)
	{
		Doam();
		Anhsang();

//		if(Resistor >3000)
//		{
//			GPIOPinWrite(GPIO_PORTF_BASE,DATA, 0X02);
//		}
//		else
//		{
//			GPIOPinWrite(GPIO_PORTF_BASE,DATA, 0X00);
//		}
//
//		if(voltage<9)
//		{
//			GPIOPinWrite(GPIO_PORTF_BASE,DATA, 0X04);
//		}
//		else
//		{
//			GPIOPinWrite(GPIO_PORTF_BASE,DATA, 0X00);
//		}
		Analyse_Command();
	}

}
