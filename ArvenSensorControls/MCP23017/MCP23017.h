/*
 * MCP23017.h
 * MCP23017 Port Expander
 * Allows for up to 16 GPI/O pins to be utilized
 * via I2C. These are addressed via 2 different 
 * ports : PORTA & PORTB
 *
 * Created: 2023-03-10
 *  Author: Nubal Manhas
 */

//I2C address for MCP23017
//our I2C library adjusts it
//for 7 bit addressing, so no
//need to account for that
#define MCP23017_Addr 0x20

//
typedef enum
{
	INPUT = 1,
	OUTPUT = 0
} PinMode;
typedef enum
{
	MCP23017_PORTA = 0x00,
	MCP23017_PORTB = 0x01
} MCP23017_PORT;
typedef enum
{
	PIN0_ADDR = 0b00000001,
	PIN1_ADDR = 0b00000010,
	PIN2_ADDR = 0b00000100,
	PIN3_ADDR = 0b00001000,
	PIN4_ADDR = 0b00010000,
	PIN5_ADDR = 0b00100000,
	PIN6_ADDR = 0b01000000,
	PIN7_ADDR = 0b10000000,
} MCP23017_PINADDR;
typedef enum
{
		HIGH = 1,
		LOW = 0
}MCP23017_OUTPUT;
typedef enum
{
	PIN0_NUM = 0,
	PIN1_NUM = 1,
	PIN2_NUM = 2,
	PIN3_NUM = 3,
	PIN4_NUM = 4,
	PIN5_NUM = 5,
	PIN6_NUM = 6,
	PIN7_NUM = 7,
		
}MCP23017_PINNUM;
void MCP23017_Init(MCP23017_PORT port);
void MCP23017_Send(MCP23017_OUTPUT output, MCP23017_PORT port, MCP23017_PINADDR pin);
char MCP23017_ReadPin(MCP23017_PORT port, MCP23017_PINADDR pin);
void MCP23017_SetPin(PinMode mode, MCP23017_PORT port, MCP23017_PINADDR pin);