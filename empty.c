/**
 * 循迹小车 + OLED + 按键
 * BTN: PA21(圈数+) / PA22(开始)
 */
#include "ti_msp_dl_config.h"
#include "motor.h"
#include "tracking.h"
#include "oled.h"

#define IRR_SPEED  300
#define MAX_SPEED  800
#define KP  40.0f
#define KI  0.02f
#define KD  60.0f

static uint8_t s[8], X1,X2,X3,X4,X5,X6,X7,X8;
static float   g_int=0;
static int8_t  g_le=0;
static int8_t  lap_set=1, lap_done=0, turn_cnt=0;
static uint8_t started=0;

void delay_ms(uint32_t ms) {
    for(uint32_t i=0;i<ms;i++)
        for(uint32_t j=0;j<80000;j++) __asm("nop");
}
uint8_t btn(uint32_t pin) {
    if(!DL_GPIO_readPins(GPIOA,pin)){
        delay_ms(50);
        if(!DL_GPIO_readPins(GPIOA,pin)) return 1;
    }
    return 0;
}
static float pid(int8_t e){g_int+=e;if(g_int>100)g_int=100;else if(g_int<-100)g_int=-100;float o=e*KP+g_int*KI+(e-g_le)*KD;g_le=e;if(o>MAX_SPEED+200)o=MAX_SPEED+200;if(o<-MAX_SPEED-200)o=-MAX_SPEED-200;return o;}
static void mot(int16_t Vx,int16_t Vz){int16_t L=Vx-Vz,R=Vx+Vz;if(L>900)L=900;if(L<-900)L=-900;if(R>900)R=900;if(R<-900)R=-900;Motor_LeftSet(L);Motor_RightSet(R);}
static void cp(void){X1=s[0];X2=s[1];X3=s[2];X4=s[3];X5=s[4];X6=s[5];X7=s[6];X8=s[7];}

void show_select(void) {
    OLED_Clear();
    OLED_ShowStr(0,0,"Laps:");
    OLED_ShowNum(50,0,lap_set);
    OLED_ShowStr(0,2,"Press START");
}
void show_running(void) {
    OLED_Clear();
    OLED_ShowStr(0,0,"Run:");
    OLED_ShowNum(40,0,lap_done+1);
    OLED_ShowStr(0,2,"/");
    OLED_ShowNum(12,2,lap_set);
}

int main(void) {
    SYSCFG_DL_init();
    Motor_Init();
    OLED_Init();
    OLED_Clear();
    show_select();
    delay_ms(300);

    while(!started) {
        if(btn(DL_GPIO_PIN_21)){lap_set++;if(lap_set>5)lap_set=1;show_select();while(!DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_21));delay_ms(200);}
        if(btn(DL_GPIO_PIN_22)){started=1;lap_done=0;turn_cnt=0;g_int=0;g_le=0;show_running();while(!DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_22));delay_ms(500);break;}
    }

    while(started) {
        Tracking_ReadAll(s);cp();
        int po; int8_t e=0;
        if((X1==1||X2==1)&&(X4==1||X5==1)){
            for(int i=0;i<14;i++){mot(-50,200);delay_ms(10);}
            mot(IRR_SPEED,0);delay_ms(200);turn_cnt++;
            if(turn_cnt>=4){turn_cnt=0;lap_done++;show_running();}
            if(lap_done>=lap_set){started=0;Car_Stop();}continue;
        }
        else if(X4==1&&X5==1&&X1!=1&&X8!=1)e=0;
        else if(X4==1&&X5!=1)e=-2;else if(X4!=1&&X5==1)e=2;
        else if(X7==1||X8==1)e=6;else if(X1==1||X2==1)e=-6;
        else e=g_le;
        po=(int)pid(e);mot(IRR_SPEED,-po);delay_ms(5);
    }

    OLED_Clear();
    OLED_ShowStr(0,1,"DONE!");
    while(1);
}
