# NUEDC 2025 E题 — 简易自行瞄准装置

MSPM0G3507 循迹小车 + OpenMV H7 视觉瞄准。沿 100cm 正方形黑线自动行驶，二维云台蓝紫激光实时瞄准靶心。

## 硬件

| 模块 | 型号 |
|------|------|
| 主控 | 立创天猛星 MSPM0G3507 |
| 电机 | TB6612 + 直流减速电机 ×2 |
| 循迹 | 亚博 8 路灰度（CD4051 多路复用）|
| 显示 | SSD1306 OLED 128×32（软件 I2C）|
| 按键 | 圈数选择 + 开始 |
| 视觉 | OpenMV H7 + 二维云台 |
| 激光 | 405nm / ≤10mW |

## 引脚分配

**TB6612 → MSPM0：**

| TB6612 | 引脚 | 功能 |
|--------|------|------|
| PWMA | PA26 | 左 PWM |
| AIN1 | PA14 | 左方向 1 |
| AIN2 | PA15 | 左方向 2 |
| PWMB | PA27 | 右 PWM |
| BIN1 | PA13 | 右方向 1 |
| BIN2 | PA12 | 右方向 2 |
| STBY | PA7 | 使能 |

**亚博 8 路灰度 → MSPM0：**

| 模块 | 引脚 | 方向 |
|------|------|------|
| AD0 | PB26 | 输出（地址 bit0）|
| AD1 | PB27 | 输出（地址 bit1）|
| AD2 | PB0 | 输出（地址 bit2）|
| OUT | PB1 | 输入（数据）|

**OLED → MSPM0：**

| OLED | 引脚 |
|------|------|
| SCL | PA11 |
| SDA | PA10 |

**按键 → MSPM0：**

| 功能 | 引脚 | 接法 |
|------|------|------|
| 圈数+ | PA21 | 按键 → GND |
| 开始 | PA22 | 按键 → GND |

## 编译

- CCS Theia + MSPM0 SDK 2.05.01.00
- 保存 `.syscfg` 自动生成配置
- 工程需添加 `oled.c`（右键 → Add Files）

## 参数

```c
#define IRR_SPEED  300   // 直行速度
#define KP  40.0f        // PID
#define KI  0.02f
#define KD  60.0f
```

转弯：`for(int i=0;i<14;i++)` 调转角，`delay_ms(200)` 调回正时间。

## 文件

| 文件 | 说明 |
|------|------|
| `empty.c` | 主程序 |
| `motor.c/h` | TB6612 电机 |
| `tracking.c/h` | CD4051 传感器 |
| `oled.c/h` | OLED |
| `empty.syscfg` | SysConfig |
| `main.py` | OpenMV 瞄准 |

## 问题记录

1. **CD4051 通道切换**：AD 脚必须设 OUTPUT，切换后等 1ms 再读 OUT
2. **PWM 极性**：CC 值对应低电平时长，需取反 `PWM_MAX - speed`
3. **转弯不稳**：电池掉电导致转角不准，加传感器闭环微调
4. **OpenMV 闪跳**：放宽角度容差 + 四点平均增强中心稳定
