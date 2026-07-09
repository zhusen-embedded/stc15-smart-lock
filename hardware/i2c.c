#include "i2c.h"
#include "delay.h"
#include "STC15Fxxxx.H"

#define SCL     P34
#define SDA     P35

/* 1 个 bit 约 5 µs，对应 100 kHz 标准模式。
 * AT24C02 支持 400 kHz，但排线长的时候 100 kHz 更稳。
 */
#define I2C_DELAY()     delay_us(5)

void i2c_init(void)
{
    /* 准双向口，靠外部上拉 */
    P3M1 &= ~0x30;
    P3M0 &= ~0x30;
    SCL = 1;
    SDA = 1;
}

void i2c_start(void)
{
    SDA = 1;
    SCL = 1;
    I2C_DELAY();
    SDA = 0;        /* SCL 高电平期间 SDA 下降沿 = 起始 */
    I2C_DELAY();
    SCL = 0;
}

void i2c_stop(void)
{
    SDA = 0;
    SCL = 1;
    I2C_DELAY();
    SDA = 1;        /* SCL 高电平期间 SDA 上升沿 = 停止 */
    I2C_DELAY();
}

/* 返回 0 表示从机应答，1 表示无应答 */
unsigned char i2c_write_byte(unsigned char byt)
{
    unsigned char i;
    unsigned char ack;

    for (i = 0; i < 8; i++) {
        SDA = (byt & 0x80) ? 1 : 0;
        byt <<= 1;
        I2C_DELAY();
        SCL = 1;
        I2C_DELAY();
        SCL = 0;
    }

    /* 第 9 个时钟读应答 */
    SDA = 1;
    I2C_DELAY();
    SCL = 1;
    I2C_DELAY();
    ack = SDA;
    SCL = 0;

    return ack;
}

unsigned char i2c_read_byte(unsigned char ack)
{
    unsigned char i;
    unsigned char dat = 0;

    SDA = 1;
    for (i = 0; i < 8; i++) {
        I2C_DELAY();
        SCL = 1;
        I2C_DELAY();
        dat <<= 1;
        if (SDA) dat |= 0x01;
        SCL = 0;
    }

    /* 主机应答：ack=1 给低电平（继续读），ack=0 给高电平（结束） */
    SDA = ack ? 0 : 1;
    I2C_DELAY();
    SCL = 1;
    I2C_DELAY();
    SCL = 0;
    SDA = 1;

    return dat;
}
