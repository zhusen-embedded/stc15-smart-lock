#ifndef __LCD1602_H
#define __LCD1602_H

/* LCD1602 字符屏，4 位数据线接 P0.4~P0.7
 * RS = P2.6   RW = P2.5   EN = P2.7
 */

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(unsigned char x, unsigned char y);
void lcd_putc(unsigned char c);
void lcd_puts(const char *s);

/* 在指定位置打印 1~6 位十进制数，左侧补空格 */
void lcd_put_uint(unsigned char x, unsigned char y, unsigned long v, unsigned char width);

/* 打印掩码：输入 n 位密码时显示 n 个 '*'，其余补空格 */
void lcd_put_mask(unsigned char x, unsigned char y, unsigned char n);

#endif
