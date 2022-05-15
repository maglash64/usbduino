/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usb_device.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PID_ERROR                   0x0
#define PID_OK                      0x1
#define PID_DEVICE                  0x2

//GPIO specific packet id's
#define PID_SET_GPIO                0x3
#define PID_SET_GPIO_VAL            0x4
#define PID_GET_GPIO_VAL            0x5

//ADC specific packet id's
#define PID_SET_ADC                 0x6
#define PID_GET_ADC_SINGLE          0x7
#define PID_GET_ADC_STREAM          0x8

//DAC specific packet id's
#define PID_SET_DAC                 0x9
#define PID_SET_DAC_SINGLE          0xA
#define PID_SET_DAC_STREAM          0xB

//UART specific packet id's

#define PID_UART_SET_DEVICE         0x7
#define PID_UART_GET_DEVICE         0x8

#define PID_UART_TRANSMIT           0x9
#define PID_UART_RECIEVE            0xA

//I2C specific packet id's
#define PID_I2C_SET_DEVICE          0xB
#define PID_I2C_GET_DEVICE          0xC

#define PID_I2C_TRANSMIT
#define PID_I2C_RECIEVE





/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t	usb_buffer[64];
uint8_t	usb_rx_ready;
uint8_t	usb_tx_ready = 1;

struct PacketTypeDef
{
	uint16_t pid;
	uint16_t length;
	uint16_t intr;
	uint8_t data[58];
}__attribute__((packed)) *packet;

struct PinTypeDef
{
	uint32_t port;
	uint32_t pin;
	uint32_t dir;
	uint32_t pull;
	uint32_t val;
}__attribute__((packed)) *pin_config;

struct ErrorTypeDef
{
	uint16_t	type;
	uint8_t		str[56];
}__attribute__((packed)) *error;

struct Report{
	int8_t 		throttle;
	int8_t 		roll;
	int8_t 		pitch;
	int8_t 		yaw;
	uint8_t  	btn1 	: 1;
	uint8_t  	btn2	: 1;
	uint8_t  	btn3	: 1;
	uint8_t  	btn4	: 1;
	uint8_t  	btn5 	: 1;
	uint8_t  	btn6	: 1;
	uint8_t  	btn7	: 1;
	uint8_t  	btn8	: 1;
}__attribute__((packed)) report;


GPIO_TypeDef *mapGpioPort(uint32_t port)
{
	GPIO_TypeDef * ret;
	switch(port)
	{
	case 1:
		ret = GPIOA;
		break;
	case 2:
		ret = GPIOB;
		break;
	case 3:
		ret = GPIOC;
		break;
	case 4:
		ret = GPIOD;
		break;
	case 5:
		ret = GPIOE;
		break;
	default:
		ret = GPIOA;
		break;
	}
	return ret;
}

uint16_t mapGpioPin(uint32_t pin)
{
	uint16_t ret;

	switch(pin)
	{
	case 0:
		ret = GPIO_PIN_0;
		break;
	case 1:
		ret = GPIO_PIN_1;
		break;
	case 2:
		ret = GPIO_PIN_2;
		break;
	case 3:
		ret = GPIO_PIN_3;
		break;
	case 4:
		ret = GPIO_PIN_4;
		break;
	case 5:
		ret = GPIO_PIN_5;
		break;
	case 6:
		ret = GPIO_PIN_6;
		break;
	case 7:
		ret = GPIO_PIN_7;
		break;
	case 8:
		ret = GPIO_PIN_8;
		break;
	case 9:
		ret = GPIO_PIN_9;
		break;
	case 10:
		ret = GPIO_PIN_10;
		break;
	case 11:
		ret = GPIO_PIN_11;
		break;
	case 12:
		ret = GPIO_PIN_12;
		break;
	case 13:
		ret = GPIO_PIN_13;
		break;
	case 14:
		ret = GPIO_PIN_14;
		break;
	case 15:
		ret = GPIO_PIN_15;
		break;
	case 16:
		ret = GPIO_PIN_All;
		break;
	default:
		ret = GPIO_PIN_0;
	}
	return ret;
}

uint32_t mapGpioDir(uint32_t dir)
{
	uint32_t ret;
	switch(pin_config->dir)
	{
	case 1:
		ret = GPIO_MODE_AF_INPUT;
		break;
	case 2:
		ret = GPIO_MODE_OUTPUT_PP;
		break;
	case 3:
		ret = GPIO_MODE_OUTPUT_OD;
		break;
	case 4:
		ret = GPIO_MODE_AF_PP;
		break;
	case 5:
		ret = GPIO_MODE_AF_OD;
		break;
	case 6:
		ret = GPIO_MODE_ANALOG;
		break;
	default:
		ret = GPIO_MODE_OUTPUT_PP;
		break;
	}
	return ret;
}



uint16_t mapAdcPin(uint32_t pin)
{
	uint16_t ret;
	switch(pin)
	{
	case 1:
		ret = ADC_CHANNEL_0;
		break;
	case 2:
		ret = ADC_CHANNEL_1;
		break;
	case 3:
		ret = ADC_CHANNEL_2;
		break;
	case 4:
		ret = ADC_CHANNEL_3;
		break;
	case 5:
		ret = ADC_CHANNEL_4;
		break;
	case 6:
		ret = ADC_CHANNEL_5;
		break;
	case 7:
		ret = ADC_CHANNEL_6;
		break;
	case 8:
		ret = ADC_CHANNEL_7;
		break;
	case 9:
		ret = ADC_CHANNEL_8;
		break;
	case 10:
		ret = ADC_CHANNEL_9;
		break;
	default:
		ret = ADC_CHANNEL_0;
	}
	return ret;
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
  /* USER CODE BEGIN 2 */
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_ChannelConfTypeDef sConfig = {0};
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(usb_rx_ready)
		{
			packet = (struct PacketTypeDef*)usb_buffer;

			switch(packet->pid)
			{
				case PID_DEVICE:
				{
					//THE DEVICE IS STM32F103
					packet->length = 2;
					packet->data[0] = 0xFF;
					packet->data[1] = 0x11;
				}break;

				case PID_SET_GPIO:
				{
					pin_config = (struct PinTypeDef*) packet->data;

					GPIO_InitStruct.Mode = mapGpioDir(pin_config->dir);
					GPIO_InitStruct.Pin = mapGpioPin(pin_config->pin);
					GPIO_InitStruct.Pull = pin_config->pull;
					GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
					HAL_GPIO_Init((GPIO_TypeDef*)mapGpioPort(pin_config->port), &GPIO_InitStruct);

					//HAL_NVIC_EnableIRQ()
					packet->pid = PID_OK;

					USBD_Transmit(&hUsbDeviceFS, (uint8_t*)packet, 0x40);
				}break;

				case PID_SET_GPIO_VAL:
				{
					pin_config = (struct PinTypeDef*) packet->data;

					//write to a digital pin..
					HAL_GPIO_WritePin(mapGpioPort(pin_config->port), mapGpioPin(pin_config->pin), pin_config->val);
					packet->pid = PID_OK;

					USBD_Transmit(&hUsbDeviceFS, (uint8_t*)packet, 0x40);
				}break;

				case PID_GET_GPIO_VAL:
				{
					pin_config = (struct PinTypeDef*) packet->data;

					//read from a digital pin..
					pin_config->val = HAL_GPIO_ReadPin(mapGpioPort(pin_config->port), mapGpioPin(pin_config->pin));
					packet->pid = PID_OK;

					USBD_Transmit(&hUsbDeviceFS, (uint8_t*)packet, 0x40);
				}break;

				case PID_GET_ADC_SINGLE:
				{
					pin_config = (struct PinTypeDef*) packet->data;

					packet->pid = PID_OK;

					sConfig.Channel = mapAdcPin(pin_config->pin);
					sConfig.Rank = ADC_REGULAR_RANK_1;
					sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

					if(pin_config->pin % 2 == 0)
					{
						if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
						{
							packet->pid = PID_ERROR;
							error = (struct ErrorTypeDef*) packet->data;
							sprintf((char*)error->str,"Cannot Configure ADC channel!\n");
						}
						HAL_ADC_Start(&hadc1);
						HAL_ADC_PollForConversion(&hadc1, 1);
						pin_config->val =  HAL_ADC_GetValue(&hadc1);
						HAL_ADC_Stop(&hadc1);
					}else
					{
						if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
						{
							packet->pid = PID_ERROR;
							error = (struct ErrorTypeDef*) packet->data;
							sprintf((char*)error->str,"Cannot Configure ADC channel!\n");
						}
						HAL_ADC_Start(&hadc2);
						HAL_ADC_PollForConversion(&hadc2, 1);
						pin_config->val =  HAL_ADC_GetValue(&hadc2);
						HAL_ADC_Stop(&hadc2);
					}

					USBD_Transmit(&hUsbDeviceFS, (uint8_t*)packet, 0x40);
				}break;

				default:
				{
					packet->pid = PID_ERROR;

					USBD_Transmit(&hUsbDeviceFS, (uint8_t*)packet, 0x40);
				}break;

			}
			usb_rx_ready = 0;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
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


  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */
  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

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

