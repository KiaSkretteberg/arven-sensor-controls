/*
 * 36gp_555_27_en.c
 *
 * Created: 2023-02-25
 * Author: Kia Skretteberg
 * 
 * TODO: Characterize the encoders to see if they're too fast to even be useful for us or if 
 * the atmega is just too slow
 */
 #include <avr/io.h>
 #include "i2c.h"
 #include "encoder-36gp.h"
 #include "../mcp23017/mcp23017.h"
 
/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/

// value last read from portA, used for motors 5 and 6
volatile unsigned char portA_byte = 0;
 // value last read from portB, used for motors 1-4
volatile unsigned char portB_byte = 0;
//TCNT value of when we started checking speed
volatile unsigned int speedStart;
// TCNT value of when we stopped checking speed
volatile unsigned int speedEnd;

struct MotorPins {
	MCP23017_BITADDR pin1;
	MCP23017_BITADDR pin2;
};

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/

struct MotorPins determinePins(Encoder36GP_Motor motor);

MCP23017_PORT determinePort(Encoder36GP_Motor motor);


/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void Encoder36GP_InitAll(void)
{
	Encoder36GP_InitMotor(Encoder36GP_FL);
	Encoder36GP_InitMotor(Encoder36GP_FR);
}

void Encoder36GP_InitMotor(Encoder36GP_Motor motor)
{	
	// If the port expander isn't initialized yet, make sure it's initialized
	if(!MSCP23017_Initialized(MCP23017_PORTA)) MCP23017_Init(MCP23017_PORTA);
	if(!MSCP23017_Initialized(MCP23017_PORTB)) MCP23017_Init(MCP23017_PORTB);
	
	struct MotorPins pins;
	MCP23017_PORT port;
	
	// determine which port the encoders are on
	port = determinePort(motor);

    // determine which 2 pins (bits) the encoders are on in the port
    pins = determinePins(motor);
	
	// set both pins to be inputs
	MCP23017_SetPin(MCP23017_PinMode_INPUT, port, pins.pin1);
	MCP23017_SetPin(MCP23017_PinMode_INPUT, port, pins.pin2);
}

unsigned char Encoder36GP_CheckDirection(Encoder36GP_Motor motor)
{
	struct MotorPins pins = determinePins(motor);
	MCP23017_PORT port = determinePort(motor);
	unsigned char prevValue = 0;
	unsigned char newValue = MCP23017_ReadPort(port);
	unsigned char direction;
	
	switch(port)
	{
		case MCP23017_PORTA:
			// capture the old portA value for comparison
			prevValue = portA_byte;
			// store the new portA value for future comparison
			portA_byte = newValue;
			break;
		case MCP23017_PORTB:
			// capture the old portB value for comparison
			prevValue = portB_byte;
			// store the new portB value for future comparison
			portB_byte = newValue;
			break;
	}
	
	/*
	     _______         _______         _______
	pin1  A     |_____D_|       |_______|       |_______
	         _______         _______         _______
	pin2 ___| B   C |_______|       |_______|       |___
	
	*/
	{
		unsigned char A = (prevValue & pins.pin1) && !(prevValue & pins.pin2);
		unsigned char B = (newValue & pins.pin1) && (newValue & pins.pin2);
		unsigned char C = !(prevValue & pins.pin1) && (prevValue & pins.pin2);
		unsigned char D = !(newValue & pins.pin1) && !(newValue & pins.pin2);
	
		// A & B OR C & D
		if((A && B) || (C && D)) 
		{
			direction = 1; // forward?? cw? I dunno, will need to determine experimentally
		}
		else
		{
			direction = 0; // backward?? ccw? I dunno, will need to determine experimentally
		}
	}
	return direction;
}

unsigned long Encoder36GP_CheckSpeed(Encoder36GP_Motor motor)
{
	//TODO: set speed start/speed end somehow
	return speedStart <= speedEnd ? speedEnd - speedStart : 65535 - speedStart + speedEnd;
}


//TODO: Build function to read data from the pin
		//example for MCP23017, toggling PORTB PIN1 high and low depending on 
		//the input read from pin0
		/*if(MCP23017_ReadPin(MCP23017_PORTB,MCP23017_BIT0_ADDR) == 1){
			//MCP23017_Send(MCP23017_OUTPUT_HIGH,MCP23017_PORTB,MCP23017_BIT1_ADDR);
		} else{
			//MCP23017_Send(MCP23017_OUTPUT_LOW,MCP23017_PORTB,MCP23017_BIT1_ADDR);
		}*/
		//MCP23017_ReadPin(MCP23017_PORTB,MCP23017_BIT0_ADDR);


/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/


MCP23017_PORT determinePort(Encoder36GP_Motor motor)
{
	MCP23017_PORT port;
	switch(motor)
	{
		case Encoder36GP_FL:
		case Encoder36GP_FR:
		// ML, MR
			port = MCP23017_PORTB;
			break;
		// BL, BR
			// port A
	}
	return port;
}

struct MotorPins determinePins(Encoder36GP_Motor motor)
{
	struct MotorPins pins;
	
	switch(motor)
	{
		case Encoder36GP_FL:
			pins.pin1 = MCP23017_BIT0_ADDR; // pin 1
			pins.pin2 = MCP23017_BIT0_ADDR; // pin 2
			break;
		case Encoder36GP_FR:
			pins.pin1 = MCP23017_BIT2_ADDR; // pin 3
			pins.pin2 = MCP23017_BIT3_ADDR; // pin 4
			break;
	}
	return pins;
}