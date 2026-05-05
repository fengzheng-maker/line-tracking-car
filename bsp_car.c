#include "bsp_car.h"
#include "bsp_motor.h"
#include "bsp_track.h"
#include "bsp_pid.h"

static PID_TypeDef pid_left;
static PID_TypeDef pid_right;
static Car_Mode_t car_mode = CAR_MODE_STOP;
static Car_Cmd_t  car_cmd  = CAR_CMD_STOP;
static uint16_t   car_speed = 400;

void Car_Init(void)
{
    Motor_Init();
    Track_Init();

    car_mode = CAR_MODE_STOP;
    car_cmd  = CAR_CMD_STOP;
    car_speed = 400;
	
		PID_Init(&pid_left,  1.0f, 0.0f, 0.0f, -999.0f, 999.0f, -300.0f, 300.0f);
    PID_Init(&pid_right, 1.0f, 0.0f, 0.0f, -999.0f, 999.0f, -300.0f, 300.0f);
	
    Motor_Stop();
}

void Car_SetMode(Car_Mode_t mode)
{
    car_mode = mode;

    if (car_mode == CAR_MODE_STOP)
    {
        car_cmd = CAR_CMD_STOP;
        Motor_Stop();
    }
}

Car_Mode_t Car_GetMode(void)
{
    return car_mode;
}

void Car_Stop(void)
{
    car_mode = CAR_MODE_MANUAL;
    car_cmd = CAR_CMD_STOP;
}

void Car_Forward(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX)
    {
        speed = MOTOR_PWM_MAX;
    }

    car_speed = speed;
    car_mode = CAR_MODE_MANUAL;
    car_cmd = CAR_CMD_FORWARD;
}

void Car_Backward(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX)
    {
        speed = MOTOR_PWM_MAX;
    }

    car_speed = speed;
    car_mode = CAR_MODE_MANUAL;
    car_cmd = CAR_CMD_BACKWARD;
}

void Car_TurnLeft(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX)
    {
        speed = MOTOR_PWM_MAX;
    }

    car_speed = speed;
    car_mode = CAR_MODE_MANUAL;
    car_cmd = CAR_CMD_TURN_LEFT;
}

void Car_TurnRight(uint16_t speed)
{
    if (speed > MOTOR_PWM_MAX)
    {
        speed = MOTOR_PWM_MAX;
    }

    car_speed = speed;
    car_mode = CAR_MODE_MANUAL;
    car_cmd = CAR_CMD_TURN_RIGHT;
}

void Car_StartTrack(void)
{
    car_mode = CAR_MODE_TRACK;
}

void Car_Task(void)
{
    switch (car_mode)
    {
        case CAR_MODE_STOP:
            Motor_Stop();
            break;

        case CAR_MODE_MANUAL:
            switch (car_cmd)
            {
                case CAR_CMD_STOP:
                    Motor_Stop();
                    break;

                case CAR_CMD_FORWARD:
                    Motor_Forward(car_speed);
                    break;

                case CAR_CMD_BACKWARD:
                    Motor_Backward(car_speed);
                    break;

                case CAR_CMD_TURN_LEFT:
                    Motor_TurnLeft(car_speed);
                    break;

                case CAR_CMD_TURN_RIGHT:
                    Motor_TurnRight(car_speed);
                    break;

                default:
                    Motor_Stop();
                    break;
            }
            break;

        case CAR_MODE_TRACK:
            Track_Run();
            break;

        default:
            Motor_Stop();
            break;
    }
}
