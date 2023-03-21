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
 #include "mcp23017.h"

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/


/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void Encoder36GP_InitAll(void)
{
}

void Encoder36GP_InitMotor(Encoder36GP_Motor motor)
{	
	if(!MSCP23017_Initialized(MCP23017_PORTA)) MCP23017_Init(MCP23017_PORTA);
	if(!MSCP23017_Initialized(MCP23017_PORTB)) MCP23017_Init(MCP23017_PORTB);

    // ensure the desired pins are all set to inputs
    switch(motor)
    {
        case Encoder36GP_FL:
            MCP23017_SetPin(); // pin 1?
            MCP23017_SetPin(); // pin 2?
            break;
        case Encoder36GP_FR:
            MCP23017_SetPin(); // pin 3?
            MCP23017_SetPin(); // pin 4?
            break;
    }
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