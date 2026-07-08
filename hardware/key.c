#include "key.h"
#include "delay.h"

/* 行线 P1.4~P1.7，列线 P1.0~P1.3
 * 扫描时把一列拉低，读四行；有行被拉低说明该列该行的键按下。
 */
#define KEY_PORT    P1

static const unsigned char s_keymap[4][4] = {
    /* 列 0 */ {KEY_1, KEY_4, KEY_7, KEY_LEFT},
    /* 列 1 */ {KEY_2, KEY_5, KEY_8, KEY_0},
    /* 列 2 */ {KEY_3, KEY_6, KEY_9, KEY_RIGHT},
    /* 列 3 */ {KEY_OK, KEY_BACK, KEY_CLEAR, KEY_MODE},
};

void key_init(void)
{
    /* P1 全部准双向口，内部上拉 */
    P1M1 = 0x00;
    P1M0 = 0x00;
    KEY_PORT = 0xFF;
}

/* 扫描一次原始键值，不做消抖 */
static unsigned char key_raw(void)
{
    unsigned char col, row, val;

    for (col = 0; col < 4; col++) {
        KEY_PORT = (unsigned char)~(1 << col);
        delay_us(20);

        val = KEY_PORT;
        for (row = 0; row < 4; row++) {
            /* 行线是 P1.4~P1.7，右移 4 位对齐到 bit0~bit3 */
            if (((val >> (4 + row)) & 0x01) == 0) {
                return s_keymap[col][row];
            }
        }
    }
    return KEY_NONE;
}

unsigned char key_scan(void)
{
    static unsigned char s_last = KEY_NONE;
    unsigned char now = key_raw();

    /* 只有连续两次读到同一个键才算数，滤掉抖动 */
    if (now == s_last) {
        return now;
    }
    s_last = now;
    return KEY_NONE;
}

unsigned char key_wait(void)
{
    unsigned char k;

    /* 等按下 */
    do {
        k = key_raw();
    } while (k == KEY_NONE);

    /* 等松开，避免一次按键被当成多次 */
    while (key_raw() != KEY_NONE) {
        delay_ms(5);
    }
    delay_ms(20);
    return k;
}
