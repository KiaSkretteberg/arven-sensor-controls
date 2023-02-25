/*
 * hc_sr04.c
 *
 * Created: 2023-02-24 1:36:28 PM
 *  Author: Kia Skretteberg
 */
 #define F_CPU 2E6 // with external xtal enabled, and clock div/8, bus == 2MHz
 #include <avr/io.h>
 #include <stdio.h>
 #include <util/delay.h> // have to add, has delay implementation (requires F_CPU to be defined)
 #include "hc-sr04.h"
 
 /************************************************************************/
 /* Local Definitions (private functions)                                */
 /************************************************************************/
 
 // Toggle the specified pin low for 2us, then high for 10us, then back to low, in order to send out a pulse
 void trigger(int pin);
 
 // Wait for the echo signal to go low from the specified pin
 long waitForEcho(int pin);
 
 // Calculate the distance (in cm) of the sound pulse from the duration (in us)
 float calculateDistance(long duration);
 
 
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
			DDRD |= DDD5; //output
			DDRD &= ~DDD6; //input
			break;
		case HCSR04_C:
			DDRD |= DDD7; //output
			DDRB &= ~DDB0; //input
			break;
		case HCSR04_R:
			DDRB |= DDB1; //output
			DDRB &= ~DDB2; //input
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
	 long duration = 0;
	 float distance = 0.0;
	 
	 switch(device)
	 {
		case HCSR04_L:
			trigger(HCSR04_L_Trig);
			duration = waitForEcho(HCSR04_L_Echo);
			distance = calculateDistance(duration);
			break;
		case HCSR04_C:
			trigger(HCSR04_C_Trig);
			duration = waitForEcho(HCSR04_C_Echo);
			distance = calculateDistance(duration);
			break;
		case HCSR04_R:
			trigger(HCSR04_R_Trig);
			duration = waitForEcho(HCSR04_R_Echo);
			distance = calculateDistance(duration);
			break;
	 }
	 return distance;
 }


/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/

void trigger(int pin)
{
	pin = 0;
	_delay_us(2); //just to ensure we're starting a fresh pulse
	pin = 1;
	_delay_us(10); //need to wait a minimum of 10us for the 8 pulses to be sent, according to the datasheet (see header file)
	pin = 0;
}

long waitForEcho(int pin)
{
	long duration = 0;
	
	// wait for the pin to go low (the pin is high while waiting for an echo and will go low once it's returned)
	// track how long the pin is high for (in us) as that's the time it takes for the echo to be received
	while(pin)
	{
		_delay_us(1);
		++duration;
	}
		
	return duration;
}

float calculateDistance(long duration)
{
	float speed = 0.0343; // speed of sound in cm/us -- speed pulled from google as 343m/s in dry air at 20C on Feb 24th, 2023
	return (duration * speed) / 2; //calculation retrieved from datasheet (see header file)
}
