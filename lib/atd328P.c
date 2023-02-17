// A/D library, ATmega328P Version
// Simon Walker, NAIT

#include <avr/io.h>
#include <stdio.h>
#include "atd.h"

void AtoD_Init (AtoD_Channel chan)
{
  PRR &= ~(1 << PRADC); // turn on A/D module in power reduction register
  
  ADMUX = 0b00000000;   // Internal Vref off, right-aligned, ADC0 (28.9.1)  
  ADCSRB = 0b00000000;  // default, no comparator, free running mode    
  
  // use support function to set channel
  AtoD_SetChannel(chan);
  
  ADCSRA = 0b11100111;  // turn on AD, prescale 128, ADATE needs to be on? (28.9.2)
}

void AtoD_SetChannel (AtoD_Channel chan)
{
  switch (chan) 
  {
    case AtoD_Channel_0:
      DIDR0 |= 0b00000001;  // kill digital input for channel 0
      break;
    case AtoD_Channel_1:
      DIDR0 |= 0b00000010;  // kill digital input for channel 0
      break;
    case AtoD_Channel_2:
      DIDR0 |= 0b00000100;  // kill digital input for channel 0
      break;
    case AtoD_Channel_3:
      DIDR0 |= 0b00001000;  // kill digital input for channel 0
      break;
    case AtoD_Channel_4:
      DIDR0 |= 0b00010000;  // kill digital input for channel 0
      break;
    case AtoD_Channel_5:
      DIDR0 |= 0b00100000;  // kill digital input for channel 0
      break;
    case AtoD_Channel_6:
      DIDR0 |= 0b01000000;  // kill digital input for channel 0
      break;
    case AtoD_Channel_7:
      DIDR0 |= 0b10000000;  // kill digital input for channel 0
      break;
    default:               // other cases don't require digital inputs 
      DIDR0 = 0b00000000;  // all A/D channels as digital inputs
  }      
  
  // channel selection
  ADMUX &= 0b11110000;  // clear channel selection
  ADMUX |= chan;        // set back channel selection bits
}