/*
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
#include "MCP23017.h"

void MCP23017_Send(MCP23017_OUTPUT output, MCP23017_PORT port, MCP23017_BITADDR pin){
	unsigned char c;
	char b[20];
	char portADDR;
	SCI0_TxString("\nSending stuff");
	I2C_Start(MCP23017_Addr, 0);
	if(port == MCP23017_PORTA){
		I2C_Write8(0x12, 0); //address port A
		portADDR = 0x12;
	}
	if(port == MCP23017_PORTB){
		I2C_Write8(0x13, 0); //address port B
		portADDR = 0x13;
	}
	I2C_Start(MCP23017_Addr, 1);
	I2C_Read8(&c, 0, 1);
	sprintf(b, "%x", c);
	SCI0_TxString("\nMCP23017_Send Read: ");
	SCI0_TxString(b);
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
	I2C_Write8(portADDR, 0); //data to send to port
	I2C_Write8(c, 1); //data to send to port

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
	SCI0_TxString("\nInit Port  (Before Write): ");
	SCI0_TxString(b);
	I2C_Start(MCP23017_Addr, 0);
	I2C_Write8(port, 0); //Set to register 
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
	c &= pin;
	sprintf(b, "%i", c);
	SCI0_TxString("\nReadFromPort: ");
	SCI0_TxString(b);
	return c;
}
