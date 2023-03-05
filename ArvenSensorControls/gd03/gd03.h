/*
 * gd03.h
 * GD03 Force Sensing Resistor (Weight Measurement)
 * Utilizes AtoD
 *
 * Created: 2023-02-21 5:12 PM
 * Author : Kia Skretteberg
 */ 

#define AREF 5.0
#define GD03_MAXN 10 // retrieved from https://www.uneotech.com/uploads/product_download/tw/GD03-10N%20ENG.pdf
#define GD03_MINV 0.01 // Experimentally determined (voltage measured when no load) TODO: determine this value for real

typedef enum
{
	GD03_NoChange,
	GD03_SmallChange,
	GD03_LargeChange
} GD03_WeightChange;

typedef enum
{
	GD03_LoadPresent,
	GD03_LoadNotPresent
} GD03_LoadState;

// Initialize AtoD channel 0 (pin 23)
void GD03_Init(void);

// Determine the weight (in grams) that is being measured by the GD03
float GD03_DetermineWeight(void);

// Check if a load is currently being measured by the GD03
GD03_LoadState GD03_CheckForLoad(void);

// Check if the weight being measured by the GD03 changed compared to previous weight
// and how it changed in relation to the weight of a dose (was it roughly a dose [smallchange], or much more [largechange])
GD03_WeightChange GD03_CheckForWeightChange(float oldWeight, float doseWeight);