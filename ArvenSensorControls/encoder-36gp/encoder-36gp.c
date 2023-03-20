/*
 * 36gp_555_27_en.c
 * 36GP-555-27-EN Encoders for Motors
 * Utilizes I2C
 *
 * Created: 2023-02-25 12:57:54 PM
 *  Author: Kia Skretteberg
 */
 #include <avr/io.h>
 #include "i2c.h"
 #include "encoder-36gp.h"
 #include "MCP23017.h"

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


/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/