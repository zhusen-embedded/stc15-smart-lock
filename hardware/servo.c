#include "servo.h"
#include "delay.h"
#include "STC15Fxxxx.H"

#define SERVO_PIN   P21

static unsigned char s_high_tick = SERVO_MIN_TICK;
static unsigned char s_count = 0;

/* Timer0 定时 100 µs @ 12 MHz 1T：
 * 12 MHz / 1T → 1 µs 一个计数，100 µs 需要 100 个计数。
 * 16 位自动重载，重载值 = 65536 - 100 = 0xFF9C
 */
#define T0_RELOAD_H     0xFF
#define T0_RELOAD_L     0x9C

void servo_init(void)
{
    /* P2.1 推挽输出，驱动能力强一些 */
    P2M1 &= ~0x02;
    P2M0 |= 0x02;
    SERVO_PIN = 0;

    /* Timer0 工作在 16 位自动重载模式（模式 0） */
    TMOD &= 0xF0;
    TMOD |= 0x00;
    TH0 = T0_RELOAD_H;
    TL0 = T0_RELOAD_L;
    ET0 = 1;
    TR0 = 1;
}

void servo_set_angle(unsigned char deg)
{
    unsigned int tick;

    if (deg > 180) deg = 180;

    /* 线性映射：0 度 → 5 拍，180 度 → 25 拍 */
    tick = SERVO_MIN_TICK + (unsigned int)deg * (SERVO_MAX_TICK - SERVO_MIN_TICK) / 180;
    s_high_tick = (unsigned char)tick;
}

void servo_tick(void)
{
    s_count++;
    if (s_count >= SERVO_PERIOD_TICK) {
        s_count = 0;
    }

    /* 计数值小于高电平拍数就输出高，否则拉低 */
    SERVO_PIN = (s_count < s_high_tick) ? 1 : 0;
}

void servo_unlock(void)
{
    /* 舵机转速有限，转到位需要时间，这里给足 500 ms */
    servo_set_angle(90);
    delay_ms(500);
    servo_set_angle(0);
    delay_ms(500);
}
