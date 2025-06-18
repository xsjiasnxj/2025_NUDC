#include "stm32f4xx.h"
#include "disp.h"
#include "OLED.h"
#include "global.h"

void disp_title()
{
	if(mode==0)
	{
		OLED_ShowString(0,0,"SingleMode",8);
	}
	else
	{
		OLED_ShowString(0,0,"ParallelMode",8);
	}
}


void disp_others()
{
	if(mode==0&&page==0)
	{
		OLED_ShowString(0,16,"Uo set:",8);
	}
	else if(mode==0&&page==1)
	{
		OLED_ShowString(0,16,"Uo real:",8);
		OLED_ShowString(0,32,"Io real:",8);
	}
	else if(mode==1&&page==0)
	{
		OLED_ShowString(0,16,"Uo set:",8);
		OLED_ShowString(0,32,"KIo set:",8);
	}
	else if(mode==1&&page==1)
	{
		OLED_ShowString(0,16,"Uo real:",8);
		OLED_ShowString(0,32,"Io1 real:",8);
		OLED_ShowString(0,48,"Io2 real:",8);
	}
	
}

void disp_real()
{
	if(mode==0&&page==0)
	{
		OLED_ShowFloatNum(60,16,vrms_AC1_output_target,2,2,8);
	}
	else if(mode==0&&page==1)
	{
		OLED_ShowFloatNum(60,16,vrms_AC1_input,2,2,8);
		OLED_ShowFloatNum(60,32,Irms_AC1_input,2,2,8);
	}
	else if(mode==1&&page==0)
	{
		OLED_ShowFloatNum(60,16,vrms_AC1_output_target,2,2,8);
		OLED_ShowFloatNum(60,32,kI_o1o2,2,2,8);
		
	}
	else if(mode==1&&page==1)
	{
		OLED_ShowFloatNum(60,16,vrms_AC1_input,2,2,8);
		OLED_ShowFloatNum(60,32,Irms_AC1_input,2,2,8);
		OLED_ShowFloatNum(60,48,Irms_AC2_input,2,2,8);
	}	
}
