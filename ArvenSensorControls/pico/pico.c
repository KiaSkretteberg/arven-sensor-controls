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

void addDataToFrameBuffer(char * buff, long data, char * format);
 

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/


/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void Pico_InitCommunication(void)
{	
	// no interrupts for read (TODO: Should use interrupts)
	if(SCI0_Init(F_CPU, PICO_BAUD_RATE, 0)){
		PORTC |= 0b00000100;
	}
    // 8 bits, 1 stop bit, no parity
	 
}

void Pico_SendData(struct PicoFrame frame)
{
	SCI0_TxString("Start of Pico_SendData\n");
    // Initialize frame buffer that will hold the bytes to be send
    char dataFrame[PICO_FRAME_LENGTH + 1];
	dataFrame[0] = PICO_START_BYTE;
    // add IR sensor data
    SCI0_TxString("addDataToFrameBuffer: frame.IR_L_Distance\n");
	SCI0_TxString(dataFrame);
	addDataToFrameBuffer(dataFrame, frame.IR_L_Distance, "%02X");
	SCI0_TxString("\naddDataToFrameBuffer: frame.IR_R_Distance\n");
	SCI0_TxString(dataFrame);
    addDataToFrameBuffer(dataFrame, frame.IR_R_Distance, "%02X");
	
    // add ultrasonic sensor data
	SCI0_TxString("\naddDataToFrameBuffer: frame.Ultrasonic_L_Duration\n");
	SCI0_TxString(dataFrame);
    addDataToFrameBuffer(dataFrame, frame.Ultrasonic_L_Duration, "%05X");
	SCI0_TxString("\naddDataToFrameBuffer: frame.Ultrasonic_C_Duration\n");
	SCI0_TxString(dataFrame);
    addDataToFrameBuffer(dataFrame, frame.Ultrasonic_C_Duration, "%05X");
	SCI0_TxString("\naddDataToFrameBuffer: frame.IR_R_Duration\n");
	SCI0_TxString(dataFrame);
    addDataToFrameBuffer(dataFrame, frame.Ultrasonic_R_Duration, "%05X");
	SCI0_TxString("\naddDataToFrameBuffer: frame.Bump_L + frame.Bump_R << 1\n");
    SCI0_TxString(dataFrame);
	// add bump sensor data
    addDataToFrameBuffer(dataFrame, frame.Bump_L + frame.Bump_R << 1, "%X");
	SCI0_TxString("\naddDataToFrameBuffer: frame.Weight\n");
	SCI0_TxString(dataFrame);
    // add weight data
    addDataToFrameBuffer(dataFrame, frame.Weight, "%03X");
	SCI0_TxString("\naddDataToFrameBuffer: frame.Battery_Low\n");
	SCI0_TxString(dataFrame);
    // add battery data
    addDataToFrameBuffer(dataFrame, frame.Battery_Low, "%X");
	SCI0_TxString("\naddDataToFrameBuffer: frame.Motor_FL_Direction << 5 + frame.Motor_FR_Direction << 4\n");
    SCI0_TxString(dataFrame);
	SCI0_TxString("\n");
	// add motor direction data 
    addDataToFrameBuffer(dataFrame, frame.Motor_FL_Direction << 5 + frame.Motor_FR_Direction << 4, "%X");
	
    // add motor speed data
    addDataToFrameBuffer(dataFrame, frame.Motor_FL_Speed, "%02X");
    addDataToFrameBuffer(dataFrame, frame.Motor_FR_Speed, "%02X");

    // add end frame byte
	dataFrame[PICO_FRAME_LENGTH - 1] = PICO_END_BYTE;

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

void addDataToFrameBuffer(char * buff, long data, char *format)
{
    sprintf(buff + strlen(buff), format, data);
}