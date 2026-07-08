#ifndef __KEY_H
#define __KEY_H

#include "STC15Fxxxx.H"

/* 4x4 矩阵键盘
 * 行：P1.7 P1.6 P1.5 P1.4
 * 列：P1.3 P1.2 P1.1 P1.0
 *
 * 键值映射：
 *   1  2  3  A(左)
 *   4  5  6  B(右)
 *   7  8  9  C(确定)
 *   0  E  F  D(退格)
 * 特殊值：
 *   KEY_NONE  无按键
 *   KEY_MODE  进入设置模式
 *   KEY_CLEAR 清零
 */

#define KEY_NONE    0xFF
#define KEY_0       0
#define KEY_1       1
#define KEY_2       2
#define KEY_3       3
#define KEY_4       4
#define KEY_5       5
#define KEY_6       6
#define KEY_7       7
#define KEY_8       8
#define KEY_9       9
#define KEY_LEFT    10
#define KEY_RIGHT   11
#define KEY_OK      12
#define KEY_BACK    13
#define KEY_CLEAR   14
#define KEY_MODE    15

void key_init(void);

/* 扫描一次键盘，返回键值；无按键返回 KEY_NONE。
 * 内部做 20 ms 消抖，需要每 10 ms 左右调用一次。
 */
unsigned char key_scan(void);

/* 阻塞等待一次有效按键（松开后才返回），带 20 ms 消抖 */
unsigned char key_wait(void);

#endif
