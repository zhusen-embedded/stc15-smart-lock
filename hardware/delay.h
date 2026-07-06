#ifndef __DELAY_H
#define __DELAY_H

#include "STC15Fxxxx.H"

/* 12 MHz 晶振下的粗略延时。
 * STC15 是 1T 内核，一条指令约 83 ns，这里按循环次数折算。
 */

void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

#endif
