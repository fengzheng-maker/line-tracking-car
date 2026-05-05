#ifndef __BSP_ENCODER_H__
#define __BSP_ENCODER_H__

#include "main.h"
#include <stdint.h>

void Encoder_Init(void);
void Encoder_Clear(void);

int16_t Encoder_Left_GetCount(void);
int16_t Encoder_Right_GetCount(void);

int16_t Encoder_Left_GetDiff(void);
int16_t Encoder_Right_GetDiff(void);

#endif
