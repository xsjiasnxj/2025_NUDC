#include "global.h"
#include "control.h"
#include "key.h"
#include "key_handler.h"
#include "main.h"
#include "stm32f4xx.h"
#include "stdio.h"

void key_action(char key)
{
  
	if(key=='#')
	{
		menu_index++;
		if(menu_index >= 4)
		{
			menu_index=0;
		}
	}
	else if(key=='S' && disp_page == 0)
	{
		float temp = (float)atof(input_buffer);
		if(menu_index==0)
		{
			set_out_ac_voltage = temp;
		}
		else if(menu_index==1)
		{
			set_out_ac_current = temp;
			if(set_out_ac_current > 2.2f)
			{
				set_out_ac_current = 2.2f;
			}
			if(set_out_ac_current <= 0)
			{
				set_out_ac_current = 0;
			}
		}
		else if(menu_index==2)
		{
			if(temp < 1)
			{
				mode = 0;
			}
			else if(temp >= 1 && temp < 2)
			{
				mode = 1;
			}
			else
			{
				mode = 2;
			}
		}
		else if(menu_index == 3)
		{
			current_rate = temp;
			if(current_rate <= 0)
			{
				current_rate = 0;
			}
			else if(current_rate >= 2.0f)
			{
				current_rate = 2.0f;
			}
		}
	}
	else if(key=='B')
	{
		if(input_index<=0)
		{
			input_index=0;
		}
		else
		{
			input_buffer[input_index-1]='\0';
			input_index--;
		}
	}
	else if(key=='.' || (key>='0' && key<='9'))
	{
		if(input_index<9)
		{
			input_buffer[input_index]=key;
			input_buffer[input_index+1]='\0';
			input_index++;
		}
		else
		{
			for(unsigned char i=0;i<10;i++)
			{
				input_buffer[i]=0;
			}
			input_index=0;
		}
	}
	else if(key=='N' && on_off==0)
	{
      GPIO_ResetBits(GPIO_EN0_GPIO_Port, GPIO_EN0_Pin);
      GPIO_ResetBits(GPIO_EN1_GPIO_Port, GPIO_EN1_Pin);
      GPIO_ResetBits(GPIO_EN2_GPIO_Port, GPIO_EN2_Pin);
      TIM1->CCER =  TIM_CCER_CC1E | TIM_CCER_CC1NE |
                    TIM_CCER_CC2E | TIM_CCER_CC2NE |
                    TIM_CCER_CC3E | TIM_CCER_CC3NE ;
		on_off=1;
	}
	else if(key=='F')
	{
		on_off=0;
		stop_svpwm();
	}  
}