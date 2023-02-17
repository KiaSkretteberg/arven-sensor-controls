// SCI library, ATmega328P Version
// Simon Walker, NAIT

#include <avr/io.h>
#include <stdio.h>
#include "sci.h"

int SCI0_Init (unsigned long ulBus, unsigned long ulBAUD, int bRXInt)
{
  // determine the BAUD rate divisor required
  unsigned int iReg = (unsigned int)((((ulBus * 10 / 16) / ulBAUD) + 5) / 10) - 1;

  // won't fit in register
  if (iReg > 0b111111111111u)
    return -1;

  // start code will power off all modules...
  // ensure power is on : USART
  PRR &= 0b11111101;

  // set BAUD rate
  UBRR0H = iReg >> 8;
  UBRR0L = (unsigned char)iReg;

  // enable TX and RX
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  
  // async, no parity, 1 stop bit, 8 bit
  UCSR0C =  0b00000110;
  
  // optional interrupt on receiver
  if (bRXInt)
    UCSR0B |= (1 << RXCIE0);

  // no error
  return 0;
}

void SCI0_BSend (unsigned char data)
{
  // transmit buffer empty?
  while (!(UCSR0A & (1<<UDRE0)))
  ;

  // transmit complete must be cleared before send in polling mode
  if (UCSR0A & (1<<TXC0))
    UCSR0A = (1<<TXC0);

  // free to send data
  UDR0 = data;
}

void SCI0_Tx16H (unsigned int uiVal, int tl)
{
  char buff[7] = {0};
  (void)sprintf(buff, "0x%4.4X", uiVal);
  SCI0_TxString (buff);

  if (tl)
    SCI0_TxString ("\r\n");
}

void SCI0_TxString (char * buff)
{
  while (*buff)
  {
    SCI0_BSend(*buff);
    ++buff;
  }
}

int SCI0_RxByte (unsigned char * pData)
{
  if ( (UCSR0A & (1<<RXC0)) )
  {
    *pData = UDR0;
    return 0;
  }
  
  return 1;
}
