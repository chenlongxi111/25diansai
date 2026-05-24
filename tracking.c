/**
 * CD4051 8路 — 完全匹配官方 bsp_ir_eight.c
 * BLACK=1, WHITE=0
 */
#include "tracking.h"
#include "ti_msp_dl_config.h"

static void gpio_set(GPIO_Regs* g, uint32_t p, uint8_t v) {
    if(v) DL_GPIO_setPins(g, p); else DL_GPIO_clearPins(g, p);
}

static void dly_ms(uint32_t ms) {
    for(uint32_t i=0;i<ms;i++)
        for(uint32_t j=0;j<80000;j++) __asm("nop");
}

static void set_ch(uint8_t c, uint8_t b, uint8_t a) {
    gpio_set(GPIOB, DL_GPIO_PIN_0,  c);
    gpio_set(GPIOB, DL_GPIO_PIN_27, b);
    gpio_set(GPIOB, DL_GPIO_PIN_26, a);
}

void Tracking_ReadAll(uint8_t s[8]) {
    for(int ch=0;ch<8;ch++) {
        set_ch((ch>>2)&1, (ch>>1)&1, ch&1);
        dly_ms(1);
        s[ch] = DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_1) ? 1 : 0;
    }
}
