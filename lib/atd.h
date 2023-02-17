// A/D library, ATmega328 Version
// Simon Walker, NAIT
// Revision History:
// May 5 2022 - Initial Build

typedef enum AtoD_Channel
{
  AtoD_Channel_0 = 0,
  AtoD_Channel_1,
  AtoD_Channel_2,
  AtoD_Channel_3,
  AtoD_Channel_4,
  AtoD_Channel_5,
  AtoD_Channel_6,
  AtoD_Channel_7,    
  AtoD_TempSensor,
  AtoD_1V1       = 14,
  AtoD_0V
} AtoD_Channel;

void AtoD_Init (AtoD_Channel chan);

void AtoD_SetChannel (AtoD_Channel chan);