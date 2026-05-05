/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_car.h"
#include "bsp_track.h"
#include "bsp_usart.h"
#include "car_config.h"
#include "bsp_encoder.h"
#include "bsp_ultrasonic.h"
#include "bsp_motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
    APP_MODE_MANUAL = 0,
    APP_MODE_TRACK,
    APP_MODE_ENCODER_TEST,
    APP_MODE_PID_TEST
} App_Mode_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
static App_Mode_t app_mode = APP_MODE_MANUAL;
static uint32_t observe_tick = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);

/* USER CODE BEGIN PFP */
static void App_HandleCmd(uint8_t ch);
static void App_PrintHelp(void);
static void App_PrintStatus(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void App_PrintHelp(void)
{
    USART1_Printf("\r\n===== CMD LIST =====\r\n");
    USART1_Printf("w : forward\r\n");
    USART1_Printf("s : backward\r\n");
    USART1_Printf("a : turn left\r\n");
    USART1_Printf("d : turn right\r\n");
    USART1_Printf("x : stop\r\n");
    USART1_Printf("t : track mode\r\n");
    USART1_Printf("e : encoder test mode\r\n");
    USART1_Printf("p : pid test mode\r\n");
    USART1_Printf("r : reset encoder count\r\n");
    USART1_Printf("u : ultrasonic distance\r\n");
    USART1_Printf("i : print status\r\n");
    USART1_Printf("h : help\r\n");
    USART1_Printf("====================\r\n");
}

static void App_PrintStatus(void)
{
    uint8_t track_state;

    track_state = Track_ReadSensors();

    USART1_Printf("app_mode=%d\r\n", app_mode);
    USART1_Printf("L_cnt=%d R_cnt=%d\r\n",
                  Encoder_Left_GetCount(),
                  Encoder_Right_GetCount());
    USART1_Printf("track=0x%02X %s\r\n",
                  track_state,
                  Track_StateToString(track_state));
}

static void App_HandleCmd(uint8_t ch)
{
    switch (ch)
    {
        case 'w':
            app_mode = APP_MODE_MANUAL;
            Motor_SetSpeed(CAR_MANUAL_SPEED_DEFAULT, CAR_MANUAL_SPEED_DEFAULT);
            USART1_Printf("cmd=w forward\r\n");
            break;

        case 's':
            app_mode = APP_MODE_MANUAL;
            Motor_SetSpeed(-CAR_MANUAL_SPEED_DEFAULT, -CAR_MANUAL_SPEED_DEFAULT);
            USART1_Printf("cmd=s backward\r\n");
            break;

        case 'a':
            app_mode = APP_MODE_MANUAL;
            Motor_SetSpeed(-CAR_MANUAL_SPEED_DEFAULT, CAR_MANUAL_SPEED_DEFAULT);
            USART1_Printf("cmd=a turn left\r\n");
            break;

        case 'd':
            app_mode = APP_MODE_MANUAL;
            Motor_SetSpeed(CAR_MANUAL_SPEED_DEFAULT, -CAR_MANUAL_SPEED_DEFAULT);
            USART1_Printf("cmd=d turn right\r\n");
            break;

        case 'x':
            app_mode = APP_MODE_MANUAL;
            Motor_Stop();
            USART1_Printf("cmd=x stop\r\n");
            break;

        case 't':
            app_mode = APP_MODE_TRACK;
            Car_StartTrack();
            observe_tick = HAL_GetTick();
            USART1_Printf("cmd=t track mode\r\n");
            break;

        case 'e':
            app_mode = APP_MODE_ENCODER_TEST;
            Motor_Stop();
            Encoder_Clear();
            observe_tick = HAL_GetTick();
            USART1_Printf("cmd=e encoder test mode\r\n");
            break;

        case 'p':
            app_mode = APP_MODE_PID_TEST;
            Motor_Stop();
            Encoder_Clear();
            observe_tick = HAL_GetTick();
            USART1_Printf("cmd=p pid test mode\r\n");
            break;

        case 'r':
            Encoder_Clear();
            USART1_Printf("cmd=r encoder reset\r\n");
            break;

        case 'u':
        {
            float dis;
            int16_t dis10;

            dis = Ultrasonic_GetDistanceCm();

            if (dis < 0.0f)
            {
                USART1_Printf("ultrasonic timeout\r\n");
            }
            else
            {
                dis10 = (int16_t)(dis * 10.0f);
                USART1_Printf("distance=%d.%d cm\r\n",
                              dis10 / 10,
                              dis10 % 10);
            }
        }
        break;

        case 'i':
            App_PrintStatus();
            break;

        case 'h':
            App_PrintHelp();
            break;

        default:
            USART1_Printf("unknown cmd: %c\r\n", ch);
            break;
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

  /* USER CODE END 1 */

  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  Car_Init();
  Encoder_Init();
  Ultrasonic_Init();
  USART1_StartReceiveIT();

  app_mode = APP_MODE_MANUAL;
  observe_tick = HAL_GetTick();

  Motor_Stop();

  USART1_Printf("\r\nboot ok expansion board\r\n");
  App_PrintHelp();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint8_t ch;
    uint8_t track_state;
    int16_t left_cnt;
    int16_t right_cnt;
    int16_t left_diff;
    int16_t right_diff;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (USART1_ReadByte(&ch))
    {
        App_HandleCmd(ch);
    }

    switch (app_mode)
    {
        case APP_MODE_MANUAL:
            break;

        case APP_MODE_TRACK:
            Car_Task();

            if (HAL_GetTick() - observe_tick >= OBSERVE_PERIOD_MS)
            {
                observe_tick = HAL_GetTick();
                track_state = Track_ReadSensors();

                USART1_Printf("TRACK state=0x%02X %s\r\n",
                              track_state,
                              Track_StateToString(track_state));
            }
            break;

        case APP_MODE_ENCODER_TEST:
            if (HAL_GetTick() - observe_tick >= OBSERVE_PERIOD_MS)
            {
                observe_tick = HAL_GetTick();

                left_cnt   = Encoder_Left_GetCount();
                right_cnt  = Encoder_Right_GetCount();
                left_diff  = Encoder_Left_GetDiff();
                right_diff = Encoder_Right_GetDiff();

                USART1_Printf("ENC L_cnt=%d L_diff=%d | R_cnt=%d R_diff=%d\r\n",
                              left_cnt,
                              left_diff,
                              right_cnt,
                              right_diff);
            }
            break;

        case APP_MODE_PID_TEST:
            if (HAL_GetTick() - observe_tick >= OBSERVE_PERIOD_MS)
            {
                observe_tick = HAL_GetTick();

                left_cnt   = Encoder_Left_GetCount();
                right_cnt  = Encoder_Right_GetCount();
                left_diff  = Encoder_Left_GetDiff();
                right_diff = Encoder_Right_GetDiff();

                USART1_Printf("PID TEST L_cnt=%d L_diff=%d | R_cnt=%d R_diff=%d\r\n",
                              left_cnt,
                              left_diff,
                              right_cnt,
                              right_diff);
            }
            break;

        default:
            break;
    }

    HAL_Delay(CAR_TASK_PERIOD_MS);
    /* USER CODE END 3 */
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;

  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;

  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;

  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim3);
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  *
  * Current SystemClock uses HSI 8MHz.
  * Prescaler = 7 gives TIM4 counter 1MHz, 1 tick = 1us.
  *
  * If CubeMX later changes system clock to 72MHz,
  * set Prescaler to 71.
  */
static void MX_TIM4_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  /*
   * Important:
   * CubeMX later must set SYS Debug to Serial Wire.
   * This releases PA15 and PB3 from JTAG for TIM2 encoder remap.
   */

  /* Output default level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,
                    GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8,
                    GPIO_PIN_RESET);

  /* PA4: HC-SR04 TRIG */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* PA5: HC-SR04 ECHO */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* PA2: right encoder A, EXTI */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* PA3: right encoder B */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* PB5 PB6 PB7 PB8: TB6612 direction pins */
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* PB0 PB1 PB10 PB11: four tracking sensors */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI2 interrupt for right encoder A */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */

  /* USER CODE END 6 */
}
#endif
