// header for NXP version of LM75A (11-bit A/D, not 9)
// Simon Walker, NAIT
// Revision History:
// March 22 2022 - Initial Build

// read 16-bit value from device
// data for NXP version is left-aligned, 11 bits
// upper 8 bits are signed whole degrees C
// lower 3 bits are fractions of a degree (x 0.125)
int LM75A_ReadTemp (unsigned int * uiTemp);

// use read temp to convert to an actual temperature
//  returns -300 if I2C error
float LM75A_GetTempF ();