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
#include <stdlib.h> 

//VoFA
Vofa_HandleTypedef vofa1;
float	Ii_temp=0;
float	Io_temp=0;
float  Vi_temp=0;
float Vo_temp = 0;
float Vofa_Buffer[8];
// Ensure buffer and buffer_index are properly declared and initialized
static char buffer[32] = {0}; // Buffer to store input characters
static int buffer_index = 0;  // Index for the buffer

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
    vrms_DC_output_target=12.0f;
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

        //保护方式，添加消抖
        static int debounce_counter_lv_in = 0;
        static int debounce_counter_oc = 0;
        static int debounce_counter_sc_out = 0;

        if (vrms_DC_input < 23.0f) {
            if (++debounce_counter_lv_in > 500) { // 连续500次检测到低电压才触发保护
            flag_protect = LESS_VOLTAGE_IN;
            debounce_counter_lv_in = 0; // 重置计数器
            }
        } else {
            debounce_counter_lv_in = 0; // 重置计数器
        }

        if (irms_DC_output > 2.5f) {
            if (++debounce_counter_oc > 500) { // 连续500次检测到过流才触发保护
            flag_protect = OVER_CURRENT;
            debounce_counter_oc = 0; // 重置计数器
            }
        } else {
            debounce_counter_oc = 0; // 重置计数器
        }
        if (zout < 0.1f&&zout>0.0f&&flag_protect==NORMAL) {
            if (++debounce_counter_sc_out > 500) { // 连续500次检测到短路才触发保护
            flag_protect = SHORT_CIRCUIT_OUT;
            debounce_counter_sc_out = 0; // 重置计数器
            }
        } else {
            debounce_counter_sc_out = 0; // 重置计数器
        }
        if (flag_protect != LESS_VOLTAGE_IN && flag_protect != OVER_CURRENT && flag_protect != SHORT_CIRCUIT_OUT) {
            flag_protect = NORMAL;
        }
        //复位判断
        if(flag_reset==1)
        {
           TIM1_Config_Init();//PWM重新输出
           flag_protect=NORMAL;//正常模式
           flag_reset=0;       //退出复位状态
        }
        if (flag_protect== NORMAL)
        {
            LED0=1;
            LED2=0;
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
           set_duty1(duty);
        }else{
            LED0=0;
            LED2=1;
           // PWM_StopAndSetLow();
            set_duty1(0);
        }
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除中断标志位
}

//测量并显示充电电流 I1，在 I1=1~2A 范围内测量精度不低于 2%。
void TIM3_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{	
        OLED_Clear();
        
        Vofa_Buffer[0]=irms_DC_output;
        Vofa_Buffer[1]= vrms_DC_output_target;
        Vofa_Buffer[2] = (1-duty)*PWM_PERIOD;
        Vofa_Buffer[3] = vrms_DC_output;
        Vofa_JustFloat(&vofa1,Vofa_Buffer,3);
        
        key_command_callback(&key1); //按键回调函数，返回键值
        //键入设定值
        if ((key1.key_num >= '0' && key1.key_num <= '9') || key1.key_num == '.') {
            if (buffer_index < sizeof(buffer) - 1) {
            buffer[buffer_index++] = key1.key_num;
            buffer[buffer_index] = '\0'; // Null-terminate the string
            }
        } else if (key1.key_num == 'S') {
            if (buffer_index > 0) {
            char *endptr;
            float temp_value = strtof(buffer, &endptr); // Convert string to float with validation
            if (*endptr == '\0') { // Check if the entire string was converted
                vrms_DC_output_target = temp_value;
            } else {
                // Handle invalid input (e.g., show an error message or reset the buffer)
                OLED_ShowString(0, 0, "Invalid Input", OLED_8X16);
            }
            buffer_index = 0; // Reset buffer index
            memset(buffer, 0, sizeof(buffer)); // Clear the buffer
            }
        } else if (key1.key_num == 'B') {
            if (buffer_index > 0) {
            buffer[--buffer_index] = '\0'; // Remove last character
            }
        }else if(key1.key_num == 'N')
        {
            vrms_DC_output_target+=0.1;
        }
        else if(key1.key_num == 'F')
        {
            vrms_DC_output_target-=0.1;
        }
        OLED_ShowString(0,0,"Uset:",OLED_8X16);OLED_ShowString(108,0,"V",OLED_8X16);
        // Display logic
        if (buffer_index > 0) {
            OLED_ShowString(48, 0, buffer, OLED_8X16); // Show buffer
            OLED_ReverseArea(48, 0, 108, 16);
        } else {
            if (vrms_DC_output_target >= 0 && vrms_DC_output_target <= 15) {
            OLED_ShowFloatNum(48, 0, vrms_DC_output_target, 2, 2, OLED_8X16); // Show target value
            } else {
            OLED_ShowString(48, 0, "ERR", OLED_8X16); // Show error if out of range
            }
        }
        OLED_ShowString(0,16,"Uout:",OLED_8X16);OLED_ShowString(108,16,"V",OLED_8X16);
        
        OLED_ShowFloatNum(48,16,vrms_DC_output,2,2,OLED_8X16);
        
        OLED_ShowString(0,32,"Iout:",OLED_8X16);OLED_ShowString(108,32,"A",OLED_8X16);
        OLED_ShowFloatNum(48,32,irms_DC_output,2,2,OLED_8X16);
        
        OLED_ShowString(0,48,"Protect:",OLED_8X16);
        if (flag_protect == LESS_VOLTAGE_IN) {
            OLED_ShowString(64,48,"LV_IN",OLED_8X16);
        } else if (flag_protect == OVER_CURRENT) {
            OLED_ShowString(64,48,"OC",OLED_8X16);
        } else if (flag_protect == SHORT_CIRCUIT_OUT) {
            OLED_ShowString(64,48,"SC_OUT",OLED_8X16);
        } else {
            OLED_ShowString(64,48,"NORMAL",OLED_8X16);
        }

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


