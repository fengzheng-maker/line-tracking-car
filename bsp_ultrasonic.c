#include "bsp_ultrasonic.h"

extern TIM_HandleTypeDef htim4;

/*
 * Expansion board HC-SR04 pins:
 *
 * TRIG -> PA4
 * ECHO -> PA5
 *
 * TIM4 is used as a 1us counter.
 */

#define ULTRASONIC_TRIG_GPIO_Port    GPIOA
#define ULTRASONIC_TRIG_Pin          GPIO_PIN_4

#define ULTRASONIC_ECHO_GPIO_Port    GPIOA
#define ULTRASONIC_ECHO_Pin          GPIO_PIN_5

static void Ultrasonic_DelayUs(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    while (__HAL_TIM_GET_COUNTER(&htim4) < us);
}

void Ultrasonic_Init(void)
{
    HAL_TIM_Base_Start(&htim4);

    HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port,
                      ULTRASONIC_TRIG_Pin,
                      GPIO_PIN_RESET);
}

float Ultrasonic_GetDistanceCm(void)
{
    uint32_t timeout;
    uint32_t echo_time_us;

    /* Make sure TRIG starts low */
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port,
                      ULTRASONIC_TRIG_Pin,
                      GPIO_PIN_RESET);
    Ultrasonic_DelayUs(2);

    /* Send a trigger pulse longer than 10us */
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port,
                      ULTRASONIC_TRIG_Pin,
                      GPIO_PIN_SET);
    Ultrasonic_DelayUs(15);
    HAL_GPIO_WritePin(ULTRASONIC_TRIG_GPIO_Port,
                      ULTRASONIC_TRIG_Pin,
                      GPIO_PIN_RESET);

    /* Wait for ECHO rising edge */
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    while (HAL_GPIO_ReadPin(ULTRASONIC_ECHO_GPIO_Port,
                            ULTRASONIC_ECHO_Pin) == GPIO_PIN_RESET)
    {
        timeout = __HAL_TIM_GET_COUNTER(&htim4);

        if (timeout > 30000)
        {
            return -1.0f;
        }
    }

    /* Measure ECHO high level time */
    __HAL_TIM_SET_COUNTER(&htim4, 0);

    while (HAL_GPIO_ReadPin(ULTRASONIC_ECHO_GPIO_Port,
                            ULTRASONIC_ECHO_Pin) == GPIO_PIN_SET)
    {
        echo_time_us = __HAL_TIM_GET_COUNTER(&htim4);

        if (echo_time_us > 30000)
        {
            return -1.0f;
        }
    }

    echo_time_us = __HAL_TIM_GET_COUNTER(&htim4);

    /*
     * Sound speed: about 343m/s = 0.0343cm/us
     * Distance = echo_time * 0.0343 / 2
     */
    return echo_time_us * 0.0343f / 2.0f;
}
