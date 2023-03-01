/*
 * hc_sr04.c
 * HC-SR04 Ultrasonic Sensor(s)
 * Utilizes GPIO
 *
 * Created: 2023-02-24 1:36:28 PM
 *  Author: Kia Skretteberg
 */
 #define F_CPU 2E6 // with external xtal enabled, and clock div/8, bus == 2MHz
 #include <avr/io.h>
 #include <stdio.h>
 #include <util/delay.h> // have to add, has delay implementation (requires F_CPU to be defined)
 #include "hc-sr04.h"
 #include "sci.h"
 
/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/
 
// Toggle the specified pin low for 2us, then high for 10us, then back to low, in order to send out a pulse
void trigger(HCSR04_Device device);
 
// Wait for the echo signal to go low from the specified pin
long waitForEcho(HCSR04_Device device);
 
// Calculate the distance (in cm) of the sound pulse from the duration (in us)
float calculateDistance(long duration);
 
 
/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/

volatile long echoTimeStart = 0;
volatile long echoTimeEnd = 0;
volatile HCSR04_Device activeDevice = HCSR04_None;

 
/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

 void HCSR04_InitAll(void)
 {
	 HCSR04_InitDevice(HCSR04_L);
	 HCSR04_InitDevice(HCSR04_C);
	 HCSR04_InitDevice(HCSR04_R);
 }

 void HCSR04_InitDevice(HCSR04_Device device)
 {
	 switch(device)
	 {
		case HCSR04_L:
			DDRD |= HCSR04_L_Trig; //output
			DDRD &= ~HCSR04_L_Echo; //input
			
			PCMSK2 |= HCSR04_L_Echo; // turn on PCINT22 pin mask (enable interrupts) (12.2.6)
			PCICR |= 0b00000100; // turn on interrupts for group 2 (12.2.4)
			break;
		case HCSR04_C:
			DDRD |= HCSR04_C_Trig; //output
			DDRB &= ~HCSR04_C_Echo; //input
			
			PCMSK0 |= HCSR04_C_Echo; // turn on PCINT0 pin mask (enable interrupts) (12.2.8)
			PCICR |= 0b00000001; // turn on interrupts for group 0 (12.2.4)
			break;
		case HCSR04_R:
			DDRB |= HCSR04_R_Trig; //output
			DDRB &= ~HCSR04_R_Echo; //input
			
			PCMSK0 |= HCSR04_R_Echo; // turn on PCINT2 pin mask (enable interrupts) (12.2.8)
			PCICR |= 0b00000001; // turn on interrupts for group 0 (12.2.4)
			break; 
		/*case HCSR04_All:
			DDRD |= 0b1010000; // 5 and 7 are output
			DDRD &= ~(0b01000000) //6 is input
			DDRB |= 0b0000010; // 1 is output
			DDRB &= ~(0b01000101) //0 and 2 are input
			break;*/
	 }
 }
 
int HCSR04_CheckForObstacle(HCSR04_Device device, float distance)
{
	float dDistance = HCSR04_GetEchoDistance(device);
	 
	return dDistance < distance ? 1 : 0;
}


 float HCSR04_GetEchoDistance(HCSR04_Device device)
 {
	 char buff[20];
	 long duration = 0;
	 float distance = 0.0;
	 
	 echoTimeStart = 0;
	 echoTimeEnd = 0;
	 
	 switch(device)
	 {
		case HCSR04_L:
			trigger(HCSR04_L);
			duration = waitForEcho(HCSR04_L);
			distance = calculateDistance(duration);
			sprintf(buff, "%f", distance);
			SCI0_TxString(buff);
			break;
		case HCSR04_C:
			trigger(HCSR04_C);
			duration = waitForEcho(HCSR04_C);
			distance = calculateDistance(duration);
			break;
		case HCSR04_R:
			trigger(HCSR04_R);
			duration = waitForEcho(HCSR04_R);
			distance = calculateDistance(duration);
			break;
	 }
	 return distance;
 }

 void HCSR04_ISR()
 {
	 // Only perform the check if there's an active device
	 if(activeDevice != HCSR04_None)
	 {
		 int condition = 0;
		 // determine the high condition for the active device
		 switch(activeDevice)
		 {
			 case HCSR04_L:
				condition = PIND & HCSR04_L_Echo;
				break;
			 case HCSR04_C:
				condition = PINB & HCSR04_C_Echo;
				break;
			 case HCSR04_R:
				condition = PINB & HCSR04_R_Echo;
				break;
		 }
		 
		 // When the echo starts, track current TCNT value
		 if(condition)
		 {
			 echoTimeStart = TCNT0;
		 }
		 // When echo ends, track the new TCNT value and indicate no device is active
		 else
		 {
			 echoTimeEnd = TCNT0;
			 activeDevice = HCSR04_None;
		 }
	 }
 }


/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/

int trigger(HCSR04_Device device)
{
	// ensure there is no active device
	if(activeDevice == HCSR04_None)
	{
		int pin;
		
		// set the device to be active
		activeDevice = device;
		
		// Determine which pin needs to be toggled based on the device
		switch(activeDevice)
		{
			case HCSR04_L:
				pin = HCSR04_L_Trig;
				break;
			case HCSR04_C:
				pin = HCSR04_C_Trig;
				break;
			case HCSR04_R:
				pin = HCSR04_R_Trig;
				break;
		}
	
		// set pin low for 2 us to ensure we're starting with a fresh pulse
		PORTD &= ~pin;
		_delay_us(2);
		
		// set the pin high for a minimum of 10us to ensure the 8 pulses are sent, according to the datasheet (see header file)
		PORTD |= pin;
		_delay_us(10); 
		PORTD &= ~pin;
		
		// successful trigger
		return 1;
	}
	
	// there was an active device so we couldn't trigger
	return 0;
}

long waitForEcho(HCSR04_Device device)
{
	// ensure this is the active device before waiting for echo
	if(activeDevice == device)
	{
		long duration = 0;
		
		// wait for the device to no longer be active, meaning the echo finished
		while(activeDevice == device);
		
		return echoTimeEnd - echoTimeStart;
	}
	
	// active device is not this device, return invalid duration
	return -1;
}

float calculateDistance(long duration)
{
	float speed = 0.0343; // speed of sound in cm/us -- speed pulled from google as 343m/s in dry air at 20C on Feb 24th, 2023
	return (duration * speed) / 2; //calculation retrieved from datasheet (see header file)
}
