/*
 * gd03.h
 *
 * Created: 2023-02-21 5:12 PM
 * Author : Kia Skretteberg
 */ 

#define AREF 5.0
#define GD03_MAXN 10 // retrieved from https://www.uneotech.com/uploads/product_download/tw/GD03-10N%20ENG.pdf
#define GD03_MINV 0.73 // Experimentally determined (voltage measured when no load) TODO: determine this value for real

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

void GD03_Init(void);

float GD03_DetermineWeight(void);

GD03_LoadState GD03_CheckForLoad(void);

GD03_WeightChange GD03_CheckForWeightChange(float newWeight, float oldWeight, float doseWeight);