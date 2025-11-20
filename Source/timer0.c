#include "timer0.h"
#include "7segment_led.h"
#include "led.h"
#include "pwm.h"
#define MODE_NORMAL      			0
#define MODE_SET_HOUR    			1
#define MODE_SET_MINUTE  			2
#define MODE_SET_ALARM_HOUR 	3
#define MODE_SET_ALARM_MINUTE 4

uint8_t count_blink;
uint8_t count_1s;
uint16_t timeout_counter = 0;
bit timeout_enable = 0;
extern bit blink_state, buzz_flag;
extern uint8_t hour, minute, mode;
/***********************************************************
**Function name   :timer0_config()
***********************************************************/
void timer0_config(void)
{
  TMOD = 0x05;          //mode 1 16 bit timer0 
	TCON0 = 0x20;         // fexto /32 = 32MHZ/32
  TH0 = (65536 - 1000)>>8;             
  TL0 = (65536 - 1000);						    	
	TR0 = 1;										  
  ET0 = 1;   
  EAL = 1;
}

/***********************************************************
**Function name   :time0_isr
***********************************************************/
void timer0_isr(void) interrupt ISRTimer0    // Vector @  0x0B
{
	
  TH0 = (65536 - 50000)>>8;    //        
  TL0 = (65536 - 50000);		 
  if(++count_blink > 10) // 50ms*10=0.5s
  { count_blink = 0;
		blink_state = !blink_state;
	}
	if(mode == MODE_NORMAL)
	{ if(++count_1s > 20) // 50ms*20=1s
		{ count_1s = 0; 
			minute++;
			if(minute > 59)
			{ minute = 0; 
				hour++;
				if(hour > 23)
				{hour = 0;}
			}
		}
	}
	if (mode == 1 || mode == 2 || mode == 3 || mode == 4)
  {timeout_enable = 1;}
  else
  { timeout_enable = 0;
    timeout_counter = 0;
  }
	if (timeout_enable)
  { timeout_counter++;
    if (timeout_counter >= 600) // 30s=600
    { timeout_enable = 0;
      timeout_counter = 0;
      mode = MODE_NORMAL;
			buzz_flag = 1;
    }
	}
}
