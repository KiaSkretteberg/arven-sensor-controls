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
* This is using IOCON.BANK = 0 (default), I2C at 400khz
* https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf#page=16
*
* Created: 2023-03-10
* Author: Nubal Manhas
*/

/* I2C address for MCP23017.
* Our I2C library adjusts it
* for 7 bit addressing, so no
* need to account for that.
* This is assuming that A0, A1, and A2 are 
* grounded. 
*/
#define MCP23017_Addr 0x20

/*
* Each pin can be configured as an input/output
*
* According to the datasheet:
* https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf#page=18
*
* 1 = input
* 0 = output
*/
typedef enum
{
	MCP23017_PinMode_INPUT = 1,
	MCP23017_PinMode_OUTPUT = 0
} MCP23017_PinMode;

/*
* According to the datasheet:
* https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf#page=12
* 
* There are two ports to control 8 pins each.
* You can enable each I/O as an input or output (ie. the
* PinMode enum from above) by writing to the I/O 
* configuration (IODIRA/IODIRB) bits. 
*
* The I/O configuration bits are as follows:
* https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/MCP23017-Data-Sheet-DS20001952.pdf#page=16
* 
* PORTA (IODIRA) = 0x00
* PORTB (IODIRB) = 0x01
*/
typedef enum
{
	MCP23017_PORTA = 0x00,
	MCP23017_PORTB = 0x01
} MCP23017_PORT;

/*
* 
*/
typedef enum
{
	MCP23017_BIT0_ADDR = 0b00000001,
	MCP23017_BIT1_ADDR = 0b00000010,
	MCP23017_BIT2_ADDR = 0b00000100,
	MCP23017_BIT3_ADDR = 0b00001000,
	MCP23017_BIT4_ADDR = 0b00010000,
	MCP23017_BIT5_ADDR = 0b00100000,
	MCP23017_BIT6_ADDR = 0b01000000,
	MCP23017_BIT7_ADDR = 0b10000000,
} MCP23017_BITADDR;

typedef enum
{
		MCP23017_OUTPUT_HIGH = 1,
		MCP23017_OUTPUT_LOW = 0
}MCP23017_OUTPUT;

void MCP23017_Init(MCP23017_PORT port);
void MCP23017_Send(MCP23017_OUTPUT output, MCP23017_PORT port, MCP23017_BITADDR pin);
char MCP23017_ReadPin(MCP23017_PORT port, MCP23017_BITADDR pin);
void MCP23017_SetPin(MCP23017_PinMode mode, MCP23017_PORT port, MCP23017_BITADDR pin);