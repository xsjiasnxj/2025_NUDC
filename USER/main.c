#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "OLED.h"
#include "exti.h"
#include "adc.h"
#include "dma.h"
#include "timer.h"
#include "gpio.h"
#include "buzzer.h"
#include "global.h"
#include "main.h"
#include "key.h"
#include "pwm.h"
#include "usart.h"
#include "math.h"
#include "disp.h"
#include "AD7606.h"
#include "Vofa.h"
#include "spi.h"



int Run_mode=0;
float real[4];
//VoFA
Vofa_HandleTypedef vofa1;
float	Ii_temp=0;
float	Io_temp=0;
float  Vi_temp=0;
float  Vo_temp=0;

int main(void)
{
 
    pid_init(&pid1, kp1, ki1, kd1);//电流环，低压端
    pid_init(&pid2, kp4, ki4, kd4);//电压环，高压端
	
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	          //4个抢先级、4个子优先级	
    delay_init(168);
    GPIO_Config_Init();
    EXTIX_Init();	
    AD7606_Init();
    SPI2_Init();
    OLED_Init();
    uart_init(115200); //串口初始化波特率为 115200
    TIM3_Config_Init();//专门用于显示
    TIM1_Config_Init();//专门用于PWM输出
    TIM2_Config_Init();//专门用于数据采样+计算+数字滤波+pid

    //设置初始值
    irms_DC_input_target=1.5;
    vrms_DC_output_target=30;
    duty=0.5;
    set_duty1(duty);
    

	OLED_Clear();
	while (1)
	{		

	}
		
}


//每1us进入一次

void TIM2_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{		
       AD7606_read_data(adc_buffer);
        for(int i=0;i<NUM_CHS;i++)
        {
            if(adc_buffer[i]<32768)
            {
                adc_real[i]=((adc_buffer[i])*10.0  ) / 32768; 
            }
            else{
                adc_buffer[i] = (~adc_buffer[i])+1;
                adc_real[i]=((adc_buffer[i])*10.0 * (-1) ) / 32768;
            }
        }
		timer_cnt++;
		Ii_temp+=adc_real[0];
		Io_temp+=adc_real[1];
		Vi_temp+=adc_real[2];
		Vo_temp+=adc_real[3];
		if(timer_cnt>100)
		{
            real[0]=Ii_temp/100;
            real[1]=Io_temp/100;
            real[2]=Vi_temp/100;	
            real[3]=Vo_temp/100;	
            //计数清零
            timer_cnt=0;	
            Ii_temp=0;
            Io_temp=0;
            Vi_temp=0;
            Vo_temp=0;
            //实际值转换
            //Vofa_JustFloat(&vofa1,real,4);
            irms_DC_input=-1*(real[0]-0.005)/1.94-0.01;
            irms_DC_output=(real[1]-0.025)/1.94;				  
            vrms_DC_input=adc_real[2]*6.2;
            vrms_DC_output=adc_real[3]*6.2;
		}
        //Vofa_JustFloat(&vofa1,real,1);
        
        //电压环
        duty+=pid_limited(&pid2,vrms_DC_output_target,vrms_DC_output,
            duty,-max_duty,max_duty);
		
			if(duty>=max_duty)
			{
			duty=max_duty;			
			}
			if(duty<=min_duty)
			{
			duty=min_duty;			
			}		
			set_duty1(1-duty);
	}
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除中断标志位
}

//测量并显示充电电流 I1，在 I1=1~2A 范围内测量精度不低于 2%。
void TIM3_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{	
        OLED_ShowString(0,0,"I2:",6);OLED_ShowString(72,0,"A",6);
        OLED_ShowFloatNum(24,0,irms_DC_input,2,3,6);
        OLED_ShowString(0,8,"U1:",6);OLED_ShowString(72,8,"V",6);
        OLED_ShowFloatNum(24,8,vrms_DC_output,2,3,6);
        OLED_ShowString(0,16,"duty:",6);
        OLED_ShowFloatNum(24,16,duty,1,4,6);
        OLED_ShowString(0,40,"Iset:",6);OLED_ShowString(72,40,"A",6);
        OLED_ShowFloatNum(24,40,irms_DC_input_target,2,3,6);
        OLED_ShowString(0,48,"Uset:",6);OLED_ShowString(72,48,"V",6);
        OLED_ShowFloatNum(24,48,vrms_DC_output_target,2,3,6);
        // OLED_ShowString(0,0,"I2:",6);OLED_ShowString(72,0,"A",6);
        OLED_Update();
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
}

//void TIM4_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
//	{		
//        if(loop_state==ILOOP)
//        {
//            duty+=pid_limited(&pid1,irms_DC_input_target,irms_DC_input,
//            duty,-max_duty,max_duty);
//		
//			if(duty>=max_duty)
//			{
//			duty=max_duty;			
//			}
//			if(duty<=min_duty)
//			{
//			duty=min_duty;			
//			}		
//            
//			set_duty1(duty);
//        }else if(loop_state==VLOOP)
//        {
//            
//        }
//	}
//	TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除中断标志位
//}


