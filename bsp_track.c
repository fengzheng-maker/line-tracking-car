#include "bsp_track.h"
#include "bsp_motor.h"
#include "car_config.h"

/*
 * Expansion board track sensor pins:
 * From right to left: PB0 PB1 PB10 PB11
 *
 * R2 = far right  -> PB0
 * R1 = right mid  -> PB1
 * L1 = left mid   -> PB10
 * L2 = far left   -> PB11
 *
 * Common infrared tracking module:
 * Black line = RESET / 0
 * White area = SET / 1
 */

#define TRACK_R2_GPIO_Port   GPIOB
#define TRACK_R2_Pin         GPIO_PIN_0

#define TRACK_R1_GPIO_Port   GPIOB
#define TRACK_R1_Pin         GPIO_PIN_1

#define TRACK_L1_GPIO_Port   GPIOB
#define TRACK_L1_Pin         GPIO_PIN_10

#define TRACK_L2_GPIO_Port   GPIOB
#define TRACK_L2_Pin         GPIO_PIN_11

#define TRACK_BLACK_LEVEL    GPIO_PIN_RESET

static uint8_t Track_IsBlack(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    return (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == TRACK_BLACK_LEVEL) ? 1 : 0;
}

void Track_Init(void)
{
    /*
     * GPIO is initialized by CubeMX.
     * Keep this function for unified module initialization.
     */
}

/*
 * Return 4-bit state:
 *
 * bit3 bit2 bit1 bit0
 * L2   L1   R1   R2
 *
 * Example:
 * 0x06 = 0110 = L1 and R1 are on black line, centered.
 */
uint8_t Track_ReadState(void)
{
    uint8_t state = 0;

    if (Track_IsBlack(TRACK_L2_GPIO_Port, TRACK_L2_Pin)) state |= 0x08;
    if (Track_IsBlack(TRACK_L1_GPIO_Port, TRACK_L1_Pin)) state |= 0x04;
    if (Track_IsBlack(TRACK_R1_GPIO_Port, TRACK_R1_Pin)) state |= 0x02;
    if (Track_IsBlack(TRACK_R2_GPIO_Port, TRACK_R2_Pin)) state |= 0x01;

    return state;
}

uint8_t Track_ReadSensors(void)
{
    return Track_ReadState();
}

const char* Track_StateToString(uint8_t sensor_val)
{
    switch (sensor_val)
    {
        case 0x00: return "0000 LOST";
        case 0x01: return "0001 R2";
        case 0x02: return "0010 R1";
        case 0x03: return "0011 R1R2";
        case 0x04: return "0100 L1";
        case 0x06: return "0110 CENTER";
        case 0x08: return "1000 L2";
        case 0x0C: return "1100 L2L1";
        case 0x0F: return "1111 ALL";
        default:   return "OTHER";
    }
}

void Track_Run(void)
{
    uint8_t state = Track_ReadState();

    switch (state)
    {
        /* Center: go straight */
        case 0x06:
            Motor_SetSpeed(TRACK_BASE_SPEED, TRACK_BASE_SPEED);
            break;

        /* Line shifts right: turn right slightly */
        case 0x02:
            Motor_SetSpeed(TRACK_TURN_FAST_SPEED, TRACK_TURN_SLOW_SPEED);
            break;

        /* Line shifts further right: turn right harder */
        case 0x03:
            Motor_SetSpeed(TRACK_TURN_FAST_SPEED, TRACK_REVERSE_SPEED);
            break;

        /* Far right detects line: sharp right */
        case 0x01:
            Motor_SetSpeed(TRACK_TURN_FAST_SPEED, -TRACK_TURN_FAST_SPEED);
            break;

        /* Line shifts left: turn left slightly */
        case 0x04:
            Motor_SetSpeed(TRACK_TURN_SLOW_SPEED, TRACK_TURN_FAST_SPEED);
            break;

        /* Line shifts further left: turn left harder */
        case 0x0C:
            Motor_SetSpeed(TRACK_REVERSE_SPEED, TRACK_TURN_FAST_SPEED);
            break;

        /* Far left detects line: sharp left */
        case 0x08:
            Motor_SetSpeed(-TRACK_TURN_FAST_SPEED, TRACK_TURN_FAST_SPEED);
            break;

        /* Crossroad or all sensors on black */
        case 0x0F:
            Motor_SetSpeed(TRACK_BASE_SPEED, TRACK_BASE_SPEED);
            break;

        /* Lost line */
        case 0x00:
            Motor_Stop();
            break;

        default:
            Motor_SetSpeed(TRACK_BASE_SPEED, TRACK_BASE_SPEED);
            break;
    }
}
