#define __XRAM_SFR_H__
#include "mcu.h"
#include "exint.h"
#include "7segment_led.h"
#include "exint.h"
#include "timer0.h"
#include "led.h"
#include "pwm.h"
#include "flash.h"

#define MODE_NORMAL      0
#define MODE_SET_HOUR    1
#define MODE_SET_MINUTE  2
#define MODE_SET_ALARM_HOUR 3
#define MODE_SET_ALARM_MINUTE 4

uint8_t mode = MODE_NORMAL;
uint8_t hour = 0, minute = 0;
uint8_t prev_hour = 0, prev_minute = 0;
uint8_t alarm_hour = 0, alarm_minute = 0;
bit blink_state = 0;
bit buzz_flag = 0;
bit alarm_triggered = 0;
bit buzzer_on_state = 0;
unsigned int alarm_buzz_counter = 0;  


void delay_ms(uint16_t ms);
void buzzer_pwm_config();
void buzzer_off();
void flash_read_alarm_time(alarm_hour, alarm_minute);
void flash_save_alarm_time(alarm_hour, alarm_minute);

/***********************************************************
**Function name   : check_alarm_trigger
***********************************************************/
void check_alarm_trigger() 
{
	if ((alarm_hour!=0 || alarm_minute!=0) && hour == alarm_hour && minute == alarm_minute)
  { alarm_triggered = 1; 
    alarm_buzz_counter = 0;
    buzzer_on_state = 0;
  }
}
/***********************************************************
**Function name   : update_buzzer_alarm
***********************************************************/
void update_buzzer_alarm()
{ if (alarm_triggered)
  { alarm_buzz_counter++;
		if (alarm_buzz_counter <= 10000) // 5s
    { if ((alarm_buzz_counter % 500) == 0)
			{ if(buzzer_on_state) 
				{ buzzer_off();
				  buzzer_on_state = 0;
				}
        else 
				{ buzzer_pwm_config();
					buzzer_on_state = 1;
        }
			}
		}
    else
    { alarm_triggered = 0;
      buzzer_off();
		  buzzer_on_state = 0;
			alarm_buzz_counter = 0;
    }
  }
}
/***********************************************************
**Function name   : main
***********************************************************/
int main(void)
{ 
	CKCON 	=	0X70;
	CLKSEL 	=	0X05;								//Fcpu = 32M/4
	CLKCMD	=	0X69;								
	CKCON 	=	0X00;	  
  int0_config();
  segment_led_config();
	timer0_config();
	led_config();
	flash_read_alarm_time(&alarm_hour, &alarm_minute);
	while(1)
  {
		if( minute != prev_minute)
		{ prev_minute = minute;
		  check_alarm_trigger();
		}
		update_buzzer_alarm();
		if(buzz_flag) 
		{ buzzer_pwm_config();
			delay_ms(300);
			buzzer_off();
			buzz_flag = 0;
		}
		switch(mode)
		{
			case 0: segment_display_time(hour, minute);
							break;
			case 1: if(blink_state) segment_display_hour(hour, minute);
						  else segment_clear_hour();
						  break;
			case 2: if(blink_state) segment_display_minute(hour, minute);
						  else segment_clear_minute();
						  break;
			case 3: if(blink_state) segment_display_hour(alarm_hour, alarm_minute);
						  else segment_clear_hour();
							if(blink_state) led_ctr(LED0, 0);
							else led_ctr(LED0, 1);
						  break;
			case 4: if(blink_state) segment_display_minute(alarm_hour,alarm_minute);
							else segment_clear_minute();
							if(blink_state) led_ctr(LED0, 0);
							else led_ctr(LED0, 1);
							break;
		}
		WDTR = 0x5A;                //CLR WDTR
    delay_ms(1);
    segment_led_scan();
		segment_display_time(prev_hour, prev_minute);
	}
}
/***********************************************************
**Function name   : delay_ms
***********************************************************/
void delay_ms(uint16_t ms)
{
  uint16_t i,j;
  for(i = 0; i < 100;i++)
  {
    WDTR = 0x5A;
    for(j=0;j<ms;j++);
  }
}
  