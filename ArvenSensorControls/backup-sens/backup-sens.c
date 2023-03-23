/*
 * backup_sens.c
 *
 * Created: 2023-02-25
 * Author: Kia Skretteberg
 */ 
#include <avr/io.h>
#include "backup-sens.h"

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/


/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void Back_Sens_InitAll(void)
{
	Back_Sens_InitSens(Back_Sens_L);
	Back_Sens_InitSens(Back_Sens_R);
}

void Back_Sens_InitSens(int sens)
{
	DDRD &= ~sens; //input
	
	PCMSK2 |= sens; // turn on PCINT18/19 pin mask (enable interrupts) (12.2.6)
	PCICR |= 0b00000100; // turn on interrupts for group 2 (12.2.4)
}

char Back_Sens_ISR(int pin)
{
	// check if this sensor was hit
	if(PIND & pin)
	{
		return 1;
	}
	return 0;
}

/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/