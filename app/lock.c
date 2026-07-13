#include "lock.h"
#include "../hardware/key.h"
#include "../hardware/lcd1602.h"
#include "../hardware/at24c02.h"
#include "../hardware/servo.h"
#include "delay.h"
#include "STC15Fxxxx.H"

#define BEEP    P20
#define AUTO_RELOCK_MS  5000UL

static lock_state_t s_state = STATE_LOCKED;
static unsigned long s_pwd = 0;         /* 已保存的正确密码 */
static unsigned char s_pwd_len = 6;
static unsigned long s_input = 0;       /* 正在输入的密码 */
static unsigned char s_input_len = 0;
static unsigned long s_tick = 0;        /* 1 ms 递增的软时钟 */
static unsigned long s_unlock_at = 0;   /* 到这个时刻自动回锁 */
static unsigned long s_lockout_at = 0;
static unsigned char s_fail = 0;
static unsigned char s_menu_sel = 0;

static void beep(unsigned char ms)
{
    BEEP = 0;           /* 低电平响 */
    delay_ms(ms);
    BEEP = 1;
}

static void show_idle(void)
{
    lcd_clear();
    lcd_puts("Password:");
    lcd_put_mask(0, 1, s_input_len);
}

static void enter_lockout(void)
{
    s_state = STATE_LOCKOUT;
    s_lockout_at = s_tick + LOCKOUT_SEC * 1000UL;
    s_input = 0;
    s_input_len = 0;
    lcd_clear();
    lcd_puts("Locked 3 min");
    beep(500);
}

static void try_unlock(void)
{
    if (s_input_len != s_pwd_len) {
        /* 位数都不对，直接算错，不用比对 */
        s_fail++;
        beep(200);
        lcd_clear();
        lcd_puts("Wrong length");
        delay_ms(800);
        s_input = 0;
        s_input_len = 0;
        if (s_fail >= FAIL_LIMIT) enter_lockout();
        else show_idle();
        return;
    }

    if (s_input == s_pwd) {
        s_fail = 0;
        s_state = STATE_UNLOCKED;
        s_unlock_at = s_tick + AUTO_RELOCK_MS;
        lcd_clear();
        lcd_puts("Unlocked");
        beep(100);
        servo_unlock();
        ee_log(25, 11, 20, 0);      /* 记录开锁，年月日时后续接 RTC */
        s_input = 0;
        s_input_len = 0;
    } else {
        s_fail++;
        if (ee_inc_fail()) {
            enter_lockout();
        } else {
            beep(200);
            lcd_clear();
            lcd_puts("Wrong password");
            delay_ms(800);
            s_input = 0;
            s_input_len = 0;
            show_idle();
        }
    }
}

void lock_init(void)
{
    P2M1 &= ~0x01;
    P2M0 |= 0x01;       /* 蜂鸣器推挽 */
    BEEP = 1;

    key_init();
    lcd_init();
    ee_init();
    servo_init();

    s_pwd = ee_load_password(&s_pwd_len);

    lcd_clear();
    lcd_puts("Smart Lock");
    lcd_set_cursor(0, 1);
    lcd_puts("Ready");
    delay_ms(800);
    show_idle();
}

/* 菜单：左/右选 YES/NO，确定进入，模式键退出 */
static void menu_poll(unsigned char k)
{
    if (k == KEY_LEFT) {
        s_menu_sel = 0;
        lcd_set_cursor(0, 1);
        lcd_puts("*YES   NO ");
    } else if (k == KEY_RIGHT) {
        s_menu_sel = 1;
        lcd_set_cursor(0, 1);
        lcd_puts(" YES  *NO ");
    } else if (k == KEY_OK) {
        if (s_menu_sel == 0) {
            s_state = STATE_CHANGE_OLD;
            s_input = 0;
            s_input_len = 0;
            lcd_clear();
            lcd_puts("Old password:");
            lcd_put_mask(0, 1, 0);
        } else {
            s_state = STATE_LOCKED;
            show_idle();
        }
    } else if (k == KEY_MODE) {
        s_state = STATE_LOCKED;
        show_idle();
    }
}

static void change_old_poll(unsigned char k)
{
    if (k >= KEY_0 && k <= KEY_9 && s_input_len < PWD_MAX_LEN) {
        s_input = s_input * 10 + k;
        s_input_len++;
        lcd_put_mask(0, 1, s_input_len);
    } else if (k == KEY_BACK && s_input_len > 0) {
        s_input /= 10;
        s_input_len--;
        lcd_put_mask(0, 1, s_input_len);
    } else if (k == KEY_CLEAR) {
        s_input = 0;
        s_input_len = 0;
        lcd_put_mask(0, 1, 0);
    } else if (k == KEY_OK) {
        if (s_input == s_pwd && s_input_len == s_pwd_len) {
            s_state = STATE_CHANGE_NEW;
            s_input = 0;
            s_input_len = 0;
            lcd_clear();
            lcd_puts("New password:");
            lcd_put_mask(0, 1, 0);
        } else {
            beep(200);
            lcd_clear();
            lcd_puts("Old wrong");
            delay_ms(800);
            s_state = STATE_LOCKED;
            s_input = 0;
            s_input_len = 0;
            show_idle();
        }
    } else if (k == KEY_MODE) {
        s_state = STATE_LOCKED;
        s_input = 0;
        s_input_len = 0;
        show_idle();
    }
}

static void change_new_poll(unsigned char k)
{
    if (k >= KEY_0 && k <= KEY_9 && s_input_len < PWD_MAX_LEN) {
        s_input = s_input * 10 + k;
        s_input_len++;
        lcd_put_mask(0, 1, s_input_len);
    } else if (k == KEY_BACK && s_input_len > 0) {
        s_input /= 10;
        s_input_len--;
        lcd_put_mask(0, 1, s_input_len);
    } else if (k == KEY_CLEAR) {
        s_input = 0;
        s_input_len = 0;
        lcd_put_mask(0, 1, 0);
    } else if (k == KEY_OK) {
        /* 至少 4 位才允许保存 */
        if (s_input_len >= PWD_MIN_LEN) {
            s_pwd = s_input;
            s_pwd_len = s_input_len;
            ee_save_password(s_pwd, s_pwd_len);
            lcd_clear();
            lcd_puts("Saved");
            beep(100);
            delay_ms(800);
        } else {
            lcd_clear();
            lcd_puts("Need >= 4 digits");
            delay_ms(800);
        }
        s_input = 0;
        s_input_len = 0;
        s_state = STATE_LOCKED;
        show_idle();
    } else if (k == KEY_MODE) {
        s_state = STATE_LOCKED;
        s_input = 0;
        s_input_len = 0;
        show_idle();
    }
}

static void locked_poll(unsigned char k)
{
    if (k >= KEY_0 && k <= KEY_9 && s_input_len < PWD_MAX_LEN) {
        s_input = s_input * 10 + k;
        s_input_len++;
        lcd_put_mask(0, 1, s_input_len);
    } else if (k == KEY_BACK && s_input_len > 0) {
        s_input /= 10;
        s_input_len--;
        lcd_put_mask(0, 1, s_input_len);
    } else if (k == KEY_CLEAR) {
        s_input = 0;
        s_input_len = 0;
        lcd_put_mask(0, 1, 0);
    } else if (k == KEY_OK && s_input_len > 0) {
        try_unlock();
    } else if (k == KEY_MODE) {
        s_state = STATE_MENU;
        s_menu_sel = 1;
        lcd_clear();
        lcd_puts("Change pwd?");
        lcd_set_cursor(0, 1);
        lcd_puts(" YES  *NO ");
    }
}

void lock_poll(void)
{
    unsigned char k = key_wait();

    s_tick += 10;       /* 粗略软时钟，一次轮询约 10 ms */

    switch (s_state) {
    case STATE_LOCKED:
        locked_poll(k);
        break;

    case STATE_MENU:
        menu_poll(k);
        break;

    case STATE_CHANGE_OLD:
        change_old_poll(k);
        break;

    case STATE_CHANGE_NEW:
        change_new_poll(k);
        break;

    case STATE_UNLOCKED:
        if (s_tick >= s_unlock_at) {
            s_state = STATE_LOCKED;
            show_idle();
        }
        break;

    case STATE_LOCKOUT:
        if (s_tick >= s_lockout_at) {
            s_fail = 0;
            ee_clear_fail();
            s_state = STATE_LOCKED;
            show_idle();
        } else if ((s_lockout_at - s_tick) % 1000 < 10) {
            /* 每秒刷一次剩余时间 */
            lcd_set_cursor(0, 1);
            lcd_puts("Wait ");
            lcd_put_uint(5, 1, (s_lockout_at - s_tick) / 1000, 3);
            lcd_puts(" s ");
        }
        break;

    default:
        s_state = STATE_LOCKED;
        break;
    }
}

lock_state_t lock_state(void)
{
    return s_state;
}
