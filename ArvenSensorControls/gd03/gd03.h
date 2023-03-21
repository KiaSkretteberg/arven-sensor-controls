/*
 * gd03.h
 * GD03 Force Sensing Resistor (Weight Measurement)
 * Utilizes AtoD
 *
 * Created: 2023-02-21
 * Author : Kia Skretteberg
 */ 

// Initialize AtoD channel 0 (pin 23)
void GD03_Init(void);

// Capture the atdoval from the channel
int GD03_CaptureAtoDVal(void);