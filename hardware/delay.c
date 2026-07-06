#include "STC15Fxxxx.H"
#include "intrins.h"
#include "delay.h"

/* 12 MHz、1T 模式下，delay_us 用 4 个 nop 校准过一次。
 * 精确时序（I2C、单总线）都走这个函数，不要用 for 空循环。
 */
void delay_us(unsigned int us)
{
    unsigned int i;
    for (i = 0; i < us; i++) {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    while (ms--) {
        for (i = 0; i < 250; i++) {
            delay_us(4);
        }
    }
}
