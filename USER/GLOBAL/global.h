#ifndef	__GLOBAL_H__
#define	__GLOBAL_H__

#include "pid.h"
#include "key.h"
#include "stm32f4xx.h"

#define h_pi 					314.15926535f
#define M_PI 					3.1415926535f
#define sqrt2 					1.41421356f
#define sample_time 			0.0001f
#define v_time					0.020f
#define disp_time   			0.01f

//频率设置
#define CKTIM	        ((u32)168000000uL)  //主频
#define TIM_CKTIM	    ((u32) 84000000uL)  //定时器频率
#define PWM_PRSC        ((u8)0)            //TIM3分频系数
#define PWM_FREQ        ((u16) 10000)      //PWM频率(Hz)
#define PWM_PERIOD      ((u16) (TIM_CKTIM / (u32)( 2*PWM_FREQ *(PWM_PRSC+1))))  //因为是中心对齐计数模式，所以要乘2
//采样+控制
#define TIM2_PRSC		((u8)4)
#define TIM2_PERIOD     ((u16) (TIM_CKTIM / (u32)(1.0f/sample_time*(TIM2_PRSC+1))))
//显示
#define TIM3_PRSC		((u8)400)
#define TIM3_PERIOD     ((u16) (TIM_CKTIM / (u32)(1.0f/disp_time*(TIM3_PRSC+1))))

////控制
//#define TIM4_PRSC		((u8)4)
//#define TIM4_PERIOD  ((u16) (CKTIM / (u32)( 1.0f/pid_time*(TIM4_PRSC+1))))



//pid参数
#define kp1	0.00003f
#define ki1 0.00008f
#define kd1	0.00f

#define kp2	0.00003f
#define ki2	0.00008f
#define kd2	0.00f

#define kp3	0
#define ki3	0
#define kd3	0

#define kp4	0.00003f
#define ki4	0.00008f
#define kd4	0

enum PROTECT_FLAG {
  NORMAL = 0,   // 正常
  OVER_CURRENT, // 过流
  LESS_VOLTAGE_IN, // 输入欠压
  SHORT_CIRCUIT_OUT, // 输出短路
};

//标志变量
extern char flag_zero;
extern char flag_protect;
extern char flag_reset;
//调试变量
extern uint16_t test_cnt;
extern char test_string[10];

// 按键
extern Key_HandleTypeDef key1; // 按键句柄

extern pidtype pid1,pid2,pid3,pid4;

//采样还原参数(k,b，补偿)
extern float  sample2real_k[8];
extern float  sample2real_b[8];
extern float  Compensation[8];
//采样变量
extern uint16_t adc_buffer[8];
extern float adc_real[8];

extern uint16_t test_num;
extern uint16_t filter_cnt;
extern uint16_t timer_cnt;
extern float vref;

extern float vrms_DC_input;
extern float irms_DC_input;
extern float vrms_DC_output;
extern float irms_DC_output;
extern float zout;

extern float duty;

extern float vrms_DC_output_target;
extern float irms_DC_input_target;
extern float Irms_AC1_input;
extern float Irms_AC2_input;
extern float vrms_AC1_input;


extern unsigned char mode;
extern unsigned char list_index;


extern float max_duty ;
extern float min_duty ;


//期望输出参数
extern  float vrms_AC1_output_target;
extern  float irms_AC2_output_target;
extern  float Iorms_output_target;
extern  float v_temp;
extern  float kI_o1o2;



//显示及功能变量
extern unsigned char mode;
extern unsigned char index;
extern unsigned char page;

#endif