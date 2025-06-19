#include "GLOBAL/global.h"
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

//VoFA
Vofa_HandleTypedef vofa1;
float	Ii_temp=0;
float	Io_temp=0;
float  Vi_temp=0;
float  Vo_temp=0;
char keynum='#';
void PWM_StopAndSetLow(void);
    
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
    TIM1_Config_Init();//专门用于PWM输出
    TIM2_Config_Init();//专门用于数据采样+计算+数字滤波+pid
    TIM3_Config_Init();//专门用于显示+串口打印
    
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
        AD7606_read();
		timer_cnt++;
		Ii_temp+=adc_real[0];
		Io_temp+=adc_real[1];
		Vi_temp+=adc_real[2];
		Vo_temp+=adc_real[3];
        //均值滤波
		if(timer_cnt>100)
		{
            //更新实际值
            irms_DC_input=Ii_temp/100;
            irms_DC_output=Io_temp/100;
            vrms_DC_input=Vi_temp/100;
            vrms_DC_output = Vo_temp / 100;
            zout = vrms_DC_output / irms_DC_output;
            // 计数清零
            timer_cnt = 0;
            Ii_temp=0;
            Io_temp=0;
            Vi_temp=0;
            Vo_temp=0;
        }

        //保护方式
        if (vrms_DC_input < 23.0f) {
            flag_protect = LESS_VOLTAGE_IN;
        } else if (irms_DC_output > 2.5f) {
            flag_protect = OVER_CURRENT;
        } else if (zout < 0.1f) {
            flag_protect =  SHORT_CIRCUIT_OUT;
        } else {
            flag_protect = NORMAL;
        }
        //复位判断
        if(flag_reset==1)
        {
           TIM1_Config_Init();//PWM重新输出
           flag_protect=NORMAL;//正常模式
           flag_reset=0;       //退出复位状态
        }
        if (flag_protect== NORMAL) {
            // 电压环
            duty += pid_limited(&pid2, vrms_DC_output_target, vrms_DC_output,
                              duty, -max_duty, max_duty);
            if (duty >= max_duty) {
            duty = max_duty;			
			}
			if(duty<=min_duty)
			{
			duty=min_duty;			
			}
            set_duty1(1 - duty);
        }else{
            PWM_StopAndSetLow();
        }
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除中断标志位
}

//测量并显示充电电流 I1，在 I1=1~2A 范围内测量精度不低于 2%。
void TIM3_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{	
        Vofa_JustFloat(&vofa1,adc_real,1);
       key_command_callback(&key1); //按键回调函数，返回键值
//        OLED_ShowString(0,0,"Uset:",OLED_8X16);OLED_ShowString(108,0,"V",OLED_8X16);
//        OLED_ShowFloatNum(48,0,vrms_DC_output_target,2,3,OLED_8X16);
//        
//        OLED_ShowString(0,16,"Uout:",OLED_8X16);OLED_ShowString(108,16,"V",OLED_8X16);
//        OLED_ShowFloatNum(48,16,vrms_DC_output,2,3,OLED_8X16);
//        
//        OLED_ShowString(0,32,"Iout:",OLED_8X16);OLED_ShowString(108,32,"A",OLED_8X16);
//        OLED_ShowFloatNum(48,32,irms_DC_output,2,3,OLED_8X16);
//        
//        OLED_ShowString(0,48,"Protect:",OLED_8X16);
//        if (flag_protect == LESS_VOLTAGE_IN) {
//            OLED_ShowString(64,48,"LV_IN",OLED_8X16);
//        } else if (flag_protect == OVER_CURRENT) {
//            OLED_ShowString(64,48,"OC",OLED_8X16);
//        } else if (flag_protect == SHORT_CIRCUIT_OUT) {
//            OLED_ShowString(64,48,"SC_OUT",OLED_8X16);
//        } else {
//            OLED_ShowString(64,48,"NORMAL",OLED_8X16);
//        }
           
        // OLED_ShowString(0,0,"I2:",6);OLED_ShowString(72,0,"A",6);
        if(key1.key_num!='#')
        {
            keynum=key1.key_num;
        }

        OLED_ShowChar(0, 0, keynum, OLED_8X16);

        OLED_Update();
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
}


// 关闭PWM并设置为低电平
void PWM_StopAndSetLow(void)
{
    // 关闭定时器
    TIM_Cmd(TIM1, DISABLE);
    // 重新配置GPIO为推挽输出
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //速度 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;      //下拉
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化 PF9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化 PF9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //GPIOF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化 PF9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //GPIOF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化 PF9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化 PF9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //GPIOF9
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化 PF9	
};

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


