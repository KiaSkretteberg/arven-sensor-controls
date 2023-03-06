/*
 * sen0427.h
 * VL6180X IR Sensor(s) Module
 * Utilizes I2C
 *
 * Created: 2023-02-25 12:54:34 PM
 *  Author: 
 */ 
  // default for SEN0427 is 0x29 but can't both live on save addr so readdr'd
#define SEN0427_Addr	0x29 // default, will need to change in initialize
#define SEN0427_L_Addr	0x30
#define SEN0427_R_Addr	0x31

#define VL6180X_SYSTEM_MODE_GPIO0                     0X010
#define VL6180X_SYSTEM_MODE_GPIO1                     0X011

/* Interrupt mode source for Range readings[bit:2-0]:
    0: Disabled
    1: Level Low (value < thresh_low)
    2: Level High (value > thresh_high)
    3: Out Of Window (value < thresh_low OR value > thresh_high)
    4: New sample ready
*/
#define VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO          0x014
// Possible values returned from SYSTEM_INTERRUPT_CONFIG_GPIO  
// or possible options to set to indicate how interrupt should happen
#define VL6180X_INT_DISABLE          0
#define VL6180X_LEVEL_LOW            1
#define VL6180X_LEVEL_HIGH           2
#define VL6180X_OUT_OF_WINDOW        3
#define VL6180X_NEW_SAMPLE_READY     4

#define VL6180X_SYSTEM_INTERRUPT_CLEAR                0x015 // set bit 0 to 1 in order to clear interrupt for range
#define VL6180X_SYSTEM_FRESH_OUT_OF_RESET             0x016
#define VL6180X_SYSTEM_GROUPED_PARAMETER_HOLD         0x017
#define VL6180X_SYSRANGE_START                        0x018 // bit 0 (1 = start, 0 = stop) -- stop only used for continuous
                                                            // bit 1 (1 =  0 single measure, 1 = continuous)
// High Threshold value for ranging comparison. Range 0-255mm.
#define VL6180X_SYSRANGE_THRESH_HIGH                  0x019
//Low Threshold value for ranging comparison. Range 0-255mm.
#define VL6180X_SYSRANGE_THRESH_LOW                   0x01A
// Time delay between measurements in Ranging continuous mode. Range 0-254 (0 = 10ms). Step size = 10ms.
#define VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD      0x01B
//Maximum time to run measurement in Ranging modes.Range 1 - 63 ms
#define VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME         0x01C

// possible results of SYSRANGE
// #define VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE   0x022
// #define VL6180X_SYSRANGE_MAX_AMBIENT_LEVEL_MULT       0x02C
// #define VL6180X_SYSRANGE_RANGE_CHECK_ENABLES          0x02D
#define VL6180X_SYSRANGE_VHV_RECALIBRATE              0x02E
#define VL6180X_SYSRANGE_VHV_REPEAT_RATE              0x031
// #define VL6180X_RESULT_INTERRUPT_STATUS_GPIO          0x04F
//#define VL6180X_RESULT_RANGE_VAL                      0x062 // implicitly trunated????? 



#define VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD       0x10A


// #define VL6180X_FIRMWARE_RESULT_SCALER                0x120
//#define VL6180X_I2C_SLAVE_DEVICE_ADDRESS              0x212 // implicitly truncated???
//#define VL6180X_INTERLEAVED_MODE_ENABLE               0x2A3 // implicitly truncated???

#define VL6180X_RESULT_RANGE_STATUS                   0x04D
// possible results of RESULT_RANGE_STATUS
#define VL6180X_NO_ERR                                0x00
#define VL6180X_EARLY_CONV_ERR                        0x06
#define VL6180X_MAX_CONV_ERR                          0x07
#define VL6180X_IGNORE_ERR                            0x08
#define VL6180X_MAX_S_N_ERR                           0x0B
#define VL6180X_RAW_Range_UNDERFLOW_ERR               0x0C
#define VL6180X_RAW_Range_OVERFLOW_ERR                0x0D
#define VL6180X_Range_UNDERFLOW_ERR                   0x0E
#define VL6180X_Range_OVERFLOW_ERR                    0x0F

typedef enum
{
    SEN0427_RangeResult__NO_ERR = VL6180X_NO_ERR,
    SEN0427_RangeResult__EARLY_CONV_ERR = VL6180X_EARLY_CONV_ERR,
    SEN0427_RangeResult__MAX_CONV_ERR = VL6180X_MAX_CONV_ERR,
    SEN0427_RangeResult__IGNORE_ERR = VL6180X_IGNORE_ERR,
    SEN0427_RangeResult__MAX_S_N_ERR = VL6180X_MAX_S_N_ERR,
    SEN0427_RangeResult__RAW_Range_UNDERFLOW_ERR = VL6180X_RAW_Range_UNDERFLOW_ERR,
    SEN0427_RangeResult__RAW_Range_OVERFLOW_ERR = VL6180X_RAW_Range_OVERFLOW_ERR,
    SEN0427_RangeResult__Range_UNDERFLOW_ERR = VL6180X_Range_UNDERFLOW_ERR,
    SEN0427_RangeResult__Range_OVERFLOW_ERR = VL6180X_Range_OVERFLOW_ERR
} SEN0427_RangeResult;

// #define VL6180X_DIS_INTERRUPT        0
// #define VL6180X_HIGH_INTERRUPT       1
// #define VL6180X_LOW_INTERRUPT        2

typedef enum
{
	SEN0427_L = 0, // left SEN0427 sensor	-- I2C addr 0x30
	SEN0427_R = 1, // center SEN0427 sensor	-- I2C addr 0x31
	SEN0427_None = 10
} SEN0427_Device;

// Initializes all SEN0427 Devices
void SEN0427_InitAll(void);

// Initializes the specified device on I2C with proper address and settings
int SEN0427_InitDevice(SEN0427_Device device);

// Checks if the sensor no longer detects ground beneath it (1 if no ground ==> drop, 0 if ground ==> no drop)
int SEN0427_CheckForDrop(void);

// Returns the status of the current range measurement
SEN0427_RangeResult SEN0427_GetRangeResult(SEN0427_Device device);

// Retrieve a single measurement from the specified device
unsigned char SEN0427_GetSingleMeasurement(SEN0427_Device device);

void SEN0427_StartContinuousMeasurement(SEN0427_Device device);

void SEN0427_StopContinuousMeasurement(SEN0427_Device device);

// Should only be called manually after calling SEN0427_StartContinuousMeasurement,
// automatically called by GetSingleMeasurement
unsigned char SEN0427_ReadRangeMeasurement(SEN0427_Device device);
