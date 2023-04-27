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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ACC_PEDAL_1_RESET 0.0
#define ACC_PEDAL_1_FULL 3.3

#define ACC_PEDAL_2_RESET 0.0
#define ACC_PEDAL_2_FULL 3.3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

CAN_HandleTypeDef hcan1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint32_t buffer[6]; //Storing two values in the buffer
uint8_t flag=0;

struct {
  uint16_t torque;
  uint8_t enabled;
} RMSControl;

float normalized_acc;
float normalized_brake;

float pedal1;
float pedal2;
float pedal3;
float pedal4;
/*  VALUES FOR DEBUGGING THE BUFFER
float pedal5;
float pedal6;
*/

float getPedal(uint32_t variable){
	return (float)variable*3.3/4096;
}

float FEB_Normalized_Acc_Pedals(){
	float acc_pedal_1 = getPedal(buffer[2]);
	float acc_pedal_2 = getPedal(buffer[3]);

	float ped1_normalized = (acc_pedal_1 - ACC_PEDAL_1_RESET)/ (ACC_PEDAL_1_FULL - ACC_PEDAL_1_RESET);
	float ped2_normalized = (acc_pedal_2 - ACC_PEDAL_2_RESET) / (ACC_PEDAL_2_FULL - ACC_PEDAL_2_RESET);

	if(abs(ped1_normalized - ped2_normalized) > 0.1 ){
		flag = 1;
	}

	float final_normalized = 0.5*(ped1_normalized + ped2_normalized);

	final_normalized = final_normalized > 1 ? 1 : final_normalized;
	final_normalized = final_normalized < 0.05 ? 0 : final_normalized;

	return final_normalized;

}
void FEB_LVPDB_sendBrake(){
	FEB_CAN_Transmit(&hcan1,LVPDB_ID,normalized_brake,sizeof(float));
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
	uint8_t message_data[8] = {0,0,0,0,0,0,0};
	normalized_acc = 0;
	normalized_brake = 0;

	for (int i=0; i<100; i+=1) {
		FEB_CAN_Transmit(&hcan1, 0x0C0, message_data, 8);
	    HAL_Delay(100);
	}
	FEB_RMS_enable();
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_ADC1_Init(void);
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_CAN1_Init();
  MX_ADC1_Init();

  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1,buffer,6);


//  uint8_t sleep_time = 10;
  char buf[128];
  uint8_t buf_len;

	/* Node_1 */
//	FEB_CAN_Init(&hcan1, BMS_ID);
//	float temp = 0.0;
//	float volt = 0.0;

	/* Node_2 */
//	FEB_CAN_Init(&hcan1, SM_ID);
//	uint8_t cmd_1 = 0;

	/* Node_3 */
	//FEB_CAN_Init(&hcan1, APPS_ID); // The transceiver must be connected otherwise you get sent into an infinite loop
	//FEB_RMS_Init();
//	float acc_1 = 0.0;
//	float acc_2 = 0.0;
//	float brake = 0.0;
//	float torque = 0.0;

	/* Node_4 */
//	FEB_CAN_Init(&hcan1, SM_ID);
//	uint8_t emergency = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
	  /* Node_1 */
//	  temp = temp + 1;
//	  FEB_CAN_Transmit(&hcan1, BMS_TEMPERATURE, &temp, sizeof(BMS_TEMPERATURE_TYPE));
//	  volt = volt + 2;
//	  FEB_CAN_Transmit(&hcan1, BMS_VOLTAGE, &volt, sizeof(BMS_VOLTAGE_TYPE));
//	  buf_len = sprintf(buf, "BMS node. Receiving \nSM_CMD1: %d \nEmergency: %d \n\n", SM_MESSAGE.command_1, EMERGENCY_MESSAGE.sm_emergency);
//	  HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 10);

	  /* Node_2 */
//	  cmd_1 += 1;
//	  FEB_CAN_Transmit(&hcan1, SM_COMMAND_1, &cmd_1, sizeof(SM_COMMAND_1_TYPE));
//	  buf_len = sprintf(buf, "SM node. Receiving \nBMS_temp:%d BMS_volt:%d \n\n", BMS_MESSAGE.temperature, BMS_MESSAGE.voltage);
//	  HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 10);

	  /* Node_3 */
//	  acc_1 = acc_1 + 0.1;
//	  FEB_CAN_Transmit(&hcan1, APPS_ACCELERATOR1_PEDAL, &acc_1, sizeof(APPS_ACCELERATOR1_PEDAL_TYPE));
//	  acc_2 = acc_2 + 0.2;
//	  FEB_CAN_Transmit(&hcan1, APPS_ACCELERATOR2_PEDAL, &acc_2, sizeof(APPS_ACCELERATOR2_PEDAL_TYPE));
//	  brake = acc_2 + 0.3;
//	  FEB_CAN_Transmit(&hcan1, APPS_BRAKE_PEDAL, &brake, sizeof(APPS_BRAKE_PEDAL_TYPE));
//	  torque = torque + 0.4;
//	  FEB_CAN_Transmit(&hcan1, APPS_TORQUE, &torque, sizeof(APPS_TORQUE_TYPE));
//	  buf_len = sprintf(buf, "APPS node. Receiving \nSM_CMD1: %d \n\n", SM_MESSAGE.command_1);
//	  HAL_UART_Transmit(&huart2, (uint8_t *)buf, buf_len, 10);

	  /* Node_4 */
//	  emergency += 1;
//	  FEB_CAN_Transmit(&hcan1, EMERGENCY_SM_EMERGENCY, &emergency, sizeof(EMERGENCY_SM_EMERGENCY_TYPE));

	  //void FEB_CAN_Transmit(CAN_HandleTypeDef* hcan, AddressIdType Msg_ID, void* pData, uint8_t size) {


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  pedal1 = getPedal(buffer[2]);
	  pedal2 = getPedal(buffer[3]);

	  pedal3 = getPedal(buffer[4]);
	  pedal4 = getPedal(buffer[5]);

	  normalized_acc = pedal1/3.3;
	  normalized_brake = pedal3/3.3;

	  buf_len = sprintf(buf, "Pedal Value: %.6f \n", normalized_acc);
	  HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len,10);
	  uint16_t torque = normalized_acc * 50;


	  //Transmit CAN messages to other boards

	  //FEB_RMS_setTorque(torque);
	  //FEB_LVPDB_sendBrake();

	  HAL_Delay(100);
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
  hadc1.Init.NbrOfConversion = 6;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

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
