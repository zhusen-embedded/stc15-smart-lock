#ifndef __SERVO_H
#define __SERVO_H

/* 舵机控制：50 Hz PWM，0.5 ms~2.5 ms 脉宽对应 0~180 度
 *
 * 用 Timer0 做 100 µs 基准中断，中断里数节拍：
 *   1 个周期 = 20 ms = 200 个节拍
 *   0 度  = 0.5 ms =  5 个节拍
 *   180 度 = 2.5 ms = 25 个节拍
 *
 * 没用硬件 PWM 是因为 STC15 的 PCA 模块要占用固定引脚，
 * 而板子上舵机接在 P2.1，只能用定时器软件生成。
 */

#define SERVO_TICK_PER_MS   10
#define SERVO_PERIOD_TICK   200
#define SERVO_MIN_TICK      5
#define SERVO_MAX_TICK      25

void servo_init(void);

/* 角度 0~180，超范围自动夹紧 */
void servo_set_angle(unsigned char deg);

/* 开锁动作：转到 90 度保持 1 s，再回 0 度 */
void servo_unlock(void);

/* 定时器中断服务里调用，10 µs 进一次 */
void servo_tick(void);

#endif
