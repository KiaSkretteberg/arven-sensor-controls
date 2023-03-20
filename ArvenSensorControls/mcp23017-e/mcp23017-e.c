/*
* MCP23017.h
* MCP23017 Port Expander
*
* Allows for up to 16 GPI/O pins to be utilized
* via I2C. These are addressed via 2 different 
* ports : PORTA & PORTB
*
* Datasheet: 
* https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf
*
* ********* NOTE: *************
* This is using IOCON.BANK = 0 (default), I2C at 400khz. Using SCI
* for debugging purposes
* https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf#page=16
*
*Other references used:
*https://arduinodiy.wordpress.com/2017/03/14/adding-an-mcp23017-16-port-io-expander-to-arduino-or-esp8266/
*
* Created: 2023-03-10
* Author: Nubal Manhas
*/

#include <atmel_start.h>
#include <stdio.h>
#include "atd.h"
#include <util/delay.h>
#include "I2C.h"
#include <stdlib.h>
#include "utils/utils.h"
#include "sci.h"
#include <string.h>
#include "mcp23017.h"

bool portA_initialized = 0;
bool portB_initialized = 0;

/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

bool MSCP23017_Initialized(MCP23017_PORT port)
{
	return port == MCP23017_PORTA ? portA_initialized : portB_initialized;
}

void MCP23017_Init(MCP23017_PORT port){
	unsigned char c;
	char b[20];
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(port, 0); //Set to register 
	I2C_Start(MCP23017_Addr, 1);
	I2C_Read8(&c, 0, 1);
	sprintf(b, "%x", c);
	SCI0_TxString("\nInit Port (Before Write): ");
	SCI0_TxString(b);
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(port, 0); //Set to register 	
	I2C_Write8(0xff, 1); //Set all pins to input on the specified port
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(port, 0); //Set to register 
	I2C_Start(MCP23017_Addr, 1);
	I2C_Read8(&c, 0, 1);
	sprintf(b, "%x", c);
	SCI0_TxString("\nInit Port (After Write): ");
	SCI0_TxString(b);
}

void MCP23017_SetPin(PinMode mode, MCP23017_PORT port, MCP23017_BITADDR pin){
	unsigned char c;
	char b[20];
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(port, 0); //Set to register 
	I2C_Start(MCP23017_Addr, 1);
	I2C_Read8(&c, 0, 1);
	sprintf(b, "%x", c);
	SCI0_TxString("\nPin Set First Write: (Before Write): ");
	SCI0_TxString(b);
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(port, 0); //Set to register 
	//check to see the pinmode desired (input or output), and
	//then check if the pin is already set to that pinmode
	if(mode == MCP23017_PinMode_INPUT && !(c & (1<<pin))){
		c|= pin;
	}
	if(mode == MCP23017_PinMode_OUTPUT && (c & (1<<pin))){
		c &= ~pin;
	}
	sprintf(b, "%x", c);
	SCI0_TxString("\nSetting Pin (Before Write): ");
	SCI0_TxString(b);
	I2C_Write8(c, 1); //Set input/output to specified pin
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(port, 0); //Set to register 
	I2C_Start(MCP23017_Addr, 1);
	I2C_Read8(&c, 0, 1);
	sprintf(b, "%x", c);
	SCI0_TxString("\nSetting Pin (After Write): ");
	SCI0_TxString(b);
}

char MCP23017_ReadPin(MCP23017_PORT port, MCP23017_BITADDR pin){
	unsigned char c;
	char b[20];
	I2C_Start(MCP23017_Addr, 0); //start I2C on the MCP address
	//check the port chosen, the r/w
	//address will change depending
	//on the port
	//
	//0x12 = PORTA
	//0x13 = PORTB	
	if(port == MCP23017_PORTB){
		I2C_Write8(0x13, 0); //address port B
	} 
	if(port == MCP23017_PORTA){
		I2C_Write8(0x12, 0); //address port A
	}	
	I2C_Start(MCP23017_Addr, 1); //start I2C, with read
	I2C_Read8(&c, I2C_NACK, 1); //store the read byte into c, no ack
	sprintf(b, "%i", c);
	SCI0_TxString("\nReadFromPort: ");
	SCI0_TxString(b);	
	//&= to get what the bit is set to at the specified pin
	c &= pin;
	sprintf(b, "%i", c);
	SCI0_TxString("\nReadFromPort: ");
	SCI0_TxString(b);
	return c;
}

void MCP23017_Send(MCP23017_OUTPUT output, MCP23017_PORT port, MCP23017_BITADDR pin){
	//value after addressing the port
	//will use this to append the output 
	//we want after
	unsigned char c;
	//buffer for sprintf, using for debug purposes 
	char b[20];
	//stores the port address for read/write
	char portADDR;
	SCI0_TxString("\nSending stuff");
	//I2C startup routine
	I2C_Start(MCP23017_Addr, 0);
	//check the port chosen, the r/w
	//address will change depending
	//on the port
	//
	//0x12 = PORTA
	//0x13 = PORTB
	if(port == MCP23017_PORTA){
		I2C_Write8(0x12, 0); //address port A
		portADDR = 0x12;
	}
	if(port == MCP23017_PORTB){
		I2C_Write8(0x13, 0); //address port B
		portADDR = 0x13;
	}
	I2C_Start(MCP23017_Addr, 1);
	//read + store the value from the r/w address
	I2C_Read8(&c, 0, 1);
	sprintf(b, "%x", c);
	SCI0_TxString("\nMCP23017_Send Read: ");
	SCI0_TxString(b);
	//check if the output is high (ie. MCP23017_OUTPUT_HIGH).
	//we would want to set the bit given (ie. pin) to high
	//within the r/w address, so we simply |= the corresponding
	//bit. we &= and 1's compliment if we want that bit to be set to 
	//low
	if(output){
		c |= pin;
		SCI0_TxString("\nSending high");
	} else{
		c&= ~pin;
	}
	sprintf(b, "%x", c);
	SCI0_TxString("\nMCP23017_Send Write: ");
	SCI0_TxString(b);
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(portADDR, 0);
	//write the new data back, with the desired bit now set to high/low 
	I2C_Write8(c, 1); 

	device_initialized = 1;
}