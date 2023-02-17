/*
 * ArvenSensorControls.c
 *
 * Created: 2023-01-30 8:22:18 PM
 * Author : Kia Skretteberg
 */ 

#define F_CPU 2E6 // with external xtal enabled, and clock div/8, bus == 2MHz
#include <avr/io.h>
#include <util/delay.h> // have to add, has delay implementation (requires F_CPU to be defined)
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "atd.h"
#include "sci.h"
#include "gd03.h"
#include <stdio.h>
// global variables
// constant for timer output compare offset, init and ISR rearm
const unsigned int _Timer_OC_Offset = 250; // 1 / (2000000 / 8 / 250) = 1ms (prescale 8)
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
	SCI0_TxString("\n328 Up! Characters will echo.\r");
	AtoD_Init(AtoD_Channel_0);

	// set the global interrupt flag (enable interrupts)
	// this is backwards from the 9S12
	sei();
	// make portd pin 7 an output (PD7)
	DDRD |= 0b10000000;


	// main program loop - don't exit
	while(1)
	{
		// go idle!
		sleep_cpu();

		//// are we past the scheduled event?
		if (uiAtoDEventNext - _Ticks > cuiAtoDEventCount)
		{
			uiAtoDEventNext += cuiAtoDEventCount; // rearm
			measure_gd03_voltage();
		}

	}
}
// output compare A interrupt
ISR(TIMER1_COMPA_vect)
{
	// rearm the output compare operation
	OCR1A += _Timer_OC_Offset; // 1ms intervals

	// up the global tick count
	++_Ticks;
}
