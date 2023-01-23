/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "LTC6811.h"
#include "FEB_temp_volt_map.h"
#include "FEB_temp_volt.h"
#include "FEB_charge_CAN.h"
// #include "FEB_logger.h"
// #include "FEB_CAN.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define CELLS_PER_BANK        17
#define N_BANKS               1
#define N_ICS                 N_BANKS * 2


#define BITS_PER_MESSAGE 1
#define VOLTAGE_ID 0b0
#define TEMPERATURE_ID 0b1
#define IS_CHARGING 0

typedef struct {
  float voltage;
  float temperature;
} Cell;

typedef struct {
  Cell cells[CELLS_PER_BANK];
} Bank;

typedef struct {
  cell_asic config[N_ICS];
  Bank banks[N_BANKS];
} Accumulator;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MODULE_NAME     "BMS"

#define ENABLED               1
#define DISABLED              0
#define DATALOG_ENABLED       1
#define DATALOG_DISABLED      0


//ADC Command Configurations. See LTC681x.h for options.
#define ADC_OPT               ADC_OPT_DISABLED //!< ADC Mode option bit
#define ADC_CONVERSION_MODE   MD_7KHZ_3KHZ //!< ADC Mode
#define ADC_DCP               DCP_ENABLED //!< Discharge Permitted
#define CELL_CH_TO_CONVERT    CELL_CH_ALL //!< Channel Selection for ADC conversion
#define AUX_CH_TO_CONVERT     AUX_CH_ALL //!< Channel Selection for ADC conversion
#define STAT_CH_TO_CONVERT    STAT_CH_ALL //!< Channel Selection for ADC conversion
#define SEL_ALL_REG           REG_ALL //!< Register Selection
#define SEL_REG_A             REG_1 //!< Register Selection
#define SEL_REG_B             REG_2 //!< Register Selection

#define MEASUREMENT_LOOP_TIME 500 //!< Loop Time in milliseconds(ms)

//Under Voltage and Over Voltage Thresholds
#define OV_THRESHOLD          41000 //!< Over voltage threshold ADC Code. LSB = 0.0001 ---(4.1V)
#define UV_THRESHOLD          30000 //!< Under voltage threshold ADC Code. LSB = 0.0001 ---(3V)

//Loop Measurement Setup. These Variables are ENABLED or DISABLED. Remember ALL CAPS
#define WRITE_CONFIG          DISABLED  //!< This is to ENABLED or DISABLED writing into to configuration registers in a continuous loop
#define READ_CONFIG           DISABLED //!< This is to ENABLED or DISABLED reading the configuration registers in a continuous loop
#define MEASURE_CELL          ENABLED //!< This is to ENABLED or DISABLED measuring the cell voltages in a continuous loop
#define MEASURE_AUX           ENABLED //!< This is to ENABLED or DISABLED reading the auxiliary registers in a continuous loop
#define MEASURE_STAT          DISABLED //!< This is to ENABLED or DISABLED reading the status registers in a continuous loop
#define PRINT_PEC             DISABLED //!< This is to ENABLED or DISABLED printing the PEC Error Count in a continuous loop

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

Accumulator accumulator;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void measurement_loop(uint8_t datalog_en);
void print_menu(void);
void print_wrconfig(void);
void print_rxconfig(void);
void print_cells(uint8_t datalog_en);
void print_aux(uint8_t datalog_en);
void print_stat(void);
void print_sumofcells(void);
void check_mux_fail(void);
void print_selftest_errors(uint8_t adc_reg, int8_t error);
void print_overlap_results(int8_t error);
void print_digital_redundancy_errors(uint8_t adc_reg, int8_t error);
void print_open_wires(void);
void print_pec_error_count(void);
int8_t select_s_pin(void);
void print_wrpwm(void);
void print_rxpwm(void);
void print_wrsctrl(void);
void print_rxsctrl(void);
void print_wrcomm(void);
void print_rxcomm(void);
void print_conv_time(uint32_t conv_time);
void check_error(int error);
void serial_print_text(char data[]);
void serial_print_hex(uint8_t data);
char read_hex(void);
char get_char(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

Accumulator accumulator;
cell_asic bms_ic_arr[8]; //!< Global Battery Variable

uint8_t REFON = 1; //!< Reference Powered Up Bit
uint8_t ADCOPT = 0; //!< ADC Mode option bit
uint8_t GPIOBITS_A[5] = { 1, 1, 0, 0, 0 }; //!< GPIO Pin Control // Gpio 1,2,3,4,5    First two are ADC inputs, set to TRUE; latter 3 are MUX sel, outputs
uint16_t UV = UV_THRESHOLD; //!< Under-voltage Comparison Voltage
uint16_t OV = OV_THRESHOLD; //!< Over-voltage Comparison Voltage
uint8_t DCCBITS_A[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; //!< Discharge cell switch //Dcc 1,2,3,4,5,6,7,8,9,10,11,12
uint8_t DCTOBITS[4] = { 1, 0, 1, 0 }; //!< Discharge time value // Dcto 0,1,2,3 // Programed for 4 min


float getVoltage(uint16_t val) {
  return val == 65535 ? -42 : val * 0.0001;
}

//float getTemperature(uint16_t val) {
//  return val == 65535 ? -42 : 7550 - 16186 * (0.0001 * val) + 13149 * pow(0.0001 * val, 2) - 4755 * pow(0.0001 * val, 3) + 642 * pow(0.0001 * val, 4);
//}


int8_t pollTemperature(uint16_t temperature_ch) {
  int8_t error = 0;
  LTC6811_init_cfg(N_ICS, accumulator.config);
  GPIOBITS_A[4] = (temperature_ch >> 2) & 0b1;
  GPIOBITS_A[3] = (temperature_ch >> 1) & 0b1;
  GPIOBITS_A[2] = (temperature_ch >> 0) & 0b1;
  GPIOBITS_A[1] = 0b1;
  GPIOBITS_A[0] = 0b1;
  for (uint16_t i = 0; i < N_ICS; i+=1) {
    LTC6811_set_cfgr(i, accumulator.config, REFON, ADCOPT, GPIOBITS_A, DCCBITS_A, DCTOBITS, UV, OV);
  }

  LTC6811_reset_crc_count(N_ICS, accumulator.config);
  LTC6811_init_reg_limits(N_ICS, accumulator.config);

  wakeup_sleep(N_ICS);
  LTC6811_wrcfg(N_ICS, accumulator.config);

  wakeup_idle(N_ICS);
  LTC6811_adax(ADC_CONVERSION_MODE, AUX_CH_ALL);
  LTC6811_pollAdc();
  wakeup_idle(N_ICS);
  error = LTC6811_rdaux(SEL_ALL_REG, N_ICS, accumulator.config); // Set to read back all aux registers
  return error;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  FEB_CAN_Init(&hcan1);

  LTC6811_init_cfg(N_ICS, accumulator.config);
  LTC6811_set_cfgr(0, accumulator.config, REFON, ADCOPT, GPIOBITS_A, DCCBITS_A, DCTOBITS, UV, OV);

  //    temperatures[temperature_ch] = bms_ic_arr[ic_idx].aux.a_codes[1];

  LTC6811_reset_crc_count(N_ICS, accumulator.config);
  LTC6811_init_reg_limits(N_ICS, accumulator.config);

  float MIN_TEMPERATURE;
  float MAX_TEMPERATURE;
  float MIN_VOLTAGE;
  float MAX_VOLTAGE;
  float MAX_CURRENT;

  if (IS_CHARGING) {
    MIN_TEMPERATURE = 0;
    MAX_TEMPERATURE = 45;
    MIN_VOLTAGE = 3.0;
    MAX_VOLTAGE = 4.2;
    MAX_CURRENT = 12;
  } else {
    MIN_TEMPERATURE = -20;
    MAX_TEMPERATURE = 60;
    MIN_VOLTAGE = 3.0;
    MAX_VOLTAGE = 4.2;
  }

  // start charging
  if (IS_CHARGING) {
	  BMS_MESSAGE.max_voltage = (uint16_t) (MAX_VOLTAGE * CELLS_PER_BANK * 10);
	  BMS_MESSAGE.max_current = (uint16_t) (MAX_CURRENT * 10);
	  BMS_MESSAGE.control = 0;

	  FEB_CAN_Transmit(&hcan1);
  }
  uint8_t stop_charge = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    int8_t error = 0;

    char str[1024];

    LTC6811_init_cfg(N_ICS, accumulator.config);
    uint8_t temperature_ch = 0;
    GPIOBITS_A[4] = (temperature_ch >> 2) & 0b1;
    GPIOBITS_A[3] = (temperature_ch >> 1) & 0b1;
    GPIOBITS_A[2] = (temperature_ch >> 0) & 0b1;
    GPIOBITS_A[1] = 0b1;
    GPIOBITS_A[0] = 0b1;
    LTC6811_set_cfgr(N_ICS, accumulator.config, REFON, ADCOPT, GPIOBITS_A, DCCBITS_A, DCTOBITS, UV, OV);

    LTC6811_reset_crc_count(N_ICS, accumulator.config);
    LTC6811_init_reg_limits(N_ICS, accumulator.config);

    wakeup_sleep(N_ICS);
    LTC6811_wrcfg(N_ICS, accumulator.config);

    wakeup_idle(N_ICS);
    LTC6811_adcv(ADC_CONVERSION_MODE, ADC_DCP, CELL_CH_TO_CONVERT);
    LTC6811_pollAdc();
    wakeup_idle(N_ICS);

    error = LTC6811_rdcv(SEL_ALL_REG, N_ICS, accumulator.config);


    sprintf(str, "error code %d\r\n", error);
    // FEB_log(MODULE_NAME, "DEBUG", str);

    for (uint16_t bank_idx = 0; bank_idx < N_BANKS; bank_idx += 1) {
      accumulator.banks[bank_idx].cells[16].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[0]);
      accumulator.banks[bank_idx].cells[15].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[1]);
      accumulator.banks[bank_idx].cells[14].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[2]);
      accumulator.banks[bank_idx].cells[13].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[3]);
      accumulator.banks[bank_idx].cells[12].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[6]);
      accumulator.banks[bank_idx].cells[11].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[7]);
      accumulator.banks[bank_idx].cells[10].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[8]);
      accumulator.banks[bank_idx].cells[9].voltage = getVoltage(accumulator.config[bank_idx * 2].cells.c_codes[9]);

      accumulator.banks[bank_idx].cells[8].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[0]);
      accumulator.banks[bank_idx].cells[7].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[1]);
      accumulator.banks[bank_idx].cells[6].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[2]);
      accumulator.banks[bank_idx].cells[5].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[3]);
      accumulator.banks[bank_idx].cells[4].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[4]);
      accumulator.banks[bank_idx].cells[3].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[6]);
      accumulator.banks[bank_idx].cells[2].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[7]);
      accumulator.banks[bank_idx].cells[1].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[8]);
      accumulator.banks[bank_idx].cells[0].voltage = getVoltage(accumulator.config[bank_idx * 2 + 1].cells.c_codes[9]);
    }

    pollTemperature(0);
    for (uint16_t bank_idx = 0; bank_idx < N_BANKS; bank_idx += 1) {
      accumulator.banks[bank_idx].cells[12].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[16].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[1]);
      accumulator.banks[bank_idx].cells[4].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[8].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[1]);
    }

    pollTemperature(1);
    for (uint16_t bank_idx = 0; bank_idx < N_BANKS; bank_idx += 1) {
      accumulator.banks[bank_idx].cells[11].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[15].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[1]);
      accumulator.banks[bank_idx].cells[3].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[7].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[1]);
    }

    pollTemperature(2);
    for (uint16_t bank_idx = 0; bank_idx < N_BANKS; bank_idx += 1) {
      accumulator.banks[bank_idx].cells[10].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[14].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[1]);
      accumulator.banks[bank_idx].cells[2].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[6].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[1]);
    }

    pollTemperature(3);
    for (uint16_t bank_idx = 0; bank_idx < N_BANKS; bank_idx += 1) {
      accumulator.banks[bank_idx].cells[9].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[13].temperature = getTemperature(accumulator.config[bank_idx * 2].aux.a_codes[1]);
      accumulator.banks[bank_idx].cells[1].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[0]);
      accumulator.banks[bank_idx].cells[5].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[1]);
    }

    pollTemperature(4);
    for (uint16_t bank_idx = 0; bank_idx < N_BANKS; bank_idx += 1) {
      accumulator.banks[bank_idx].cells[0].temperature = getTemperature(accumulator.config[bank_idx * 2 + 1].aux.a_codes[0]);
    }

    uint8_t battery_charged = 1;
    for (uint16_t bank_idx = 0; bank_idx < N_BANKS; bank_idx++) {
    	if (IS_CHARGING) {
    		for (uint16_t cell_idx = 0; cell_idx < CELLS_PER_BANK; cell_idx++) {
    			// temperature check
    			float cell_temperature = accumulator.banks[bank_idx].cells[cell_idx].temperature;
    			if (cell_temperature < MIN_TEMPERATURE || cell_temperature > MAX_TEMPERATURE) {
    				stop_charge = 1;
    			}

    			// battery percentage check
    			float cell_voltage = accumulator.banks[bank_idx].cells[cell_idx].voltage;
    			if (cell_voltage < 0.95 * MAX_VOLTAGE) {
    				battery_charged = 0;
    			}
    		}
    	} else {
			// shutdown logic
			for (uint16_t cell_idx = 0; cell_idx < CELLS_PER_BANK; cell_idx++) {
				// voltage shutdown logic
				float cell_voltage = accumulator.banks[bank_idx].cells[cell_idx].voltage;
				if (cell_voltage < MIN_VOLTAGE || cell_voltage > MAX_VOLTAGE) {
					// initiate shutdown circuit
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

					sprintf(str, "Bank %d, Cell %d: Voltage\n", bank_idx, cell_idx);
					HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
				}


				// temperature shutdown logic
				float cell_temperature = accumulator.banks[bank_idx].cells[cell_idx].temperature;
				if (cell_temperature < MIN_TEMPERATURE || cell_temperature > MAX_TEMPERATURE) {
					// initiate shutdown circuit
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

					sprintf(str, "Bank %d, Cell %d: Temperature\n", bank_idx, cell_idx);
					HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
				}
			}
    	}



    	// send data via UART
    	char temp_str[256];

    	// voltage
    	sprintf(str, "%d", (bank_idx << BITS_PER_MESSAGE) + VOLTAGE_ID);
    	for (uint16_t cell_idx = 0; cell_idx < CELLS_PER_BANK; cell_idx++) {
    		sprintf(temp_str, " %f", accumulator.banks[bank_idx].cells[cell_idx].voltage);
    		strncat(str, temp_str, strlen(temp_str));
    	}
    	sprintf(temp_str, "\n");
    	strncat(str, temp_str, strlen(temp_str));

    	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);

    	// temperature
    	sprintf(str, "%d", (bank_idx << BITS_PER_MESSAGE) + TEMPERATURE_ID);
    	for (uint16_t cell_idx = 0; cell_idx < CELLS_PER_BANK; cell_idx++) {
    		sprintf(temp_str, " %f", accumulator.banks[bank_idx].cells[cell_idx].temperature);
    	    strncat(str, temp_str, strlen(temp_str));
    	}
    	sprintf(temp_str, "\n");
    	strncat(str, temp_str, strlen(temp_str));
    	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
    }

    if (IS_CHARGING) {
    	// check message received from charger
    	if (CAN_Rx_flag) {
    		FEB_reset_rx_flag();

    		// check voltage
    		if (CHARGER_MESSAGE.operating_voltage > MAX_VOLTAGE * 10) {
    			stop_charge = 1;
    		} else if (CHARGER_MESSAGE.operating_current > MAX_CURRENT * 10) {
    			stop_charge = 1;
    		}
    	}

    	if (battery_charged) {
    		stop_charge = 1;
    	}

    	if (stop_charge) {
    	    BMS_MESSAGE.max_voltage = 0;
    	    BMS_MESSAGE.max_current = 0;
    	    BMS_MESSAGE.control = 1;
    	}

    	FEB_CAN_Transmit(&hcan1);
    }

    if (IS_CHARGING) {
    	HAL_Delay(1000);
    } else {
    	HAL_Delay(1000);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 128;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 32;
  hcan1.Init.Mode = CAN_MODE_LOOPBACK;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_2TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
