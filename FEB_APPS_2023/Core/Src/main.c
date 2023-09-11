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
#include "FEB_CAN.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SLEEP_TIME 10

#define ACC_PEDAL_1_START 860.0
#define ACC_PEDAL_1_END 1110.0

#define ACC_PEDAL_2_START 3245.0
#define ACC_PEDAL_2_END 2995.0

//#define BRAKE_PEDAL_1_START 1350.0
//#define BRAKE_PEDAL_1_END 910.0

#define BRAKE_PEDAL_1_START 385.0
#define BRAKE_PEDAL_1_END 565.0

#define BRAKE_PEDAL_2_START 1390.0
#define BRAKE_PEDAL_2_END 1160.0

#define PRESURE_START 345.0
#define PRESURE_END 380.0

#define INIT_VOLTAGE 242 // initial voltage of accumulator
#define PEAK_CURRENT 95  // max current (in amps) we want to pull
#define MAX_TORQUE 230
#define RPM_TO_RAD_S .10472

const uint16_t Sensor_Min = 4095.0/5.0*0.5;
const uint16_t Sensor_Max = 4095.0/5.0*4.5;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

CAN_HandleTypeDef hcan1;

TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t buffer[6]; //Storing two values in the buffer
bool isImpl = false;

struct {
  uint16_t torque;
  uint8_t enabled;
} RMSControl;

float normalized_acc;
float normalized_brake;

//float pedal1;
//float pedal2;
//float pedal3;
//float pedal4;
/*  VALUES FOR DEBUGGING THE BUFFER
float pedal5;
float pedal6;
*/

//float getPedal(uint32_t variable){
//	return (float)variable*3.3/4096;
//}

float FEB_Normalized_Acc_Pedals(){
	// raw ADC readings of the two acc pedal sensors
	uint16_t acc_pedal_1 = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
	uint16_t acc_pedal_2 = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
	char buf[128];
	uint8_t buf_len;
	buf_len = sprintf(buf, "acc1:%d acc2:%d\n", acc_pedal_1, acc_pedal_2);
	//HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);


	// check implausibility for shorting
	if (acc_pedal_1 < Sensor_Min || acc_pedal_1 > Sensor_Max
			|| acc_pedal_2 < Sensor_Min || acc_pedal_2 > Sensor_Max
			|| abs(acc_pedal_1 - acc_pedal_2) < 100) {
		isImpl = true;
		return 0.0;
	}

	//convert to % travel
	// sensor 1 has positive slope
	float ped1_normalized = (acc_pedal_1 - ACC_PEDAL_1_START)/ (ACC_PEDAL_1_END - ACC_PEDAL_1_START);
	// sensor 2 has negative slope
	float ped2_normalized = (acc_pedal_2 - ACC_PEDAL_2_START) / (ACC_PEDAL_2_END - ACC_PEDAL_2_START);

	// sensor measurements mismatch by more than 10%
	if(abs(ped1_normalized - ped2_normalized) > 0.1 ){
		isImpl = true;
		return 0.0;
	}

	float final_normalized = 0.5*(ped1_normalized + ped2_normalized);

	// Implausiblity check if both pedals are stepped
	if (normalized_brake > 0.2 && normalized_acc > 0.1) {
		isImpl = true;
	}

	// recover from implausibility if acc pedal is not 5% less
	if (final_normalized < 0.05 && isImpl) {
		isImpl = false;
	}

	if (!isImpl) {
		final_normalized = final_normalized > 1 ? 1 : final_normalized;
		final_normalized = final_normalized < 0.05 ? 0 : final_normalized;
		return final_normalized;
	} else {
		return 0.0;
	}
}

float FEB_Normalized_Brake_Pedals(){
	uint16_t brake_pedal_1 = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
	char buf[128];
		uint8_t buf_len;
		buf_len = sprintf(buf, "brake%d\n", brake_pedal_1);
		HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);

	float final_normalized = (brake_pedal_1 - BRAKE_PEDAL_1_START)/ (BRAKE_PEDAL_1_END - BRAKE_PEDAL_1_START);
	final_normalized = final_normalized > 1 ? 1 : final_normalized;
	final_normalized = final_normalized < 0.05 ? 0 : final_normalized;

	if (brake_pedal_1 < 10 || brake_pedal_1 > 4085) {
		return 0.0;
	}

	return final_normalized;
}


void FEB_APPS_sendBrake(){
	FEB_CAN_Transmit(&hcan1,APPS_BRAKE_PEDAL,&normalized_brake,sizeof(float));
}

void FEB_RMS_updateTorque() {
  uint8_t message_data[8] = {RMSControl.torque & 0xFF, RMSControl.torque >> 8, 0, 0, 0, RMSControl.enabled, 0, 0};
  FEB_CAN_Transmit(&hcan1, 0x0C0, message_data, 8);
}

void FEB_RMS_setTorque(uint16_t torque) {
  RMSControl.torque = torque * 10;
  FEB_RMS_updateTorque();
}

void FEB_RMS_enable() {
  RMSControl.enabled = 1;
  FEB_RMS_updateTorque();
}

void FEB_RMS_disable() {
  RMSControl.enabled = 0;
  FEB_RMS_updateTorque();
}

void FEB_RMS_Init(){
	// Clear fault in case inverter is powered up before disable command is sent
	uint8_t fault_clear_addr = 20;
	uint8_t fault_clear_data = 0;
	// param msg format (little-endian):
	// 0,1: param addr
	// 2: r/w cmd
	// 3: NA
	// 4,5: data
	// 6,7: NA
	uint8_t param_msg[8] = {fault_clear_addr, 0, 1, 0, fault_clear_data, 0, 0, 0};
	FEB_CAN_Transmit(&hcan1, 0x0C1, param_msg, 8);

	// send disable command to remove lockout
	uint8_t message_data[8] = {0,0,0,0,0,0,0};
	for (int i = 0; i < 10; i++) {
		FEB_CAN_Transmit(&hcan1, 0x0C0, message_data, 8);
		HAL_Delay(10);
	}

	// Select CAN msg to broadcast
	uint8_t param_addr = 148;
	uint8_t CAN_active_msg_byte4 = 0b10100000; // motor position, input voltage
	uint8_t CAN_active_msg_byte5 = 0b10010100; // flux info (dq axes), torque/timer info, internal states
//	uint8_t CAN_active_msg_byte4 = 0xff;
//	uint8_t CAN_active_msg_byte5 = 0xff;
	uint8_t broadcast_msg[8] = {param_addr, 0, 1, 0, CAN_active_msg_byte4, CAN_active_msg_byte5, 0, 0};
	FEB_CAN_Transmit(&hcan1, 0x0C1, broadcast_msg, 8);
}

int16_t min(int16_t x1, int16_t x2) {
	if (x1 < x2) {
		return x1;
	}
	return x2;
}

uint16_t FEB_getTorque(float normalized_acc) {
	int16_t accumulator_voltage = min(INIT_VOLTAGE, (RMS_MESSAGE.HV_Bus_Voltage - 50) / 10);
	int16_t motor_speed = -1 * RMS_MESSAGE.Motor_Speed * RPM_TO_RAD_S;
	if (motor_speed == 0) {
		return normalized_acc * MAX_TORQUE;
	}
	uint16_t maxTorque = min(MAX_TORQUE, (accumulator_voltage * PEAK_CURRENT) / motor_speed);
	return normalized_acc * maxTorque;
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_ADC1_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_InjectedStart(&hadc1);
  HAL_TIM_Base_Start(&htim5);


  char buf[128];
  uint8_t buf_len;

  FEB_CAN_Init(&hcan1, APPS_ID); // The transceiver must be connected otherwise you get sent into an infinite loop
  RMSControl.enabled = 0;
  RMSControl.torque= 0.0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //ready to drive
	  if (SW_MESSAGE.ready_to_drive == 1) {
		  normalized_acc = FEB_Normalized_Acc_Pedals();
		  if (!RMSControl.enabled) {  // when the car just powered on, driver commands ready to drive but rms is not enabled
			  FEB_RMS_Init();
			  RMSControl.enabled = 1;
		  }
	  } else {
		  normalized_acc = FEB_Normalized_Acc_Pedals();
		  normalized_acc = 0.0;
		  RMSControl.enabled = 0;
	  }

	  uint16_t torque = FEB_getTorque(normalized_acc);
	  normalized_brake = FEB_Normalized_Brake_Pedals();
	  FEB_RMS_setTorque(torque);
	  FEB_APPS_sendBrake();

	  buf_len = sprintf(buf, "rtd:%d, enable:%d lockout:%d impl:%d acc: %.3f brake: %.3f Bus Voltage: %d Motor Speed: %d\n", SW_MESSAGE.ready_to_drive, Inverter_enable, Inverter_enable_lockout, isImpl, normalized_acc, normalized_brake, RMS_MESSAGE.HV_Bus_Voltage, RMS_MESSAGE.Motor_Speed);

	  HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, 1000);

	  HAL_Delay(SLEEP_TIME);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 160;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_1;
  sConfigInjected.InjectedRank = 1;
  sConfigInjected.InjectedNbrOfConversion = 3;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_84CYCLES;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_RISING;
  sConfigInjected.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJECCONV_T5_TRGO;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_4;
  sConfigInjected.InjectedRank = 2;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_5;
  sConfigInjected.InjectedRank = 3;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
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
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 159;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 999;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

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
  while (1)
  {
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
