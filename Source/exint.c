#include "exint.h"
#include "7segment_led.h"
#include "led.h"
#include " pwm.h"
#include "flash.h"


#define MODE_NORMAL     		 	0
#define MODE_SET_HOUR    			1
#define MODE_SET_MINUTE  			2
#define MODE_SET_ALARM_HOUR 	3
#define MODE_SET_ALARM_MINUTE 4

#define	keyout0		P44																						//按键接口引脚定义
#define	keyout3		P47


#define	keyin0		P24
#define keyin1    P25
#define keyin3    P27

extern uint8_t mode;
extern uint8_t prev_hour, prev_minute;
extern uint8_t hour, minute;
extern uint8_t alarm_hour, alarm_minute;
extern bit buzz_flag;

/***********************************************************
**Function name   :int0_config()
**
**Input parameter : 
**
**Input parameter :
**
**Description     :INT0 is configured to trigger the falling edge
***********************************************************/
void int0_config()
{
  P4  &= 0X0F;
  P4M |= 0XF0;
  
  PEDGE = 0X02;
  EX0 = 1;
  EAL = 1;
}
/***********************************************************
**Function name   :
**
**Input parameter : 
**
**Input parameter :
**
**Description     :
***********************************************************/
void int0_isr(void) interrupt ISRInt0
{ 
  uint8_t i;
  PEDGE = 0;
  P44 = 0;
  P4 |= 0XEF; 
	buzz_flag = 1;
	
  for(i = 0;i<100;i++);
  if(P24 == 0) 
	{
		if(mode == 0)
		{
			prev_hour = hour;
			prev_minute = minute;	
			mode=1;
		}
		else if(mode== 1) 
		{mode=2;}
		else if(mode == 2)
		{mode=0;}
	}
	for(i=0;i<100;i++);
	if(P27 == 0)
	{
		if(mode == 0) mode = 3;
		else if(mode == 3) mode = 4;
		else if(mode == 4)
		{
			flash_save_alarm_time(alarm_hour, alarm_minute);
			mode = 0;
			led_ctr(LED0,1);
		}
	}
	P44 = 1;

	P47 = 0;
	for(i=0;i<100;i++);
	if(P24 == 0)
	{
		if(mode == 1)
		{
			hour++;
			if(hour > 23) 
				hour = 0;
		}
		else if(mode == 2)
		{
			minute++;
			if(minute > 59 )
				minute = 0;
		}
		else if(mode == 3)
		{
			
			alarm_hour++;
			if(alarm_hour > 23)
				alarm_hour = 0;
		}
		else if(mode == 4)
		{
			
			alarm_minute++;
			if(alarm_minute > 59)
				alarm_minute = 0;
		}
		
	}
	for(i=0;i<100;i++);
	if(P25 == 0)
	 {
		 if(mode == 1)
		 {
			 if(hour == 0)
				hour = 23;
			 else hour--;
	   }
		 else if(mode == 2)
		 {
			 if(minute == 0)
			 minute = 59;
			 else minute--;
		 }
		 else if(mode == 3)
		{
			if(alarm_hour == 0)
				alarm_hour = 23;   
			 else alarm_hour--;	 
    }
		else if(mode == 4)
		{
			if(alarm_minute == 0)
				alarm_minute = 59;  
			 else alarm_minute--;
		}		 
	 }
	 P47 = 1;
	P4 &= 0x0F;
	PEDGE = 0X02;
}
	