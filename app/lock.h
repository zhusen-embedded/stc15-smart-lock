#ifndef __LOCK_H
#define __LOCK_H

/* 密码锁业务逻辑，和硬件驱动分开：
 *   hardware/  只关心引脚和时序
 *   app/       只关心状态和流程
 * 这样换一块板子只要改 hardware，逻辑不用动。
 */

#define PWD_MIN_LEN     4
#define PWD_MAX_LEN     6
#define FAIL_LIMIT      5
#define LOCKOUT_SEC     180

typedef enum {
    STATE_LOCKED = 0,   /* 待机，等待输入 */
    STATE_INPUT,        /* 正在输入密码 */
    STATE_UNLOCKED,     /* 已开锁，等待自动回锁 */
    STATE_MENU,         /* 设置菜单 */
    STATE_CHANGE_OLD,   /* 修改密码：验证旧密码 */
    STATE_CHANGE_NEW,   /* 修改密码：输入新密码 */
    STATE_LOCKOUT       /* 连续错误锁定中 */
} lock_state_t;

void lock_init(void);

/* 主循环里反复调用，非阻塞 */
void lock_poll(void);

lock_state_t lock_state(void);

#endif
