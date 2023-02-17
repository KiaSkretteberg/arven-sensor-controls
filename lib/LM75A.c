// implementation for NXP version of LM75A,118 (11-bit A/D, not 9)
// Simon Walker, NAIT

#include <avr/io.h>
#include "I2C.h"
#include "LM75A.h"

int LM75A_ReadTemp (unsigned int * uiTemp)
{
	unsigned char ucDataH, ucDataL;

	// start transaction with device address
	if (I2C_Start(0x48, I2C_WRITE))
	  return -1;
	
	// want to read temperature (register zero)
	if (I2C_Write8(0, I2C_NOSTOP))
	  return -2;
	
  // issue restart to change to read
	if (I2C_Start(0x48, I2C_READ))
	  return -3;

  // read temperature data byte high, nostop, ack more data please
	if (I2C_Read8(&ucDataH, I2C_ACK, I2C_NOSTOP))
	  return -4;

  // read temperature data byte low, stop, nack no more data please
	if (I2C_Read8(&ucDataL, I2C_NACK, I2C_STOP))
	  return -5;

  // form up the raw data into a 16-bit raw value for return
	*uiTemp = ((unsigned int)ucDataH << 8) + ucDataL;
	
	return 0;
}

float LM75A_GetTempF ()
{
	unsigned int uiTempRaw;

	if (LM75A_ReadTemp(&uiTempRaw))
	{
		return -300;
	}

	// convert to a float (hard way, from unsigned)
	float fdec = 0;
	if (uiTempRaw & 0x8000)
		fdec = -0.125f * (((~uiTempRaw) >> 5) + 1);
	else
		fdec = 0.125f * (uiTempRaw >> 5);
	return fdec;
}


