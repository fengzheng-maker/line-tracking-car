#include "bsp_pid.h"

static float PID_Limit(float value, float min, float max)
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

void PID_Init(PID_TypeDef *pid,
              float kp, float ki, float kd,
              float output_min, float output_max,
              float integral_min, float integral_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->target = 0.0f;
    pid->measure = 0.0f;
    pid->error = 0.0f;
    pid->last_error = 0.0f;

    pid->integral = 0.0f;
    pid->output = 0.0f;

    pid->output_min = output_min;
    pid->output_max = output_max;

    pid->integral_min = integral_min;
    pid->integral_max = integral_max;
}

void PID_Reset(PID_TypeDef *pid)
{
    pid->target = 0.0f;
    pid->measure = 0.0f;
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
}

float PID_Calc(PID_TypeDef *pid, float target, float measure)
{
    float derivative;

    pid->target = target;
    pid->measure = measure;

    pid->error = pid->target - pid->measure;

    pid->integral += pid->error;
    pid->integral = PID_Limit(pid->integral, pid->integral_min, pid->integral_max);

    derivative = pid->error - pid->last_error;

    pid->output = pid->kp * pid->error
                + pid->ki * pid->integral
                + pid->kd * derivative;

    pid->output = PID_Limit(pid->output, pid->output_min, pid->output_max);

    pid->last_error = pid->error;

    return pid->output;
}

