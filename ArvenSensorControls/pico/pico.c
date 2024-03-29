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

/*
* Function to read the bump_L and bump_R
* frame values, and translating them into
* the corresponding char as below:
*
* ------------------------------------------
* |    Hex Value    |    b1    |     b0    |
* ------------------------------------------
* |        A        |    1     |     0     |
* ------------------------------------------
* |        B        |    1     |     1     |
* ------------------------------------------
* |        C        |    0     |     0     |
* ------------------------------------------
* |        D        |    0     |     1     |
* ------------------------------------------
*/
char parseBumpVal(char bump_L,char bump_R);

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
	// Temporary buffer for holding a single value to be added to the frame
	char buff[6];
    // Initialize frame buffer that will hold the bytes to be send
    char dataFrame[PICO_FRAME_LENGTH + 3];
	// ensure the frame is empty
	strcpy(dataFrame, "");
	// Add the start byte
	sprintf(buff, "%c", PICO_START_BYTE);
	strcat(dataFrame, buff);	
	// Add the byte indicating what data has changes (TODO: Figure out how to set this)
	sprintf(buff, "%02X", 0b00100100);
	strcat(dataFrame, buff);	
    // add IR sensor data
	sprintf(buff, "%02X", frame.IR_L_Distance);
	strcat(dataFrame, buff);
	sprintf(buff, "%02X", frame.IR_R_Distance);
	strcat(dataFrame, buff);
    // add ultrasonic sensor data
	sprintf(buff, "%05lX", frame.Ultrasonic_L_Duration);
	strcat(dataFrame, buff);
	sprintf(buff, "%05lX", frame.Ultrasonic_C_Duration);
	strcat(dataFrame, buff);
	sprintf(buff, "%05lX", frame.Ultrasonic_R_Duration);
	strcat(dataFrame, buff);
	// add bump sensor data
	sprintf(buff, "%c", parseBumpVal(frame.Bump_L, frame.Bump_R));
	strcat(dataFrame, buff);
    // add weight data
	sprintf(buff, "%03X", frame.Weight);
	strcat(dataFrame, buff);
    // add battery data
	sprintf(buff, "%c", frame.Battery_Low ? 1 : 0);
	strcat(dataFrame, buff);
	// add motor direction data 
	sprintf(buff, "%02X", (frame.Motor_FL_Direction << 5) + (frame.Motor_FR_Direction << 4));
	strcat(dataFrame, buff);
    // add motor speed data
	sprintf(buff, "%02X", frame.Motor_FL_Speed);
	strcat(dataFrame, buff);
	sprintf(buff, "%02X", frame.Motor_FR_Speed);
	strcat(dataFrame, buff);
    // add end frame byte
	sprintf(buff, "%c", PICO_END_BYTE);
	strcat(dataFrame, buff);	
    // send out the actual frame
	SCI0_TxString(dataFrame);
	// send a new line for easier readability, the pico will ignore it
	SCI0_TxString("\n");
}

void Pico_ReceiveData(void)
{
    unsigned char data;
    while(!SCI0_RxByte(&data))
    {
        //TODO: Do something?
    }
}

char parseBumpVal(char bump_L,char bump_R)
{
	if(bump_L && !bump_R)
	{
		return 'A';
	}
	else if(bump_L && bump_R)
	{
		return 'B';
	}
	else if(!bump_L && !bump_R)
	{
		return 'C';
	}
	else //!bump_L, bumpR -- only option left
	{
		return 'D';
	}
}