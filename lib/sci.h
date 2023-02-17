// SCI library, ATmega328P Version
// Simon Walker, NAIT
// Revision History:
// March 18 2022 - Initial Build

// what the ISR should look like (copy to implementation)
/*
ISR (USART_RX_vect)
{
  // indicate byte has been received (and can detect overrun too)
  ++_bRXFlag;
  
  // pull the byte (clears the interrupt too)
  _RXByte = UDR0;
}
*/

// initialize UCSR0 for asynchronous use, 8N1, at specified BAUD rate
int SCI0_Init (unsigned long ulBus, unsigned long ulBAUD, int bRXInt);

// blocking send of a byte
void SCI0_BSend (unsigned char data);

// blocking send of a string
void SCI0_TxString (char * buff);

// blocking send of a 16-bit HEX value as 0x0000
void SCI0_Tx16H (unsigned int uiVal, int tl);

// zero on byte rxed, otherwise no byte to read
int SCI0_RxByte (unsigned char * pData);
