#ifndef __AT24C02_H
#define __AT24C02_H

/* AT24C02：256 字节 EEPROM，I2C 地址 0xA0（写）/ 0xA1（读）
 *
 * 存储布局（自己定的，不是芯片规定）：
 *   0x00~0x05  当前密码 6 位数字，每位 1 字节
 *   0x06       当前密码有效位数（4~6）
 *   0x07       失败次数计数
 *   0x08~0x0F  预留
 *   0x10~0x2F  开锁记录环形区，每条 4 字节：年 月 日 分
 *
 * 为什么每位单独存一字节而不是整个密码存成 ul：
 *   1. 6 位十进制最大 999999，压成 ul 要 3 字节，改一位就要重写整块；
 *   2. 分开存可以只改变化的位，减少擦写次数；
 *   3. 掉电时最多丢一位，不会整串密码变乱码。
 */

#define EE_ADDR_PWD     0x00
#define EE_ADDR_LEN     0x06
#define EE_ADDR_FAIL    0x07
#define EE_ADDR_LOG     0x10
#define EE_LOG_MAX      8

void ee_init(void);

unsigned char ee_read(unsigned char addr);
void ee_write(unsigned char addr, unsigned char dat);

/* 读回 6 位密码和有效位数，组装成整数 */
unsigned long ee_load_password(unsigned char *len);

/* 保存密码，len 为有效位数（4~6） */
void ee_save_password(unsigned long pwd, unsigned char len);

/* 失败次数 +1，超过 5 次返回 1（触发锁定） */
unsigned char ee_inc_fail(void);
void ee_clear_fail(void);

/* 追加一条开锁记录，满了覆盖最旧的 */
void ee_log(unsigned char year, unsigned char month, unsigned char day, unsigned char minute);

#endif
