#include "bsp_motor.h"

extern TIM_HandleTypeDef htim3;

/*
 * Expansion board TB6612 pins
 *
 * Left motor:
 * PWMA -> PA7 -> TIM3_CH2
 * AIN1 -> PB6
 * AIN2 -> PB5
 *
 * Right motor:
 * PWMB -> PA6 -> TIM3_CH1
 * BIN1 -> PB7
 * BIN2 -> PB8
 *
 * STBY -> connected to 5V on expansion board
 */

/* Left motor A */
#define AIN1_GPIO_Port    GPIOB
#define AIN1_Pin          GPIO_PIN_6
#define AIN2_GPIO_Port    GPIOB
#define AIN2_Pin          GPIO_PIN_5
#define PWMA_TIM_CHANNEL  TIM_CHANNEL_2

/* Right motor B */
#define BIN1_GPIO_Port    GPIOB
#define BIN1_Pin          GPIO_PIN_7
#define BIN2_GPIO_Port    GPIOB
#define BIN2_Pin          GPIO_PIN_8
#define PWMB_TIM_CHANNEL  TIM_CHANNEL_1

static int16_t Motor_Limit(int16_t speed)
{
    if (speed > MOTOR_PWM_MAX)  return MOTOR_PWM_MAX;
    if (speed < -MOTOR_PWM_MAX) return -MOTOR_PWM_MAX;
    return speed;
}

static void Motor_SetLeft(int16_t speed)
{
    speed = Motor_Limit(speed);

    if (speed > 0)
    {
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, PWMA_TIM_CHANNEL, speed);
    }
    else if (speed < 0)
    {
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, PWMA_TIM_CHANNEL, -speed);
    }
    else
    {
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, PWMA_TIM_CHANNEL, 0);
    }
}

static void Motor_SetRight(int16_t speed)
{
    speed = Motor_Limit(speed);

    if (speed > 0)
    {
        HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, PWMB_TIM_CHANNEL, speed);
    }
    else if (speed < 0)
    {
        HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim3, PWMB_TIM_CHANNEL, -speed);
    }
    else
    {
        HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim3, PWMB_TIM_CHANNEL, 0);
    }
}

void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, PWMA_TIM_CHANNEL);
    HAL_TIM_PWM_Start(&htim3, PWMB_TIM_CHANNEL);

    Motor_Enable();
    Motor_Stop();
}

void Motor_Enable(void)
{
    /*
     * STBY is connected to 5V on the expansion board.
     * No GPIO control is needed.
     */
}

void Motor_Disable(void)
{
    /*
     * STBY cannot be disabled by software on this board.
     * Stop motors instead.
     */
    Motor_Stop();
}

void Motor_Stop(void)
{
    Motor_SetLeft(0);
    Motor_SetRight(0);
}

void Motor_SetSpeed(int16_t left_speed, int16_t right_speed)
{
    Motor_SetLeft(left_speed);
    Motor_SetRight(right_speed);
}

void Motor_Forward(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX) speed = MOTOR_PWM_MAX;
    Motor_SetSpeed(speed, speed);
}

void Motor_Backward(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX) speed = MOTOR_PWM_MAX;
    Motor_SetSpeed(-speed, -speed);
}

void Motor_TurnLeft(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX) speed = MOTOR_PWM_MAX;
    Motor_SetSpeed(-speed, speed);
}

void Motor_TurnRight(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX) speed = MOTOR_PWM_MAX;
    Motor_SetSpeed(speed, -speed);
}
