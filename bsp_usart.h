#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "main.h"
#include <stdint.h>

void USART1_StartReceiveIT(void);
void USART1_SendByte(uint8_t byte);
void USART1_SendString(const char *str);
void USART1_Printf(const char *fmt, ...);

uint8_t USART1_ReadByte(uint8_t *byte);

#endif
