/*
 * gd03.c
 *
 * Created: 2023-02-21
 * Author : Kia Skretteberg
 */
#include <avr/io.h>
#include <stdio.h>
#include "atd.h"
#include "gd03.h"

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/

/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void GD03_Init(void)
{
	AtoD_Init(AtoD_Channel_0); // pin 23
}

int GD03_CaptureAtoDVal(void)
{
    unsigned char AD_low = ADCL; // must be read first
    unsigned char AD_high = ADCH;
    unsigned int atodval = (AD_low + AD_high * 256);
	return atodval;
}

/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/
