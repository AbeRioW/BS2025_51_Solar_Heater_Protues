#ifndef __ADC0832_H
#define __ADC0832_H		
	  	 
#include "REG51.h"

//ADC相关
sbit CS = P1^2;   // 片选信号
sbit CLK = P1^3;  // 时钟信号
sbit DIO = P1^4;  // 数据输入输出（双向）

unsigned char adc0832_readbyte(unsigned char chanel);
#endif  
	 



