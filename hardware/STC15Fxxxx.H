#ifndef __STC15FXXXX_H
#define __STC15FXXXX_H

/* STC15F2K60S2 寄存器定义（节选，只列本项目用到的）
 * 完整版见 STC 官方 ISP 下载软件里的头文件。
 */

sfr P0      = 0x80;
sfr P1      = 0x90;
sfr P2      = 0xA0;
sfr P3      = 0xB0;
sfr P4      = 0xC0;

sfr P0M1    = 0x93;
sfr P0M0    = 0x94;
sfr P1M1    = 0x91;
sfr P1M0    = 0x92;
sfr P2M1    = 0x95;
sfr P2M0    = 0x96;
sfr P3M1    = 0xB1;
sfr P3M0    = 0xB2;
sfr P4M1    = 0xB3;
sfr P4M0    = 0xB4;

sfr PCON    = 0x87;
sfr AUXR    = 0x8E;
sfr WDT_CONTR = 0xC1;

sfr IE      = 0xA8;
sfr IP      = 0xB8;
sbit EA     = IE^7;
sbit ET0    = IE^1;

sfr TMOD    = 0x89;
sfr TCON    = 0x88;
sfr TH0     = 0x8C;
sfr TL0     = 0x8A;
sbit TR0    = TCON^4;
sbit TF0    = TCON^5;

/* 引脚 */
sbit P20    = P2^0;
sbit P21    = P2^1;
sbit P25    = P2^5;
sbit P26    = P2^6;
sbit P27    = P2^7;
sbit P34    = P3^4;
sbit P35    = P3^5;
sbit P42    = P4^2;

#endif
