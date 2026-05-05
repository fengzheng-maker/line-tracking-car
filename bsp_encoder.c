#include "bsp_encoder.h"

extern TIM_HandleTypeDef htim2;

/*
 * Expansion board encoder plan:
 *
 * Left encoder:
 * A -> PA15 / TIM2_CH1
 * B -> PB3  / TIM2_CH2
 * Use TIM2 hardware encoder mode.
 *
 * Right encoder:
 * A -> PA2 / EXTI2
 * B -> PA3 / GPIO_Input
 * Use software counting.
 *
 * CubeMX must set:
 * SYS Debug -> Serial Wire
 * so PA15 and PB3 are released from JTAG.
 */

#define ENC_RIGHT_A_GPIO_Port   GPIOA
#define ENC_RIGHT_A_Pin         GPIO_PIN_2

#define ENC_RIGHT_B_GPIO_Port   GPIOA
#define ENC_RIGHT_B_Pin         GPIO_PIN_3

static int16_t left_last_count = 0;

static volatile int32_t right_count = 0;
static int32_t right_last_count = 0;

void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

    __HAL_TIM_SET_COUNTER(&htim2, 0);
    left_last_count = 0;

    right_count = 0;
    right_last_count = 0;
}

void Encoder_Clear(void)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    left_last_count = 0;

    right_count = 0;
    right_last_count = 0;
}

int16_t Encoder_Left_GetCount(void)
{
    return (int16_t)__HAL_TIM_GET_COUNTER(&htim2);
}

int16_t Encoder_Right_GetCount(void)
{
    return (int16_t)right_count;
}

int16_t Encoder_Left_GetDiff(void)
{
    int16_t now;
    int16_t diff;

    now = (int16_t)__HAL_TIM_GET_COUNTER(&htim2);
    diff = now - left_last_count;
    left_last_count = now;

    return diff;
}

int16_t Encoder_Right_GetDiff(void)
{
    int32_t now;
    int32_t diff;

    now = right_count;
    diff = now - right_last_count;
    right_last_count = now;

    return (int16_t)diff;
}

/*
 * Right encoder software counting.
 *
 * PA2: EXTI rising/falling edge
 * PA3: normal GPIO input
 *
 * If the right encoder direction is reversed during real testing,
 * swap right_count++ and right_count--.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == ENC_RIGHT_A_Pin)
    {
        GPIO_PinState a_state;
        GPIO_PinState b_state;

        a_state = HAL_GPIO_ReadPin(ENC_RIGHT_A_GPIO_Port, ENC_RIGHT_A_Pin);
        b_state = HAL_GPIO_ReadPin(ENC_RIGHT_B_GPIO_Port, ENC_RIGHT_B_Pin);

        if (a_state == b_state)
        {
            right_count++;
        }
        else
        {
            right_count--;
        }
    }
}
