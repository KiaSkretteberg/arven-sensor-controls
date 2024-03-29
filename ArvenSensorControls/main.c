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
#include "mcp23017\mcp23017.h"
#include "pico\pico.h"
#include <stdio.h>
#define LED 0b00000100 // PC2, pin 25

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/

// constant for timer output compare offset, init and ISR rearm
const unsigned int _Timer_OC_Offset = 1000; // 1 / (16000000 / 8 / 1000) = 0.5ms (prescale 8) -- wanted prescale 16
const unsigned int timerEventCount = 2000; // every 100 ms
// global counter for timer ISR, used as reference to coordinate activities
volatile unsigned int _Ticks = 0;
// global tracker for bump sensor data
volatile char bump_L = 0;
volatile char bump_R = 0;


/************************************************************************/
/* Main Program Loop                                                    */
/************************************************************************/

int main(void)
{	
	// make portc2 (pin 25) an output (PC2)
	DDRC |= LED;
	// one-time initialization section
	// bring up the timer, requires ISR!
	Timer_Init(Timer_Prescale_8, _Timer_OC_Offset); // 1ms intervals
	// enable sleep mode, for idle, sort of similar to WAI on 9S12X (13.2)
	sleep_enable();
	// bring up the I2C bus, at 400kHz operation
	I2C_Init(F_CPU, I2CBus400);
	GD03_Init();
	SEN0427_InitDevice(SEN0427_R);
	//SEN0427_InitAll();
	//MCP23017_Init(MCP23017_PORTB);	
	
	// requires ISR for PCI2
	Back_Sens_InitAll();
	// requires ISR for PCI2 & PCI0
	HCSR04_InitAll();
	// not compatible with SCI initialization
	Pico_InitCommunication();
	
	// set the global interrupt flag (enable interrupts)
	// this is backwards from the 9S12
	sei();
		struct PicoFrame frame;
		frame.Bump_R = 0;
		frame.Bump_L = 0;
		frame.Ultrasonic_C_Duration = 0;
		frame.Ultrasonic_R_Duration = 0;
		frame.Ultrasonic_L_Duration = 0;	
		frame.IR_L_Distance = 0;
		frame.IR_R_Distance = 0;
		frame.Motor_FL_Direction = 0;
		frame.Motor_FR_Direction = 0;
		frame.Motor_FL_Speed = 0;
		frame.Motor_FR_Speed = 0;
		frame.Battery_Low = 0;
		frame.Weight = 0;
	// main program loop - don't exit
	while(1)
	{
		sleep_cpu();

		//PORTC ^= LED;
		// go idle!
		//PORTD |= 0b00010000;
		//DDRB |= HCSR04_R_Trig;
		//PORTB |= HCSR04_R_Trig;
		//PORTD &= ~HCSR04_R;
		if(_Ticks > timerEventCount){
			_Ticks = 0;
			PORTC ^= LED;
			frame.Weight = GD03_CaptureAtoDVal();
			//PORTC &= ~LED;		
			 frame.Ultrasonic_L_Duration = HCSR04_GetEchoDuration(HCSR04_L);
			frame.Bump_L = bump_L;
			frame.Bump_R = bump_R;
			frame.Ultrasonic_C_Duration = HCSR04_GetEchoDuration(HCSR04_C);
			//
			frame.Ultrasonic_R_Duration = HCSR04_GetEchoDuration(HCSR04_R);
			//
			//frame.IR_L_Distance = SEN0427_CaptureDistance(SEN0427_L);
			//
			frame.IR_R_Distance = SEN0427_CaptureDistance(SEN0427_R);
			//PORTC &= ~LED;
			//TODO: Set up code to retrieve battery level from GPIO

			//TODO: Set up encoder data	
			Pico_SendData(frame);						
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

	bump_L = Back_Sens_ISR(Back_Sens_L);
	bump_R = Back_Sens_ISR(Back_Sens_R);
}

// ISR for PCI0, covering PCINT0 through PCINT8
ISR (PCINT0_vect)
{
	HCSR04_ISR();
}
