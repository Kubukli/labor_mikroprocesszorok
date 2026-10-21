
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Heart shape generation using STM32 DAC
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dac.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdint.h>
#include "OLED.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define HEART_SAMPLES 400U
#define PI 3.14159265358979323846f
#define DAC_MAX 4095U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

uint32_t heart_x[HEART_SAMPLES];
uint32_t heart_y[HEART_SAMPLES];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

void GenerateHeart(uint32_t *x_data,
                   uint32_t *y_data,
                   uint32_t length);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void GenerateHeart(uint32_t *x_data,
                   uint32_t *y_data,
                   uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        float t = 2.0f * PI * (float)i / (float)length;

        // Heart X coordinate
        float x = 16.0f * powf(sinf(t), 3.0f);

        // Heart Y coordinate
        float y = 13.0f * cosf(t)
                - 5.0f * cosf(2.0f * t)
                - 2.0f * cosf(3.0f * t)
                - cosf(4.0f * t);

        // Normalize X from [-16, 16] to [0, 4095]
        float x_norm = (x + 16.0f) / 32.0f;

        // Normalize Y approximately from [-17, 12] to [0, 4095]
        float y_norm = (y + 17.0f) / 29.0f;

        if (x_norm < 0.0f) x_norm = 0.0f;
        if (x_norm > 1.0f) x_norm = 1.0f;

        if (y_norm < 0.0f) y_norm = 0.0f;
        if (y_norm > 1.0f) y_norm = 1.0f;

        x_data[i] = (uint32_t)(x_norm * DAC_MAX);
        y_data[i] = (uint32_t)(y_norm * DAC_MAX);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    uint32_t index = 0;
    uint32_t previous_tick = 0;

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    HAL_Init();

    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    /* USER CODE END SysInit */

    MX_GPIO_Init();
    MX_DAC_Init();
    MX_I2C2_Init();

    /* USER CODE BEGIN 2 */

    // Generate heart coordinates
    GenerateHeart(heart_x, heart_y, HEART_SAMPLES);

    // Initialize DAC Channel 1
    HAL_DAC_SetValue(&hdac,
                     DAC_CHANNEL_1,
                     DAC_ALIGN_12B_R,
                     heart_x[0]);

    // Initialize DAC Channel 2
    HAL_DAC_SetValue(&hdac,
                     DAC_CHANNEL_2,
                     DAC_ALIGN_12B_R,
                     heart_y[0]);

    // Start both DAC channels
    if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_DAC_Start(&hdac, DAC_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    previous_tick = HAL_GetTick();

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
    {
        uint32_t current_tick = HAL_GetTick();

        if ((uint32_t)(current_tick - previous_tick) >= 1U)
        {
            previous_tick += 1U;

            HAL_DAC_SetValue(&hdac,
                             DAC_CHANNEL_1,
                             DAC_ALIGN_12B_R,
                             heart_x[index]);

            HAL_DAC_SetValue(&hdac,
                             DAC_CHANNEL_2,
                             DAC_ALIGN_12B_R,
                             heart_y[index]);

            index++;

            if (index >= HEART_SAMPLES)
            {
                index = 0;
            }
        }

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

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    RCC_OscInitStruct.PLL.PLLR = 2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                            FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief Error Handler
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

void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* USER CODE END 6 */
}

#endif
