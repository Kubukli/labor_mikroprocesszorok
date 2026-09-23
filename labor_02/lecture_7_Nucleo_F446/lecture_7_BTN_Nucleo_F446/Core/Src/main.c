/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DEBOUNCING_TRESHOLD 2500

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

static void LAB_GPIO_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void LAB_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;

    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOG,
                      GPIO_PIN_14,
                      GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOF,
                      GPIO_PIN_15 | GPIO_PIN_14,
                      GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOE,
                      GPIO_PIN_13,
                      GPIO_PIN_RESET);
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

  LAB_GPIO_Init();


  uint32_t count_debounce = 0;

  uint8_t previous_state_debounce = 0;

  uint8_t current_state_debounce =
      HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_9);

  uint8_t previous_state_edge =
      HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_9);

  uint32_t counter = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

      count_debounce = 0;

      while (count_debounce < DEBOUNCING_TRESHOLD)
      {
          previous_state_debounce = current_state_debounce;

          current_state_debounce =
              HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_9);

          if (previous_state_debounce == current_state_debounce)
          {
              count_debounce++;
          }
      }

      if (current_state_debounce && !previous_state_edge)
      {

          counter++;

          HAL_GPIO_WritePin(
              GPIOG,
              GPIO_PIN_14,
              (counter & 0x08) ?
              GPIO_PIN_SET : GPIO_PIN_RESET
          );

          HAL_GPIO_WritePin(
              GPIOF,
              GPIO_PIN_15,
              (counter & 0x04) ?
              GPIO_PIN_SET : GPIO_PIN_RESET
          );

          HAL_GPIO_WritePin(
              GPIOE,
              GPIO_PIN_13,
              (counter & 0x02) ?
              GPIO_PIN_SET : GPIO_PIN_RESET
          );

          HAL_GPIO_WritePin(
              GPIOF,
              GPIO_PIN_14,
              (counter & 0x01) ?
              GPIO_PIN_SET : GPIO_PIN_RESET
          );
      }

      previous_state_edge = current_state_debounce;


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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

  /**
   * Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /**
   * Initializes the RCC Oscillators
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;

  RCC_OscInitStruct.HSIState = RCC_HSI_ON;

  RCC_OscInitStruct.HSICalibrationValue =
      RCC_HSICALIBRATION_DEFAULT;

  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;


  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
      Error_Handler();
  }


  /**
   * Initializes the CPU, AHB and APB buses clocks
   */

  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_SYSCLK |
      RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2;


  RCC_ClkInitStruct.SYSCLKSource =
      RCC_SYSCLKSOURCE_HSI;


  RCC_ClkInitStruct.AHBCLKDivider =
      RCC_SYSCLK_DIV1;


  RCC_ClkInitStruct.APB1CLKDivider =
      RCC_HCLK_DIV1;


  RCC_ClkInitStruct.APB2CLKDivider =
      RCC_HCLK_DIV1;


  if (HAL_RCC_ClockConfig(
          &RCC_ClkInitStruct,
          FLASH_LATENCY_0) != HAL_OK)
  {
      Error_Handler();
  }
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

  __disable_irq();

  while (1)
  {
  }

  /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

/**
  * @brief Reports the name of the source file and
  * source line number where assert_param error occurred.
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */

  /*
   * User can add his own implementation to report
   * the file name and line number.
   */

  /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
