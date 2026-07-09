#ifndef __I2C_H
#define __I2C_H

/* 软件模拟 I2C，用在 AT24C02 上
 * SCL = P3.4   SDA = P3.5
 *
 * 没用硬件 I2C 是因为 STC15 的 I2C 只能做主，而这里只需要读一个 EEPROM，
 * 软件模拟省掉中断配置，也方便在任意引脚上调时序。
 */

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_write_byte(unsigned char byt);
unsigned char i2c_read_byte(unsigned char ack);

#endif
