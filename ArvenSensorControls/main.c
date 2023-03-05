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
#include "sci.h"
#include "gd03/gd03.h"
#include "hc-sr04/hc-sr04.h"
#include <stdio.h>
#define LED 0b00000100 // PC2, pin 25

// global variables
// constant for timer output compare offset, init and ISR rearm
const unsigned int _Timer_OC_Offset = 1000; // 1 / (16000000 / 8 / 1000) = 0.5ms (prescale 8) -- wanted prescale 16
// global counter for timer ISR, used as reference to coordinate activities
volatile unsigned int _Ticks = 0;
int main(void)
{
	// variable for managing the A/D update
	const unsigned int cuiAtoDEventCount = 500; // every 1/2 second
	unsigned int uiAtoDEventNext = cuiAtoDEventCount;
	// one-time initialization section
	// bring up the timer, requires ISR!
	Timer_Init(Timer_Prescale_8, _Timer_OC_Offset); // 1ms intervals
	// enable sleep mode, for idle, sort of similar to WAI on 9S12X (13.2)
	sleep_enable();
	SCI0_Init(F_CPU, 9600, 1); // 16Mhz clock, 9600 baud

	// welcome message, so we know it booted OK
	SCI0_TxString("\n328 Up! Characters will echo.\n");
	GD03_Init();
	// requires ISR for PCI2
	HCSR04_InitDevice(HCSR04_L);


	// set the global interrupt flag (enable interrupts)
	// this is backwards from the 9S12
	sei();
	// make portc2 (pin 25) an output (PC2)
	DDRC |= LED;


	// main program loop - don't exit
	while(1)
	{
		if(HCSR04_CheckForObstacle(HCSR04_L, 10))
		{
			PORTC |= LED; // turn on LED
		}
		else
		{
			PORTC &= ~LED; // turn off LED
		}
		// go idle!
		sleep_cpu();

		//// are we past the scheduled event?
		if (uiAtoDEventNext - _Ticks > cuiAtoDEventCount)
		{
			uiAtoDEventNext += cuiAtoDEventCount; // rearm
			//GD03_LoadState gd03Load = GD03_CheckForLoad();
			
			/*if(gd03Load == GD03_LoadPresent)
			{
				PORTC |= LED; // turn on LED
			}
			else
			{
				PORTC &= ~LED; // turn off LED
			}*/
		}
	}
}
// output compare A interrupt for timer
ISR(TIMER1_COMPA_vect)
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
}
