/**
 * @file  motor.h
 * @brief TB6612 双路电机驱动模块接口
 *
 * TB6612 控制逻辑 (STBY=1时):
 *   IN1=1, IN2=0, PWM=x → 正转
 *   IN1=0, IN2=1, PWM=x → 反转
 *   IN1=1, IN2=1        → 刹车
 *   IN1=0, IN2=0        → 自由停
 *
 * 电机映射: 电机A=左电机, 电机B=右电机
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

/* ================================================================
 * GPIO 引脚定义 (使用 SysConfig 生成的宏名)
 * ================================================================ */

/* STBY (使能) — PA7 */
#define MOTOR_STBY_PORT   GPIO_GRP_1_PORT
#define MOTOR_STBY_PIN    GPIO_GRP_1_MOTOR_STBY_PIN

/* 左电机 (A路) — PA14/PA15 */
#define MOTOR_LEFT_IN1_PORT  GPIO_GRP_1_PORT
#define MOTOR_LEFT_IN1_PIN   GPIO_GRP_1_MOTOR_IN1_L_PIN
#define MOTOR_LEFT_IN2_PORT  GPIO_GRP_1_PORT
#define MOTOR_LEFT_IN2_PIN   GPIO_GRP_1_MOTOR_IN2_L_PIN

/* 右电机 (B路) — PA12/PA13 */
#define MOTOR_RIGHT_IN1_PORT GPIO_GRP_1_PORT
#define MOTOR_RIGHT_IN1_PIN  GPIO_GRP_1_MOTOR_IN1_R_PIN
#define MOTOR_RIGHT_IN2_PORT GPIO_GRP_1_PORT
#define MOTOR_RIGHT_IN2_PIN  GPIO_GRP_1_MOTOR_IN2_R_PIN

/* ================================================================
 * PWM — 左右共用 TIMG7
 * ================================================================ */
#define MOTOR_PWM_INST    PWM_MOTOR_INST

#define PWM_LEFT_CH       DL_TIMER_CC_0_INDEX   /* PA26, TIMG7-C0 */
#define PWM_RIGHT_CH      DL_TIMER_CC_1_INDEX   /* PA27, TIMG7-C1 */
#define PWM_MAX           1000

/* ================================================================
 * 函数接口
 * ================================================================ */

void Motor_Init(void);
void Motor_Enable(void);
void Motor_Standby(void);
void Motor_LeftSet(int16_t speed);
void Motor_RightSet(int16_t speed);
void Motor_Set(int16_t left, int16_t right);

void Car_Forward(uint16_t speed);
void Car_Backward(uint16_t speed);
void Car_TurnLeft(uint16_t speed);
void Car_TurnRight(uint16_t speed);
void Car_Stop(void);
void Car_Coast(void);

#endif /* MOTOR_H */
