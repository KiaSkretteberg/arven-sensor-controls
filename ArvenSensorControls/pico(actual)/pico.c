/*
 * pico.c
 *
 * Created: 2023-03-20
 * Author: Kia Skretteberg
 */

#define F_CPU 16E6 // with external xtal enabled, and clock div/8, bus == 2MHz
#include <avr/io.h>
#include <stdio.h>
#include "sci.h"
#include "pico.h"

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/

void addDataToFrameBuffer(char * buff, long data);
 

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/


/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void Pico_InitCommunication(void)
{
    // 8 bits, 1 stop bit, no parity
	SCI0_Init(F_CPU, PCIO_BAUD_RATE, 0); // no interrupts for read (TODO: Should use interrupts)
}

void Pico_SendData(struct PicoFrame frame)
{
    // Initialize frame buffer that will hold the bytes to be send
    char dataFrame[PICO_FRAME_LENGTH + 1] = PICO_START_BYTE;

    // add IR sensor data
    addDataToFrameBuffer(frame.IR_L_Distance);
    addDataToFrameBuffer(frame.IR_R_Distance);

    // add ultrasonic sensor data
    addDataToFrameBuffer(frame.Ultrasonic_L_Duration);
    addDataToFrameBuffer(frame.Ultrasonic_C_Duration);
    addDataToFrameBuffer(frame.Ultrasonic_R_Duration);

    // add bump sensor data
    addDataToFrameBuffer(frame.Bump_L + frame.Bump_R << 1);

    // add weight data
    addDataToFrameBuffer(frame.Weight);

    // add battery data
    addDataToFrameBuffer(frame.Battery_Low);

    // add motor direction data 
    addDataToFrameBuffer(frame.Motor_FL_Direction << 5 + frame.Motor_FR_Direction << 4);

    // add motor speed data
    addDataToFrameBuffer(frame.Motor_FL_Speed);
    addDataToFrameBuffer(frame.Motor_FR_Speed);

    // add end frame byte
    dataFrame += "^";

    // send out the actual frame
	SCI0_TxString(dataFrame);
}

void Pico_ReceiveData(void)
{
    char data;
    while(!SCI0_RxByte(&data))
    {
        //TODO: Do something?
    }
}

void addDataToFrameBuffer(char * buff, long data)
{
    sprintf(*buff + strlen(*buff), "%X", data);
}