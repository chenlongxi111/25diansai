/**
 * @file  motor.c
 * @brief TB6612 双路电机驱动实现
 *
 * 左电机: PA26(PWM), PA14(IN1), PA15(IN2) — TIMG7-CC0
 * 右电机: PA27(PWM), PA13(IN1), PA12(IN2) — TIMG7-CC1
 * 使能:   PA7(STBY)
 */

#include "motor.h"
#include "ti_msp_dl_config.h"

/* ================================================================
 * 限幅
 * ================================================================ */
static inline uint16_t clamp_speed(uint16_t speed)
{
    return (speed > PWM_MAX) ? PWM_MAX : speed;
}

/* ================================================================
 * 公共接口
 * ================================================================ */

void Motor_Init(void)
{
    DL_TimerG_startCounter(MOTOR_PWM_INST);
    Motor_Standby();
    Motor_Enable();
}

void Motor_Enable(void)
{
    DL_GPIO_setPins(MOTOR_STBY_PORT, MOTOR_STBY_PIN);
}

void Motor_Standby(void)
{
    DL_GPIO_clearPins(MOTOR_STBY_PORT, MOTOR_STBY_PIN);
}

void Motor_LeftSet(int16_t speed)
{
    uint16_t abs_speed;

    if (speed > 0) {
        DL_GPIO_setPins(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN);
        DL_GPIO_clearPins(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN);
        abs_speed = (uint16_t)speed;
    } else if (speed < 0) {
        DL_GPIO_clearPins(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN);
        DL_GPIO_setPins(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN);
        abs_speed = (uint16_t)(-speed);
    } else {
        /* 刹车 */
        DL_GPIO_setPins(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN);
        DL_GPIO_setPins(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN);
        DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, 0, PWM_LEFT_CH);
        return;
    }

    abs_speed = clamp_speed(abs_speed);
    DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, PWM_MAX - abs_speed, PWM_LEFT_CH);
}

void Motor_RightSet(int16_t speed)
{
    uint16_t abs_speed;

    if (speed > 0) {
        DL_GPIO_setPins(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN);
        DL_GPIO_clearPins(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN);
        abs_speed = (uint16_t)speed;
    } else if (speed < 0) {
        DL_GPIO_clearPins(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN);
        DL_GPIO_setPins(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN);
        abs_speed = (uint16_t)(-speed);
    } else {
        /* 刹车 */
        DL_GPIO_setPins(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN);
        DL_GPIO_setPins(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN);
        DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, 0, PWM_RIGHT_CH);
        return;
    }

    abs_speed = clamp_speed(abs_speed);
    DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, PWM_MAX - abs_speed, PWM_RIGHT_CH);
}

void Motor_Set(int16_t left, int16_t right)
{
    Motor_LeftSet(left);
    Motor_RightSet(right);
}

/* ================================================================
 * 小车动作
 * ================================================================ */

void Car_Forward(uint16_t speed)
{
    speed = clamp_speed(speed);
    Motor_LeftSet((int16_t)speed);
    Motor_RightSet((int16_t)speed);
}

void Car_Backward(uint16_t speed)
{
    speed = clamp_speed(speed);
    Motor_LeftSet(-(int16_t)speed);
    Motor_RightSet(-(int16_t)speed);
}

void Car_TurnLeft(uint16_t speed)
{
    speed = clamp_speed(speed);
    Motor_LeftSet(-(int16_t)speed);
    Motor_RightSet((int16_t)speed);
}

void Car_TurnRight(uint16_t speed)
{
    speed = clamp_speed(speed);
    Motor_LeftSet((int16_t)speed);
    Motor_RightSet(-(int16_t)speed);
}

void Car_Stop(void)
{
    Motor_LeftSet(0);
    Motor_RightSet(0);
}

void Car_Coast(void)
{
    DL_GPIO_clearPins(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN);
    DL_GPIO_clearPins(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN);
    DL_GPIO_clearPins(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN);
    DL_GPIO_clearPins(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN);
    DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, 0, PWM_LEFT_CH);
    DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, 0, PWM_RIGHT_CH);
}
