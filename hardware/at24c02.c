#include "at24c02.h"
#include "i2c.h"
#include "delay.h"

#define EE_WRITE_ADDR   0xA0
#define EE_READ_ADDR    0xA1

/* AT24C02 单字节写需要 5 ms 内部擦写周期，两次写之间必须留够 */
#define EE_WRITE_CYCLE  5

void ee_init(void)
{
    i2c_init();
    delay_ms(10);
}

unsigned char ee_read(unsigned char addr)
{
    unsigned char dat;

    i2c_start();
    i2c_write_byte(EE_WRITE_ADDR);
    i2c_write_byte(addr);

    /* 重复起始，切到读方向 */
    i2c_start();
    i2c_write_byte(EE_READ_ADDR);
    dat = i2c_read_byte(0);   /* 只读一个字节，不应答 */
    i2c_stop();

    return dat;
}

void ee_write(unsigned char addr, unsigned char dat)
{
    i2c_start();
    i2c_write_byte(EE_WRITE_ADDR);
    i2c_write_byte(addr);
    i2c_write_byte(dat);
    i2c_stop();

    /* 等内部写周期结束，期间芯片不应答 */
    delay_ms(EE_WRITE_CYCLE);
}

unsigned long ee_load_password(unsigned char *len)
{
    unsigned char i;
    unsigned char digit;
    unsigned long pwd = 0;

    *len = ee_read(EE_ADDR_LEN);

    /* 从没存过密码，或者存坏了，回落到出厂默认 123456 */
    if (*len < 4 || *len > 6) {
        *len = 6;
        return 123456UL;
    }

    for (i = 0; i < *len; i++) {
        digit = ee_read(EE_ADDR_PWD + i);
        if (digit > 9) digit = 0;      /* 数据校验，非法位归零 */
        pwd = pwd * 10 + digit;
    }
    return pwd;
}

void ee_save_password(unsigned long pwd, unsigned char len)
{
    unsigned char i;
    unsigned char digit;

    if (len < 4) len = 4;
    if (len > 6) len = 6;

    /* 先存位数，再逐位存数字。中途掉电的话位数已经写上，
     * 但后面的位是旧值，校验时能发现异常。
     */
    ee_write(EE_ADDR_LEN, len);
    for (i = 0; i < len; i++) {
        digit = (unsigned char)(pwd % 10);
        pwd /= 10;
        /* 从高位往低位存，所以要算偏移 */
        ee_write((unsigned char)(EE_ADDR_PWD + len - 1 - i), digit);
    }
}

unsigned char ee_inc_fail(void)
{
    unsigned char n = ee_read(EE_ADDR_FAIL);

    if (n == 0xFF) n = 0;      /* 空片读出来是 0xFF */
    n++;
    ee_write(EE_ADDR_FAIL, n);

    return (n >= 5) ? 1 : 0;
}

void ee_clear_fail(void)
{
    ee_write(EE_ADDR_FAIL, 0);
}

void ee_log(unsigned char year, unsigned char month, unsigned char day, unsigned char minute)
{
    static unsigned char s_slot = 0;
    unsigned char base = (unsigned char)(EE_ADDR_LOG + s_slot * 4);

    ee_write(base + 0, year);
    ee_write(base + 1, month);
    ee_write(base + 2, day);
    ee_write(base + 3, minute);

    s_slot++;
    if (s_slot >= EE_LOG_MAX) s_slot = 0;
}
