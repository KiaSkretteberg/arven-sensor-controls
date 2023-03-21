/*
 * ArvenSensorControls.c
 *
 * Created: 2023-01-30 8:22:18 PM
 * Author : Kia Skretteberg
 */ 

#define F_CPU 16E6 // with external xtal enabled, and clock div/8, bus == 2MHz
#include <avr/io.h>
#include <util/delay.h> // have to add, has delay implementation (requires F_CPU to be defined)
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "atd.h"
#include "i2c.h"
#include "sci.h"
#include "gd03/gd03.h"
#include "hc-sr04/hc-sr04.h"
#include "sen0427/sen0427.h"
#include "backup-sens/backup-sens.h"
#include <stdio.h>
#define LED 0b00000100 // PC2, pin 25

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/

// constant for timer output compare offset, init and ISR rearm
const unsigned int _Timer_OC_Offset = 1000; // 1 / (16000000 / 8 / 1000) = 0.5ms (prescale 8) -- wanted prescale 16
// global counter for timer ISR, used as reference to coordinate activities
volatile unsigned int _Ticks = 0;


/************************************************************************/
/* Main Program Loop                                                    */
/************************************************************************/

int main(void)
{
	// variable for managing the A/D update
	const unsigned int cuiAtoDEventCount = 1000; // every 1/2 second
	unsigned int uiAtoDEventNext = cuiAtoDEventCount;
	// make portc2 (pin 25) an output (PC2)
	DDRC |= LED;
	// one-time initialization section
	// bring up the timer, requires ISR!
	Timer_Init(Timer_Prescale_8, _Timer_OC_Offset); // 1ms intervals
	// enable sleep mode, for idle, sort of similar to WAI on 9S12X (13.2)
	sleep_enable();
	// bring up the I2C bus, at 400kHz operation
	I2C_Init(F_CPU, I2CBus400);

	// Cannot be used while pico is initialized
	//SCI0_Init(F_CPU, 9600, 1); // 16Mhz clock, 9600 baud
	// welcome message, so we know it booted OK
	//SCI0_TxString("\n328 Up! Characters will echo.\n");

	GD03_Init();
	SEN0427_InitDevice(SEN0427_L);
	MCP23017_Init(MCP23017_PORTB);	
	MCP23017_SetPin(MCP23017_INPUT, MCP23017_PORTB, MCP23017_BIT0_ADDR);
	MCP23017_SetPin(myMode, myPort, myPin);	
	// requires ISR for PCI2
	Back_Sens_InitAll();
	// requires ISR for PCI2 & PCI0
	HCSR04_InitAll();


	// set the global interrupt flag (enable interrupts)
	// this is backwards from the 9S12
	sei();


	// main program loop - don't exit
	while(1)
	{
		char buff[200];
		/*switch(SEN0427_GetRangeResult(SEN0427_L))
		{
			case SEN0427_RangeResult__NO_ERR:
				char distance = SEN0427_GetSingleMeasurement(SEN0427_L);
				break;
			default:
				PORTC |= LED; // turn on LED
				break;
		}*/
		//long echoDurationL = HCSR04_GetEchoDuration(HCSR04_L);

		// go idle!
		sleep_cpu();
		//example for MCP23017, toggling PORTB PIN1 high and low depending on 
		//the input read from pin0
		/*if(MCP23017_ReadPin(MCP23017_PORTB,MCP23017_BIT0_ADDR) == 1){
			//MCP23017_Send(MCP23017_OUTPUT_HIGH,MCP23017_PORTB,MCP23017_BIT1_ADDR);
		} else{
			//MCP23017_Send(MCP23017_OUTPUT_LOW,MCP23017_PORTB,MCP23017_BIT1_ADDR);
		}*/
		//MCP23017_ReadPin(MCP23017_PORTB,MCP23017_BIT0_ADDR);
		//// are we past the scheduled event?
		if (uiAtoDEventNext - _Ticks > cuiAtoDEventCount)
		{
			uiAtoDEventNext += cuiAtoDEventCount; // rearm
			//int gd03_atod = GD03_CaptureAtoDVal();
		}
	}
}

/************************************************************************/
/* ISRs				                                                    */
/************************************************************************/

// output compare A interrupt for timer
ISR (TIMER1_COMPA_vect)
{
	// rearm the output compare operation
	OCR1A += _Timer_OC_Offset; // 1ms intervals

	// up the global tick count
	++_Ticks;
}

// ISR for PCI2, covering PCINT23 through PCINT16
ISR (PCINT2_vect)
{
	HCSR04_ISR();
	
	// hit something, turn on led
	/*if(Back_Sens_ISR(Back_Sens_L))
	{
		PORTC |= LED; // turn on LED
	}
	else
	{
		PORTC &= ~LED; // turn off LED
	}*/
}

// ISR for PCI0, covering PCINT0 through PCINT8
ISR (PCINT0_vect)
{
	HCSR04_ISR();	
}
