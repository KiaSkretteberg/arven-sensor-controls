/*
* mcp23017.h
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
* Since each port has 8 different I/O pins, you must
* set the bit corresponding to the pin you want to high/low
* depending on if you want an output/input, or if
* you want to set an output pin as high/low. This will
* be used to set the position of the bit you want to address 
* within the desired port.
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

/*
* An output is considered when a specified bit
* for the specified port is set to 1 (high).
* A 0 is considered as low.
*/
typedef enum
{
		MCP23017_OUTPUT_HIGH = 1,
		MCP23017_OUTPUT_LOW = 0
}MCP23017_OUTPUT;

/*
* Initialization routine
*/
void MCP23017_Init(MCP23017_PORT port);

/// @brief Returns 1 if the specified port has been initialized already
/// @param port 
/// @return 1 or 0
bool MSCP23017_Initialized(MCP23017_PORT port);

/*
* Function to send out an output (HIGH/LOW) to a desired output bit
*
* output = MCP23017_HIGH or MCP23017_LOW
* port = MCP23017_PORTA or MCP23017_PORTB
* pin = bit within PORTA/PORTB to set (0x01-0x08 in binary)
*/
void MCP23017_Send(MCP23017_OUTPUT output, MCP23017_PORT port, MCP23017_BITADDR pin);

/*
* Function to read a desired bit from the specified port.
* Input and Outputs can be read.
*
* port = MCP23017_PORTA or MCP23017_PORTB
* pin = bit within PORTA/PORTB to set (0x01-0x08 in binary)
*/
char MCP23017_ReadPin(MCP23017_PORT port, MCP23017_BITADDR pin);

/*
* Function to set the pin mode (input or output) of a desired bit
* 
* port = MCP23017_PORTA or MCP23017_PORTB
* mode = MCP23017_INPUT or MCP23017_OUTPUT
* pin = bit within PORTA/PORTB to set (0x01-0x08 in binary)
*/
void MCP23017_SetPin(MCP23017_PinMode mode, MCP23017_PORT port, MCP23017_BITADDR pin);