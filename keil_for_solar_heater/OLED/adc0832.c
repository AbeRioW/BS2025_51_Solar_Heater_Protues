#include "adc0832.h"


void Delay5us()
{
	unsigned char i,j;
	i=2;j=2;
	do{
	
		while(--j);
	}while(--i);
}

unsigned char adc0832_readbyte(unsigned char chanel)
{
	 unsigned char i,j,dat=0;
	 CS=0;

	 //发送起始位
	 CLK = 0;
	 DIO=1;
	 Delay5us();
	 CLK=1;
	 Delay5us();

	 //
	 CLK=0;
	 DIO=1;
	 Delay5us();
	 CLK=1;
	 Delay5us;

	 CLK=0;
	 DIO= chanel;
	 Delay5us();
	 CLK=1;
	 Delay5us();

	 CLK=1;
	 DIO=1;

	 for(i=0;i<8;i++)
	 {
	    CLK=1;
		Delay5us();
		CLK=0;
		Delay5us();
		j=DIO;
		dat=(dat<<1)|j;
	 }

	 CS=1;
	 return dat;
}



























