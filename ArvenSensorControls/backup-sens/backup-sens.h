/*
 * backup_sens.h
 * Snap Action Switches
 * Utilizes GPIO
 *
 * Created: 2023-02-25 12:52:40 PM
 *  Author: Kia Skretteberg
 */ 
#define Back_Sens_L 0b00000100 // PORTD
#define Back_Sens_R 0b01001000 // PORTD

// Initialize all backup sensors (switches)
void Back_Sens_InitAll(void);

// Initialize the specified switch as inputs with interrupts enabled
void Back_Sens_Init(int sens);

// ISR for detecting if a backup sensor was triggered, if either triggered returns 1, else 0
int Back_Sens_ISR();