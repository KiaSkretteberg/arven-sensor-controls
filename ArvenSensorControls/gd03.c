/*
 * gd03.c
 *
 * Created: 2023-02-21 5:12 PM
 * Author : Kia Skretteberg
 */
#include <avr/io.h>
#include <stdio.h>
#include "atd.h"
#include "sci.h"
#include "gd03.h"

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/

// measure the current voltage coming in on AtoD channel 0
float measureVoltage(void);

/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void GD03_Init(void)
{
	AtoD_Init(AtoD_Channel_0); // pin 23
}

float GD03_DetermineWeight(void)
{
	float voltage = measureVoltage();
	// 1000.0 is the conversion factor for kg to g
	// 9.81 is gravity
	// g = N * (g/kg) * m/s^2
	float max_weight = (GD03_MAXN * 1000.0 / 9.81);
	
	float weight = (voltage / AREF) * max_weight; //TODO: Add fulcrum gain
	
	return weight;
}

GD03_LoadState GD03_CheckForLoad(void)
{
	float weight = GD03_DetermineWeight();
	
	return weight > GD03_MINV ? GD03_LoadPresent : GD03_LoadNotPresent;
}

GD03_WeightChange GD03_CheckForWeightChange(float newWeight, float oldWeight, float doseWeight)
{
	float weightDifference = oldWeight - newWeight;
	GD03_WeightChange change;
	
	// The weight went up?? Track as no change because it's confusing
	if(weightDifference < 0)
		change = GD03_NoChange;
	// The difference is less than or equal to a dose (plus 10% for error)
	else if(weightDifference <= (doseWeight * 1.1))
		change = GD03_SmallChange;
	// Larger than a dose means something potentially bad happened
	else
		change = GD03_LargeChange;
	
	return change;
}

/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/

float measureVoltage(void)
{
    unsigned char AD_low = ADCL; // must be read first
    unsigned char AD_high = ADCH;
    unsigned int atodval = (AD_low + AD_high * 256);
    char buff[20];
    //sprintf(buff,"\nATOD: %i", atodval);
    //SCI0_TxString(buff);
    float q = AREF / 1024.0; // 10 bit atod [1024]
    //sprintf(buff," Q: %f", q);
    //SCI0_TxString(buff);
    float voltage = atodval * q; // min val (resting) 0.73V
    //sprintf(buff," VOLT: %f\n", voltage);
    //SCI0_TxString(buff);	
	return voltage;
}