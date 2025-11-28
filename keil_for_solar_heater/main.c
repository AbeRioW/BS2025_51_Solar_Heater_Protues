#include "oled.h"
#include "adc0832.h"
#include "stdio.h"

u32 level_high = 100;
u32 level_low = 20;
u32 temp_high = 80; 

typedef unsigned int WORD;
sbit P17 = P1^7;

WORD count;                         //1000 times counter
/* define constants */
#define FOSC 11059200L
#define T1MS (65536-FOSC/12/1000)   //1ms timer calculation method in 12T mode

//motor
sbit P20 = P2^0;
sbit P21 = P2^1;
sbit P22 = P2^2;

int show_main_ui = 0;   //showui的标志位
int is_setting = 0;

//button
sbit B_UP = P1^5;
sbit B_DOWN = P1^6;
sbit B_ENSURE = P1^7;


//External interrupt0 service routine
void exint0() interrupt 0           //(location at 0003H)
{
    is_setting++;
	if(is_setting>1)
	{
	   is_setting=0;
	} 
}


void tm0_isr() interrupt 1
{
    TL0 = T1MS;                     //reload timer0 low byte
    TH0 = T1MS >> 8;                //reload timer0 high byte
    if (count-- == 0)               //1ms * 1000 -> 1s
    {
        count = 1000;               //reset counter
		show_main_ui=1;
    }
}

//定时器1ms一次
void timer0_init(void)
{
    TMOD |= 0x01;                    //set timer0 as mode1 (16-bit)
    TL0 = T1MS;                     //initial timer0 low byte
    TH0 = T1MS >> 8;                //initial timer0 high byte
    TR0 = 1;                        //timer0 start running
    ET0 = 1;                        //enable timer0 interrupt
    EA = 1;                         //open global interrupt switch
    count = 0;                      //initial counter
}


void motor_control(int cmd)
{
      if(cmd==0)	 //上水
	  {
	     P20 = 1;
		 P21 = 0;
		 P22 = 1;
	  }

	  if(cmd==1)	 //下水
	  {
	     P20 = 0;
		 P21 = 1;
		 P22 = 1;
	  }

	  if(cmd==2)	 //停止
	  {
	     P20 = 0;
		 P21 = 0;
		 P22 = 0;
	  }
}

void setting_ui(void)
{
  int current_position=0;
  OLED_Clear();
  while(1)
  {
  	OLED_ShowString(0,0,"  Setting",8);

  	OLED_ShowString(0,1,"level low:",8);
  	OLED_ShowNum2(80,1,level_low,3,8,1); 
	OLED_ShowString(120,1,current_position==0?"<":" ",8);
   
    OLED_ShowString(0,2,"level high:",8);
    OLED_ShowNum2(88,2,level_high,3,8,1); 
     OLED_ShowString(120,2,current_position==1?"<":" ",8);

    OLED_ShowString(0,3,"temperature:",8);
    OLED_ShowNum2(96,3,temp_high,3,8,1);
	OLED_ShowString(120,3,current_position==2?"<":" ",8);

    if(B_UP==0)
	 {
	       switch(current_position)
		   {
		      case 0:
			  	   level_low++;
				   if(level_low>30)
				   		level_low=0;
			  break;			  	
			  case 1:
			  		level_high++;
					if(level_high>100)
						level_high=60;
			  break;
			  case 2:
			  		temp_high++;
					if(temp_high>100)
					   temp_high = 80;
			  break;
		   }
	   } 

	   if(B_DOWN==0)
	   {
	       switch(current_position)
		   {
		      case 0:
			  	   level_low--;
				   if(level_low<0)
				   		level_low=30;
			  break;			  	
			  case 1:
			  		level_high--;
					if(level_high<60)
						level_high=100;
			  break;
			  case 2:
			  		temp_high--;
					if(temp_high<80)
					   temp_high = 100;
			  break;
		   }
	   } 

	   if(B_ENSURE==0)
	   {
	       current_position++;
		   if(current_position>2)
		   current_position=0;
	   } 

	   if(!is_setting)
	   {
	         OLED_Clear();
	       break;
	   }
	}
}

void main_ui(void)
{
    u8 adc0_data,adc1_data; 
   	OLED_ShowString(0,0,"  water heater",8); 
	if(!is_setting)
	{
	   show_main_ui = 0;
	   adc0_data= adc0832_readbyte(0);	  //水位
	   adc1_data= adc0832_readbyte(1);

	   OLED_ShowString(0,1,"water level:",8);
	   OLED_ShowNum2(96,1,(u32)adc0_data,4,8,1);

	   OLED_ShowString(0,2,"temperature:",8);
	   OLED_ShowNum2(96,2,(u32)adc1_data,4,8,1);
	   if(adc0_data<level_low||adc1_data>temp_high)	 //水位低于20或者温度大于90就上水
	   {
	       	motor_control(0);
	   }
	   else if(adc0_data>level_high)  //水位太高,下水
	   {
	        motor_control(1);
	   }
	   else
	   {
	       motor_control(2);
	   }

	}

}

void main()
{	   

  
	char* data_light;
	OLED_Init();  //初始话OLED
	OLED_Clear();

		IT0 = 1;       //set INT0 int type (1:Falling 0:Low level)
    EX0 = 1;      //enable INT0 interrupt

	CLK = 0;  //ADC初始化
	CS = 1;

	timer0_init(); //定时器初始化


	while(1)
	{
	    if(!is_setting)
		{
		  main_ui();
		}
		else
		{
		   setting_ui();
		}


	}
}