#ifndef __BSP_PID_H__
#define __BSP_PID_H__

#include "main.h"

typedef struct
{
    float kp;
    float ki;
    float kd;

    float target;
    float measure;
    float error;
    float last_error;

    float integral;
    float output;

    float output_max;
    float output_min;

    float integral_max;
    float integral_min;
} PID_TypeDef;

void PID_Init(PID_TypeDef *pid,
              float kp, float ki, float kd,
              float output_min, float output_max,
              float integral_min, float integral_max);

void PID_Reset(PID_TypeDef *pid);

float PID_Calc(PID_TypeDef *pid, float target, float measure);

#endif
