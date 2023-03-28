/*
* pico.c
* Communication with the Raspberry Pi Pico W via SCI0
* Utilizes SCI
*
* Created: 2023-03-20
* Author: Kia Skretteberg

Utilizing Frames of the following structure:
$FFFFF1FFFF1FFFF1FFFFA3FF1FFFFF^

The above is broken up into 16 segments varying in the number of (string) bytes that represent them.

Segment 1: (1 byte)
Indication of which of the 8 sensors have changed.
The values that are indicated as being modified are the next 7 segments (segment 9 is excluded since it is just a battery indicator)

* ---------------------------------------------------------------------------------------------
* |    b7    |    b6    |     b5    |    b4    |     b3     |   b2    |    b1    |     b0     |
* ---------------------------------------------------------------------------------------------
* |  IR Left | IR Right |  US Left  |  US Ctr  |  US Right  |  Bumps  |  Weight  |  Encoders  |
* ---------------------------------------------------------------------------------------------

Segment 2: (2 bytes)
Left IR Sensor
Values 00-FF, representing a number of mm in hex

Segment 3: (2 bytes)
Right IR Sensor
Values 00-FF, representing a number of mm in hex

Segment 4: (5 bytes)
Left Ultrasonic Sensor
Values 00000-1FFFF
A measure of counts (in 0.5us?) that it took the echo to send and receive (needs to be / 2)

Segment 5: (5 bytes)
Center Ultrasonic Sensor
Values 00000-1FFFF
A measure of counts (in 0.5us?) that it took the echo to send and receive (needs to be / 2)

Segment 6: (5 bytes)
Right Ultrasonic Sensor
Values 00000-1FFFF
A measure of counts (in 0.5us?) that it took the echo to send and receive (needs to be / 2)

Segment 7: (1 byte)
Left and Right Bump Sensor
A hex value representing 2 bits with b0 being the right sensor, and b1 being the left sensor.
1 means there is an obstacle. 0 means there is no obstacle. b1 = Left, b2 = right

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
The values are chosen as the bit values represent the last 2 bits of the hex values (A = 1010 ==> 10, B = 1011 ==> 11, etc)

Segment 8: (3 bytes)
Weight (force sensing resistor)
A raw AtoD value, from a 10 bit ADC, representing the voltage measured between 0 and 5V, where 5V = 10N (max force)

Segment 9: (1 or 3 bytes)
Battery level (if 1 byte, is 0/1; if 3 bytes, is an AtoD value similar to segment 8)

Segment 10: (1 byte)
Direction of Motors (from encoders)
Each motor is represented by a single bit of 0 or 1 indicated below,
where 1 indicates forward, 0 is backwards

* ----------------------------------------------------------------------------------------
* |   b7  |  b6   |   b5    |    b4     |     b3     |     b2     |    b1    |     b0    |
* ----------------------------------------------------------------------------------------
* |     unused    | Front L |  Front R  |  Middle L  |  Middle R  |  Back L  |  Back  R  |
* ----------------------------------------------------------------------------------------

Segment 11: (2 bytes)
Speed of Front Left Motor (from encoders)
Measured in RPMs, max possible value is 255, though it should never be above 170

Segment 12: (2 byte)
Speed of Front Right Motor (from encoders)
Measured in RPMs, max possible value is 255, though it should never be above 170

Segment 13: (2 byte) -- not in use
Speed of Middle Left Motor (from encoders)
Measured in RPMs, max possible value is 255, though it should never be above 170

Segment 14: (2 byte) -- not in use
Speed of Middle Right Motor (from encoders)
Measured in RPMs, max possible value is 255, though it should never be above 170

Segment 15: (2 byte) -- not in use
Speed of Back Left Motor (from encoders)
Measured in RPMs, max possible value is 255, though it should never be above 170

Segment 16: (2 byte) -- not in use
Speed of Back Left Motor (from encoders)
Measured in RPMs, max possible value is 255, though it should never be above 170
*/

#define PICO_FRAME_LENGTH      31  // not inclusive of start/end bytes
#define PICO_START_BYTE		   '$' // indicator of a start frame
#define PICO_END_BYTE          '^' // indicator of an end frame

#define PICO_BAUD_RATE 56000

struct PicoFrame {
    unsigned char IR_L_Distance;         //measured in mm
    unsigned char IR_R_Distance;         //measured in mm

    long Ultrasonic_L_Duration; //measured in us
    long Ultrasonic_C_Duration; //measured in us
    long Ultrasonic_R_Duration; //measured in us

    char Bump_L;                // 1 if there's an object
    char Bump_R;                // 1 if there's an object

    int  Weight;                // AD value measured (5V ref)

    char Battery_Low;           // 1 if battery low

    char Motor_FL_Direction;    // 1 if forward
    unsigned char Motor_FL_Speed;        // measured in RPM

    char Motor_FR_Direction;    // 1 if forward
    unsigned char Motor_FR_Speed;        // measured in RPM

    // char Motor_ML_Direction;    // 1 if forward
    // unsigned char Motor_ML_Speed;        // measured in RPM

    // char Motor_MR_Direction;    // 1 if forward
    // unsigned char Motor_MR_Speed;        // measured in RPM

    // char Motor_BL_Direction;    // 1 if forward
    // unsigned char Motor_BL_Speed;        // measured in RPM

    // char Motor_BR_Direction;    // 1 if forward
    // unsigned char Motor_BR_Speed;        // measured in RPM
};


// initialize the pico to run on UART
void Pico_InitCommunication(void);
// Function to run to receive data. Not currently used
void Pico_ReceiveData(void);
// Send a request to the pico via uart
void Pico_SendData(struct PicoFrame frame);
