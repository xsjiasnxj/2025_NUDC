#ifndef	__GLOBAL_H__
#define	__GLOBAL_H__

#include "pid.h"
#include "pll.h"
#include "sogi.h"
#include "Vofa.h"
#include "key.h"

#define h_pi 					314.15926535f
#define M_PI 					3.1415926535f
#define sqrt2 					1.41421356f

#define Ts 			            0.0005f         //采样时间
#define disp_time   			0.01f

#define MI_MAX                  0.98f           //调制比最大值
//#define pid_time   			    0.001f


//频率设置
#define CKTIM	        ((u32)168000000uL)  //主频
#define TIM_CKTIM	    ((u32) 84000000uL)  //定时器频率
#define PWM_PRSC        ((u8)0)            //TIM3分频系数
#define PWM_FREQ        ((u16) 20000)      //PWM频率(Hz)
#define PWM_PERIOD      ((u16) (TIM_CKTIM / (u32)( 2*PWM_FREQ *(PWM_PRSC+1))))  //因为是中心对齐计数模式，所以要乘2
//采样+控制
#define TIM2_PRSC		((u8)4)
#define TIM2_PERIOD     ((u16) (TIM_CKTIM / (u32)(1.0f/Ts*(TIM2_PRSC+1))))
//显示
#define TIM3_PRSC		((u8)400)
#define TIM3_PERIOD     ((u16) (TIM_CKTIM / (u32)(1.0f/disp_time*(TIM3_PRSC+1))))

////控制
//#define TIM4_PRSC		((u8)4)
//#define TIM4_PERIOD  ((u16) (CKTIM / (u32)( 1.0f/pid_time*(TIM4_PRSC+1))))

//pid参数
#define kp1 0.5f
#define ki1 0.1f
#define kd1 0.0f

#define kp2 0.0846f
#define ki2 0.022f
#define kd2 0.0f

#define kp3 0.8f
#define ki3 0.08f
#define kd3 0.0f

#define kp4 0.0052f
#define ki4 0.00024f
#define kd4 0.0f

#define kp5 0.016f
#define ki5 0.0022f
#define kd5 0.0f

#define kp6 0.003f
#define ki6 0.004f
#define kd6 0.0f

//标志位变量
extern uint16_t pll_is_locked;
extern char test_flag;
extern char on_off;
extern char mode;
//vofa
extern Vofa_HandleTypedef vofa1;
extern float vofa_databuffer[8];
//按键
extern Key_HandleTypeDef key1;
//采样变量
extern uint16_t adc_buffer[8];
extern float adc_real[8];

extern uint16_t test_num;
extern uint16_t filter_cnt;
extern uint16_t timer_cnt;


//采样还原参数(k,b，补偿)
extern float  sample2real_k[8];
extern float  sample2real_b[8];
extern float  Compensation[8];

//pid
extern pidtype pid1,pid2,pid3,pid4,pid5,pid6;
//调制比
extern float MI;
//电流比
extern float current_rate;
//输入信号
extern float port_voltage ;
extern float port_current ;
extern float port_voltage_alpha ;
extern float port_voltage_beta ;
extern float port_current_alpha ;
extern float port_current_beta;
extern float port_voltage_d;
extern float port_voltage_q ;
extern float port_current_d;
extern float port_current_q ;
extern float port_theta;
extern float port_omega;
extern float port_voltage_peak;
extern float port_current_peak; 

extern float set_out_ac_voltage;
extern float set_out_ac_current;
extern float set_dc_current;

extern float input_voltage ;
extern float input_current ;
extern float input_voltage_peak ;
extern float input_current_peak ;
extern float input_apparent_power ;
extern float input_active_power ;
extern float input_power_factor ;
extern float input_power_factor_angle;

//SOGI锁相变量
extern sogitype sogi_v;
extern sogitype sogi_i;
extern sogitype sogi_p;

extern plltype  pll_v;
extern plltype  pll_v3;
//电压内角度（离网模式）
extern float internal_theta;

extern float input_voltage_alpha;
extern float input_voltage_beta;
extern float input_voltage_d;
extern float input_voltage_q;

extern float input_current_alpha;
extern float input_current_beta;
extern float input_current_d;
extern float input_current_q;


extern float omega;
extern float theta;

//期望输出
extern float target_current_d ;
extern float target_current_q ;
extern float target_current_d3 ;
extern float target_current_q3 ;
 
extern float target_voltage_d ;
extern float target_voltage_q ;
extern float target_voltage_d3 ;
extern float target_voltage_q3 ;

extern float target_voltage_alpha ;
extern float target_voltage_beta ;
extern float target_voltage_alpha3;
extern float target_voltage_beta3;

extern float target_voltage_peak;
extern float set_power_factor_angle;

//显示变量
extern unsigned char menu_index;
extern unsigned char input_buffer[10];
extern unsigned char input_index;
extern unsigned disp_page;
#endif