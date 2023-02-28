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
void trigger(int pin);
 
// Wait for the echo signal to go low from the specified pin
long waitForEcho(int pin);
 
// Calculate the distance (in cm) of the sound pulse from the duration (in us)
float calculateDistance(long duration);
 
 
long echoTimeStart;
long echoTimeEnd;
 
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
			DDRD |= 0b00100000; //output
			DDRD &= ~(0b01000000); //input
			
			PCMSK2 |= 0b01000000; // turn on PCINT22 pin mask (enable interrupts) (12.2.6)
			PCICR |= 0b00000100; // turn on interrupts for group 2 (12.2.4)
			break;
		case HCSR04_C:
			DDRD |= DDD7; //output
			DDRB &= ~DDB0; //input
			PCMSK0 |= 0b00000001; // turn on PCINT0 pin mask (enable interrupts) (12.2.8)
			PCICR |= 0b00000001; // turn on interrupts for group 0 (12.2.4)
			break;
		case HCSR04_R:
			DDRB |= DDB1; //output
			DDRB &= ~DDB2; //input
			PCMSK0 |= 0b00000100; // turn on PCINT2 pin mask (enable interrupts) (12.2.8)
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
 {char buff[20];
	 long duration = 0;
	 float distance = 0.0;
	 
	 echoTimeStart = 0;
	 echoTimeEnd = 0;
	 
	 switch(device)
	 {
		case HCSR04_L:
			trigger(HCSR04_L_Trig);
			duration = waitForEcho(HCSR04_L_Echo);
			distance = calculateDistance(duration);
			sprintf(buff, "%f", distance);
			SCI0_TxString(buff);
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

 void HCSR04_ISR(int pin)
 {
	 //TODO: use pin instead of 0b01000000
	 if(PIND & 0b01000000)
		echoTimeStart = TCNT0;
	 else
		echoTimeEnd = TCNT0;
 }


/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/

void trigger(int pin)
{
	//TODO: use pin instead of 0b01000000
	PORTD &= ~(0b00100000);
	_delay_us(2); //just to ensure we're starting a fresh pulse
	//pin = 1;
	PORTD |= 0b00100000;
	_delay_us(10); //need to wait a minimum of 10us for the 8 pulses to be sent, according to the datasheet (see header file)
	//pin = 0;
	PORTD &= ~(0b00100000);
}

long waitForEcho(int pin)
{
	//TODO: use pin instead of 0b01000000
	long duration = 0;
		
	// wait for the pin to go low
	while(PIND & 0b01000000);
	
	duration = echoTimeEnd - echoTimeStart;
		
	return duration;
}

float calculateDistance(long duration)
{
	float speed = 0.0343; // speed of sound in cm/us -- speed pulled from google as 343m/s in dry air at 20C on Feb 24th, 2023
	return (duration * speed) / 2; //calculation retrieved from datasheet (see header file)
}
