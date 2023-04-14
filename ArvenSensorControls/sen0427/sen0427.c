/*
 * sen0427.c
 *
 * Created: 2023-02-25
 * Author: Kia Skretteberg & Nubal Manhas
 */
#include <avr/io.h>
#include "i2c.h"
#include "sen0427.h"
#include "../mcp23017/mcp23017.h"

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/

// write out a 16 bit value as an 8 bit value to the device via I2C
void write8bit(SEN0427_Device device, uint16_t registerAddr, uint8_t value);

// read out an 8 bit value via I2C
unsigned char read8bit(SEN0427_Device device, uint16_t registerAddr);

// get the specified device's intended I2C address (not default)
uint8_t getDeviceAddr(SEN0427_Device device);

// assigns the new, proper address to the device, using the default address as the intended write device
void reAddressDevice(SEN0427_Device device);

/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

// Initializes all SEN0427 Devices
void SEN0427_InitAll(void)
{
    //Set pin to low
	PORTD &= ~SEN0427_L_EN;
	// initialize the right sensor	
    (void) SEN0427_InitDevice(SEN0427_R);
    // set the pin high to enable SEN0427_L
	PORTD |= SEN0427_L_EN;
    // initialize the left sensor
    (void) SEN0427_InitDevice(SEN0427_L);
}

// Initializes the specified device on I2C with proper address and settings
int SEN0427_InitDevice(SEN0427_Device device)
{
    unsigned char deviceExistsScanBuff[128];

    // check if a device exists at the specified address, error out if there's no device
    I2C_Scan(deviceExistsScanBuff);
    if(deviceExistsScanBuff[SEN0427_Addr])
    {
	    reAddressDevice(device);
    }
	else if(!deviceExistsScanBuff[getDeviceAddr(device)])
    {
        return -1;
    }

    // initialize the device
    if(read8bit(device, VL6180X_SYSTEM_FRESH_OUT_OF_RESET))
    {
        write8bit(device, 0x0207, 0x01);
        write8bit(device, 0x0208, 0x01);
        write8bit(device, 0x0096, 0x00);
        write8bit(device, 0x0097, 0xfd);
        write8bit(device, 0x00e3, 0x00);
        write8bit(device, 0x00e4, 0x04);
        write8bit(device, 0x00e5, 0x02);
        write8bit(device, 0x00e6, 0x01);
        write8bit(device, 0x00e7, 0x03);
        write8bit(device, 0x00f5, 0x02);
        write8bit(device, 0x00d9, 0x05);
        write8bit(device, 0x00db, 0xce);
        write8bit(device, 0x00dc, 0x03);
        write8bit(device, 0x00dd, 0xf8);
        write8bit(device, 0x009f, 0x00);
        write8bit(device, 0x00a3, 0x3c);
        write8bit(device, 0x00b7, 0x00);
        write8bit(device, 0x00bb, 0x3c);
        write8bit(device, 0x00b2, 0x09);
        write8bit(device, 0x00ca, 0x09);
        write8bit(device, 0x0198, 0x01);
        write8bit(device, 0x01b0, 0x17);
        write8bit(device, 0x01ad, 0x00);
        write8bit(device, 0x00ff, 0x05);
        write8bit(device, 0x0100, 0x05);
        write8bit(device, 0x0199, 0x05);
        write8bit(device, 0x01a6, 0x1b);
        write8bit(device, 0x01ac, 0x3e);
        write8bit(device, 0x01a7, 0x1f);
        write8bit(device, 0x0030, 0x00);
    }
    write8bit(device, VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30);
    write8bit(device, VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF);
    write8bit(device, VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01);
    write8bit(device, VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x09);
    write8bit(device, VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x00);
    write8bit(device, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x31);
    write8bit(device, 0x2A3, 0);
    write8bit(device, VL6180X_SYSTEM_MODE_GPIO1,0x20);
    write8bit(device, VL6180X_SYSTEM_FRESH_OUT_OF_RESET,0);           

    return 0;
}

SEN0427_RangeResult SEN0427_GetRangeResult(SEN0427_Device device)
{
    // TODO: Can this return something else? Could it be a problem?
    return (SEN0427_RangeResult) read8bit(device, VL6180X_RESULT_RANGE_STATUS)>>4;
}

unsigned char SEN0427_GetSingleMeasurement(SEN0427_Device device)
{
    write8bit(device, VL6180X_SYSRANGE_START, 0b01);
    return SEN0427_ReadRangeMeasurement(device);
}

void SEN0427_StartContinuousMeasurement(SEN0427_Device device)
{
    write8bit(device, VL6180X_SYSRANGE_START, 0b11);
}
void SEN0427_StopContinuousMeasurement(SEN0427_Device device)
{
    write8bit(device, VL6180X_SYSRANGE_START, 0b10);
}

unsigned char SEN0427_ReadRangeMeasurement(SEN0427_Device device)
{
    return read8bit(device, 0x062);
}

unsigned char SEN0427_CaptureDistance(SEN0427_Device device)
{
    unsigned char distance = 255; //default to 255, max distance, to indicate error

    switch(SEN0427_GetRangeResult(device))
    {
        case SEN0427_RangeResult__NO_ERR:
            distance = SEN0427_GetSingleMeasurement(device);
            break;
		default:
			// DO NOTHING FOR ERRORS
			break;
    }

    return distance;
}

/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/

void reAddressDevice(SEN0427_Device device)
{
	uint8_t deviceAddr = getDeviceAddr(device);
	I2C_Start(SEN0427_Addr, I2C_WRITE);
	// tell it which address to write
	I2C_Write8(0x212>>8, I2C_NOSTOP);
	I2C_Write8(0x212&0xFF, I2C_NOSTOP);
	// write the data, and we're done
	I2C_Write8(deviceAddr, I2C_STOP);
}

void write8bit(SEN0427_Device device, uint16_t registerAddr, uint8_t value)
{
    I2C_Start(getDeviceAddr(device), I2C_WRITE);
    // tell it which address to write
    I2C_Write8(registerAddr>>8, I2C_NOSTOP);
    I2C_Write8(registerAddr, I2C_NOSTOP);
    // write the data, and we're done
    I2C_Write8(value, I2C_STOP);
}

unsigned char read8bit(SEN0427_Device device, uint16_t registerAddr)
{
    unsigned char data;
    uint8_t deviceAddr = getDeviceAddr(device);
    // tell it which address we want to read
    I2C_Start(deviceAddr, I2C_WRITE);
    I2C_Write8(registerAddr>>8, I2C_NOSTOP);
    I2C_Write8(registerAddr&0xFF, I2C_NOSTOP);
    // switch to read
    I2C_Start(deviceAddr, I2C_READ);
    // grab the data, and we're done
    I2C_Read8(&data, I2C_NACK, I2C_STOP);
    return data;
}

uint8_t getDeviceAddr(SEN0427_Device device)
{
    uint8_t deviceAddr = 0;
    switch(device)
    {
        case SEN0427_L:
            deviceAddr = SEN0427_L_Addr;
            break;
        case SEN0427_R:
            deviceAddr = SEN0427_R_Addr;
            break;
		default:
			break;
	}
    return deviceAddr;
}