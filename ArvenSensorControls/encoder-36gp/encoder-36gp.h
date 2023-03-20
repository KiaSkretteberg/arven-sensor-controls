/*
 * 36gp_555_27_en.h
 * 36GP-555-27-EN Encoders for Motors
 * Utilizes I2C
 *
 * Created: 2023-02-25 12:57:45 PM
 *  Author: Kia Skretteberg
 */
typedef enum
{
	Encoder36GP_FL = 0, // front left wheel
	Encoder36GP_FR = 1  // front right wheel
	
	// Encoder36GP_ML = 0, // middle left wheel
	// Encoder36GP_MR = 1,  // middle right wheel
	
	// Encoder36GP_BL = 0, // back left wheel
	// Encoder36GP_BR = 1  // back right wheel
} Encoder36GP_Motor;

// Initialize all Encoders for all 36GP-555-27-EN
void Encoder36GP_InitAll(void);

// Initialize encoders for specific motor
void Encoder36GP_InitMotor(Encoder36GP_Motor motor);


