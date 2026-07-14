#include "STC15Fxxxx.H"
#include "delay.h"
#include "servo.h"
#include "../app/lock.h"

/* Timer0 中断：给舵机喂 100 µs 节拍 */
void timer0_isr(void) interrupt 1
{
    servo_tick();
}

void main(void)
{
    /* 关看门狗，学生板子上没接 */
    WDT_CONTR = 0x00;

    servo_init();
    EA = 1;             /* 开总中断，舵机 PWM 靠 Timer0 */

    lock_init();

    while (1) {
        lock_poll();
    }
}
