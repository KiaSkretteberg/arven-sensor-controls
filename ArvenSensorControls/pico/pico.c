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
#include <string.h>

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/
 

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
	char buff[6];
	//SCI0_TxString("Start of Pico_SendData\n");
    // Initialize frame buffer that will hold the bytes to be send
    char dataFrame[PICO_FRAME_LENGTH + 3];
	sprintf(buff, "%c", PICO_START_BYTE);
	strcat(dataFrame, buff);	
	sprintf(buff, "%02X", 0b00100100);
	strcat(dataFrame, buff);	
	//SCI0_TxString("Start Byte:\n");
	//SCI0_TxString(dataFrame);
    // add IR sensor data
    //SCI0_TxString("addDataToFrameBuffer: frame.IR_L_Distance\n");
	sprintf(buff, "%02X", frame.IR_L_Distance);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\naddDataToFrameBuffer: frame.IR_R_Distance\n");
	sprintf(buff, "%02X", frame.IR_R_Distance);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
    // add ultrasonic sensor data
	//SCI0_TxString("\naddDataToFrameBuffer: frame.Ultrasonic_L_Duration\n");
	sprintf(buff, "%05X", frame.Ultrasonic_L_Duration);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\naddDataToFrameBuffer: frame.Ultrasonic_C_Duration\n");
	sprintf(buff, "%05X", frame.Ultrasonic_C_Duration);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\naddDataToFrameBuffer: frame.Ultrasonic_R_Duration\n");
	sprintf(buff, "%05X", frame.Ultrasonic_R_Duration);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\naddDataToFrameBuffer: frame.Bump_L + frame.Bump_R << 1\n");
	// add bump sensor data
	sprintf(buff, "%X", frame.Bump_L + frame.Bump_R << 1);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\naddDataToFrameBuffer: frame.Weight\n");
    // add weight data
	sprintf(buff, "%03X", frame.Weight);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\naddDataToFrameBuffer: frame.Battery_Low\n");
    // add battery data
	sprintf(buff, "%X", frame.Battery_Low);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\naddDataToFrameBuffer: frame.Motor_FL_Direction << 5 + frame.Motor_FR_Direction << 4\n");
	//SCI0_TxString("\n");
	// add motor direction data 
	sprintf(buff, "%X", frame.Motor_FL_Direction << 5 + frame.Motor_FR_Direction << 4);
	strcat(dataFrame, buff);
	//SCI0_TxString(dataFrame);
	//SCI0_TxString("\n");
	
    // add motor speed data
	sprintf(buff, "%02X", frame.Motor_FL_Speed);
	strcat(dataFrame, buff);
	sprintf(buff, "%02X", frame.Motor_FR_Speed);
	strcat(dataFrame, buff);

    // add end frame byte
	sprintf(buff, "%c", PICO_END_BYTE);
	strcat(dataFrame, buff);
	SCI0_TxString("\n");
    // send out the actual frame
	SCI0_TxString(dataFrame);
}

void Pico_ReceiveData(void)
{
    unsigned char data;
    while(!SCI0_RxByte(&data))
    {
        //TODO: Do something?
    }
}