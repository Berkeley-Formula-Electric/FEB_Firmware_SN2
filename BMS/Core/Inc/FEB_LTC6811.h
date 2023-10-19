#ifndef INC_FEB_LTC6811_H_
#define INC_FEB_LTC6811_H_

// ********************************** Include **********************************

#include "stdio.h"
#include "stdbool.h"
#include "string.h"

#include "stm32f4xx_hal.h"
#include "LTC6811.h"
#include "FEB_BMS_Shutdown.h"
#include "FEB_LTC6811_Temp_LUT.h"
#include "FEB_CAN_Charger.h"

// ********************************** Accumulator Configuration **********************************

// accumulator configuration
#define FEB_LTC6811_NUM_CELLS_PER_BANK 				17
#define FEB_LTC6811_NUM_BANKS 						4
#define FEB_LTC6811_NUM_CELLS_PER_DAUGHTER_BOARD	12

// cell temperature thresholds
#define FEB_LTC6811_CELL_MIN_OPERATION_TEMPERATURE	-20
#define FEB_LTC6811_CELL_MAX_OPERATION_TEMPERATURE	60
#define FEB_LTC6811_CELL_MIN_CHARGING_TEMPERATURE	0
#define FEB_LTC6811_CELL_MAX_CHARGING_TEMPERATURE	60

// cell voltage thresholds
#define FEB_LTC6811_CELL_MIN_VOLTAGE	3.0
#define FEB_LTC6811_CELL_MAX_VOLTAGE	4.19

// cell current thresholds
#define FEB_LTC6811_CELL_MAX_OPERATING_CURRENT 55

// ********************************** Cell Balance Configuration **********************************

#define FEB_LTC6811_BALANCE_STATE 					0		// 0 (no cell balancing), 1 (passive cell balancing)
#define FEB_LTC6811_BALANCE_VOLTAGE_RESOLUTION 		0.0

// ********************************** Connectivity Configuration **********************************

#define FEB_LTC6811_UART_BITS_PER_MESSAGE 	3
#define FEB_LTC6811_UART_VOLTAGE_ID 		0b000
#define FEB_LTC6811_UART_TEMPERATURE_ID 	0b001
#define FEB_LTC6811_UART_DISCHARGE_ID 		0b010

// ********************************** LTC6811 Configuration **********************************

#define NUM_IC	 				FEB_LTC6811_NUM_BANKS * 2

#define ENABLED 				1
#define DISABLED 				0
#define DATALOG_ENABLED 		1
#define DATALOG_DISABLED 		0

#define ADC_OPT 				ADC_OPT_DISABLED 	//!< ADC Mode option bit
#define ADC_CONVERSION_MODE 	MD_7KHZ_3KHZ 		//!< ADC Mode
#define ADC_DCP 				DCP_ENABLED 		//!< Discharge Permitted
#define CELL_CH_TO_CONVERT 		CELL_CH_ALL 		//!< Channel Selection for ADC conversion
#define AUX_CH_TO_CONVERT 		AUX_CH_ALL 			//!< Channel Selection for ADC conversion
#define STAT_CH_TO_CONVERT 		STAT_CH_ALL 		//!< Channel Selection for ADC conversion
#define SEL_ALL_REG 			REG_ALL 			//!< Register Selection
#define SEL_REG_A 				REG_1 				//!< Register Selection
#define SEL_REG_B 				REG_2				//!< Register Selection

#define MEASUREMENT_LOOP_TIME 	500					//!< Loop Time in milliseconds(ms)

// Voltage thresholds (0.1 mV)
#define OV_THRESHOLD 			FEB_LTC6811_CELL_MAX_VOLTAGE * 10000 	//!< Over voltage threshold ADC Code. LSB = 0.0001 ---(4.2V)
#define UV_THRESHOLD 			FEB_LTC6811_CELL_MIN_VOLTAGE * 10000	//!< Under voltage threshold ADC Code. LSB = 0.0001 ---(3V)

// Loop measurement setup
#define WRITE_CONFIG 			DISABLED	//!< This is to ENABLED or DISABLED writing into to configuration registers in a continuous loop
#define READ_CONFIG 			DISABLED	//!< This is to ENABLED or DISABLED reading the configuration registers in a continuous loop
#define MEASURE_CELL			ENABLED 	//!< This is to ENABLED or DISABLED measuring the cell voltages in a continuous loop
#define MEASURE_AUX 			DISABLED 	//!< This is to ENABLED or DISABLED reading the auxiliary registers in a continuous loop
#define MEASURE_STAT 			DISABLED 	//!< This is to ENABLED or DISABLED reading the status registers in a continuous loop
#define PRINT_PEC 				DISABLED 	//!< This is to ENABLED or DISABLED printing the PEC Error Count in a continuous loop

// ********************************** Structs **********************************

typedef struct {
	float temperature;
	float voltage;
} Cell;

typedef struct {
	Cell cells[FEB_LTC6811_NUM_CELLS_PER_BANK];
} Bank;

typedef struct {
	cell_asic IC_config[NUM_IC];
	Bank banks[FEB_LTC6811_NUM_BANKS];	// BMS_IC
} Accumulator;

// ********************************** Function Declarations **********************************

void FEB_LTC6811_Init(void);
void FEB_LTC6811_Config_Cell_Discharge(void);

// Read voltage
void FEB_LTC6811_Poll_Voltage(void);
void FEB_LTC6811_Start_Cell_ADC_Measurement(void);
void FEB_LTC6811_Read_Cell_Voltage_Registers(void);
void FEB_LTC6811_Store_Voltage(void);
float FEB_LTC6811_Convert_Voltage(uint16_t value);

// Voltage balance cells
uint8_t FEB_LTC6811_Get_IC(uint8_t bank, uint8_t cell);
uint8_t FEB_LTC6811_Cell_Idx(uint8_t cell);
void FEB_LTC6811_Set_Discharge_Target_Voltage(void);
void FEB_LTC6811_Balance_Cells(void);
void FEB_LTC6811_Balance_Cell(uint8_t bank, uint8_t cell);
void FEB_LTC6811_Clear_Balance_Cells(void);
void FEB_LTC6811_Clear_Balance_Cell(uint8_t bank, uint8_t cell);
void FEB_LTC6811_Configure_DCCBITS_A(uint8_t ic);
void FEB_LTC6811_UART_Transmit_Discharge(void);

// Voltage interface
void FEB_LTC6811_Validate_Voltage(void);
void FEB_LTC6811_UART_Transmit_Voltage(void);
float FEB_LTC6811_Total_Bank_Voltage(void);
uint8_t FEB_LTC6811_Cells_Charged(void);
void FEB_LTC6811_Clear_Voltage(void);

// Read temperature
void FEB_LTC6811_Poll_Temperature(void);
void FEB_LTC6811_Update_GPIO(uint8_t channel);
void FEB_LTC6811_Start_GPIO_ADC_Measurement(void);
void FEB_LTC6811_Read_Aux_Voltages(void);
void FEB_LTC6811_Store_Temperature(uint8_t channel);
float FEB_LTC6811_Convert_Temperature(uint16_t value);

// Temperature interface
void FEB_LTC6811_Validate_Temperature(void);
void FEB_LTC6811_UART_Transmit_Temperature(void);
void FEB_LTC6811_Clear_Temperature(void);

#endif /* INC_FEB_LTC6811_H_ */
