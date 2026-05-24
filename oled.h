#ifndef OLED_H
#define OLED_H
#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowStr(uint8_t x, uint8_t y, const char *s);
void OLED_ShowNum(uint8_t x, uint8_t y, int n);
#endif
