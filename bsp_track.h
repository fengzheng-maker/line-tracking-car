#ifndef __BSP_TRACK_H__
#define __BSP_TRACK_H__

#include "main.h"
#include <stdint.h>

void Track_Init(void);

uint8_t Track_ReadState(void);
uint8_t Track_ReadSensors(void);

const char* Track_StateToString(uint8_t sensor_val);

void Track_Run(void);

#endif
