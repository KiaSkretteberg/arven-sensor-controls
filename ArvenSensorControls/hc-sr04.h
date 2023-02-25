/*
 * hc_sr04.h
 * HC-SR04 Ultrasonic Sensor(s)
 * Utilizes GPIO
 * 
 * Created: 2023-02-24 1:36:40 PM
 *  Author: Kia Skretteberg
 * Operating characteristics retrieved from https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
 */ 

#define HCSR04_L_Trig PORTD5
#define HCSR04_L_Echo PORTD6
#define HCSR04_C_Trig PORTD7
#define HCSR04_C_Echo PORTB0
#define HCSR04_R_Trig PORTB1
#define HCSR04_R_Echo PORTB2

typedef enum
{
	HCSR04_L = 0, // left HC-SR04 sensor	-- PD5/PD6
	HCSR04_C = 1, // center HC-SR04 sensor	-- PD7/PB0
	HCSR04_R = 2 // right HC-SR04 sensor	-- PB1/PB2
	//HCSR04_All = 3
} HCSR04_Device;

// Initialize all HCSR04 devices
void HCSR04_InitAll(void);

// Initialize the specified device with appropriate Trig as output and appropriate Echo as input
void HCSR04_InitDevice(HCSR04_Device device);

// Determine if there's an obstacle within a specified distance, of the specified device, 
// 0 = no obstacle, 1 = obstacle
int HCSR04_CheckForObstacle(HCSR04_Device device, float distance);

// Get the current distance from the specified device, in cm, for a potential obstacle
// if distance is greater than 400 (4m), no obstacle could be detected
float HCSR04_GetEchoDistance(HCSR04_Device device);
