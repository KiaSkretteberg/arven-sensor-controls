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

void add_data_to_frame_buffer(char * buff, long data);
 

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/


/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void pico_init_communication(void)
{
    // 8 bits, 1 stop bit, no parity
	SCI0_Init(F_CPU, PCIO_BAUD_RATE, 0); // no interrupts for read (TODO: Should use interrupts)
}

void pico_send_data(struct PicoFrame frame)
{
    // Initialize frame buffer that will hold the bytes to be send
    char dataFrame[PICO_FRAME_LENGTH + 1] = PICO_START_BYTE;

    // add IR sensor data
    add_data_to_frame_buffer(frame.IR_L_Distance);
    add_data_to_frame_buffer(frame.IR_R_Distance);

    // add ultrasonic sensor data
    add_data_to_frame_buffer(frame.Ultrasonic_L_Duration);
    add_data_to_frame_buffer(frame.Ultrasonic_C_Duration);
    add_data_to_frame_buffer(frame.Ultrasonic_R_Duration);

    // add bump sensor data
    add_data_to_frame_buffer(frame.Bump_L + frame.Bump_R << 1);

    // add weight data
    add_data_to_frame_buffer(frame.Weight);

    // add battery data
    add_data_to_frame_buffer(frame.Battery_Low);

    // add motor direction data 
    add_data_to_frame_buffer(frame.Motor_FL_Direction << 5 + frame.Motor_FR_Direction << 4);

    // add motor speed data
    add_data_to_frame_buffer(frame.Motor_FL_Speed);
    add_data_to_frame_buffer(frame.Motor_FR_Speed);

    // add end frame byte
    dataFrame += "^";

    // send out the actual frame
	SCI0_TxString(dataFrame);
}

void pico_receive_data(void)
{
    char data;
    while(!SCI0_RxByte(&data))
    {
        //TODO: Do something?
    }
}

void add_data_to_frame_buffer(char * buff, long data)
{
    sprintf(*buff + strlen(*buff), "%X", data);
}