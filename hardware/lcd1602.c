#include "lcd1602.h"
#include "delay.h"
#include "STC15Fxxxx.H"
#include "intrins.h"

#define LCD_DATA    P0
#define LCD_RS      P26
#define LCD_RW      P25
#define LCD_EN      P27

/* 写命令 / 写数据共用：RS 决定是命令还是数据，RW=0 表示写 */
static void lcd_write(unsigned char value, unsigned char is_data)
{
    LCD_RS = is_data;
    LCD_RW = 0;

    /* 高四位 */
    LCD_DATA = (LCD_DATA & 0x0F) | (value & 0xF0);
    LCD_EN = 1;
    delay_us(2);
    LCD_EN = 0;

    /* 低四位 */
    LCD_DATA = (LCD_DATA & 0x0F) | ((unsigned char)(value << 4) & 0xF0);
    LCD_EN = 1;
    delay_us(2);
    LCD_EN = 0;

    delay_us(50);
}

void lcd_init(void)
{
    /* P0 高四位推挽输出，低四位不用 */
    P0M1 = 0x00;
    P0M0 = 0xF0;
    P2M1 = 0x00;
    P2M0 = 0xE0;    /* P2.5 P2.6 P2.7 推挽 */

    delay_ms(15);
    lcd_write(0x33, 0);   /* 8 位模式复位序列 */
    delay_ms(5);
    lcd_write(0x32, 0);   /* 切到 4 位模式 */
    delay_ms(5);
    lcd_write(0x28, 0);   /* 4 位、2 行、5x8 点阵 */
    lcd_write(0x0C, 0);   /* 开显示、关光标 */
    lcd_write(0x06, 0);   /* 写入后光标右移 */
    lcd_write(0x01, 0);   /* 清屏 */
    delay_ms(2);
}

void lcd_clear(void)
{
    lcd_write(0x01, 0);
    delay_ms(2);
}

void lcd_set_cursor(unsigned char x, unsigned char y)
{
    unsigned char addr = (y == 0) ? (0x00 + x) : (0x40 + x);
    lcd_write(addr | 0x80, 0);
}

void lcd_putc(unsigned char c)
{
    lcd_write(c, 1);
}

void lcd_puts(const char *s)
{
    while (*s) {
        lcd_putc((unsigned char)*s++);
    }
}

void lcd_put_uint(unsigned char x, unsigned char y, unsigned long v, unsigned char width)
{
    unsigned char buf[6];
    unsigned char i;

    if (width > 6) width = 6;

    /* 先按 6 位把整个数拆开，不足的前面补空格 */
    for (i = 0; i < 6; i++) {
        buf[5 - i] = (unsigned char)('0' + v % 10);
        v /= 10;
    }

    /* 补空格：只补在 width 之外的那几位 */
    for (i = 0; i < 6 - width; i++) {
        buf[i] = ' ';
    }

    lcd_set_cursor(x, y);
    for (i = 6 - width; i < 6; i++) {
        lcd_putc(buf[i]);
    }
}

void lcd_put_mask(unsigned char x, unsigned char y, unsigned char n)
{
    unsigned char i;

    if (n > 6) n = 6;

    lcd_set_cursor(x, y);
    for (i = 0; i < 6; i++) {
        lcd_putc(i < n ? '*' : ' ');
    }
}
