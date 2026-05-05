#include "bsp_usart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;

static uint8_t usart1_rx_byte = 0;
static volatile uint8_t usart1_rx_flag = 0;
static volatile uint8_t usart1_rx_data = 0;

void USART1_StartReceiveIT(void)
{
    HAL_UART_Receive_IT(&huart1, &usart1_rx_byte, 1);
}

void USART1_SendByte(uint8_t byte)
{
    HAL_UART_Transmit(&huart1, &byte, 1, 100);
}

void USART1_SendString(const char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 100);
}

void USART1_Printf(const char *fmt, ...)
{
    char buf[128];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    USART1_SendString(buf);
}

uint8_t USART1_ReadByte(uint8_t *byte)
{
    if (usart1_rx_flag == 0)
    {
        return 0;
    }

    *byte = usart1_rx_data;
    usart1_rx_flag = 0;
    return 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        usart1_rx_data = usart1_rx_byte;
        usart1_rx_flag = 1;

        HAL_UART_Receive_IT(&huart1, &usart1_rx_byte, 1);
    }
}
