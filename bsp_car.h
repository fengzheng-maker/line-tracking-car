#ifndef __BSP_CAR_H__
#define __BSP_CAR_H__

#include "main.h"
#include <stdint.h>

typedef enum
{
    CAR_MODE_STOP = 0,
    CAR_MODE_MANUAL,
    CAR_MODE_TRACK
} Car_Mode_t;

typedef enum
{
    CAR_CMD_STOP = 0,
    CAR_CMD_FORWARD,
    CAR_CMD_BACKWARD,
    CAR_CMD_TURN_LEFT,
    CAR_CMD_TURN_RIGHT
} Car_Cmd_t;

void Car_Init(void);
void Car_Task(void);

void Car_SetMode(Car_Mode_t mode);
Car_Mode_t Car_GetMode(void);

void Car_Stop(void);
void Car_Forward(uint16_t speed);
void Car_Backward(uint16_t speed);
void Car_TurnLeft(uint16_t speed);
void Car_TurnRight(uint16_t speed);

void Car_StartTrack(void);

#endif

