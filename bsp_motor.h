#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#include "main.h"
#include <stdint.h>

#define MOTOR_PWM_MAX 999

void Motor_Init(void);
void Motor_Enable(void);
void Motor_Disable(void);
void Motor_Stop(void);
void Motor_SetSpeed(int16_t left_speed, int16_t right_speed);

void Motor_Forward(uint16_t speed);
void Motor_Backward(uint16_t speed);
void Motor_TurnLeft(uint16_t speed);
void Motor_TurnRight(uint16_t speed);

#endif

