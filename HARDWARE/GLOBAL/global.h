#ifndef	__GLOBAL_H__
#define	__GLOBAL_H__

#include "pid.h"
#include "pll.h"
#include "sogi.h"
#include "filter.h"

#define MODE_CD 0
#define MODE_FD 1
#define ILOOP   0
#define VLOOP   1
#define NLOOP   -1
#define h_pi 					314.15926535f
#define M_PI 					3.1415926535f
#define sqrt2 					1.41421356f
#define sample_time 			0.0001f
#define v_time					0.020f
#define disp_time   			0.01f
#define pid_time   			0.001f
//#define vref		1.6418
extern float vref;

extern uint16_t timer_cnt;

//频率设置
#define CKTIM	    ((u32)168000000uL)  //主频
#define PWM_PRSC    ((u8)0)            //TIM1分频系数
#define PWM_FREQ    ((u16) 5000)      //PWM频率(Hz)
#define PWM_PERIOD  ((u16) (CKTIM / (u32)( 2*PWM_FREQ *(PWM_PRSC+1))))

#define TIM2_PRSC		((u8)4)
#define TIM2_PERIOD  ((u16) (CKTIM / (u32)( 2.0/sample_time*(TIM2_PRSC+1))))

#define TIM3_PRSC		((u8)400)
#define TIM3_PERIOD  ((u16) (CKTIM / (u32)( 2.0/disp_time*(TIM3_PRSC+1))))

#define TIM4_PRSC		((u8)400)
#define TIM4_PERIOD  ((u16) (CKTIM / (u32)( 2.0/pid_time*(TIM4_PRSC+1))))

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

#define kp4	0.01f
#define ki4	0.0001f
#define kd4	0

extern pidtype pid1,pid2,pid3,pid4;

//采样电路增益
#define gain_vol_1	28.184
#define gain_cur		2.0

extern int loop_state;

extern sogitype sogi_v;
extern plltype  pll_v;
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
extern pointpassfiltertype ppf0;
extern pointpassfiltertype ppf1;
extern pointpassfiltertype ppf2;
extern pointpassfiltertype ppf3;

extern float duty;

extern float vrms_DC_output_target;
extern float irms_DC_input_target;
extern float Irms_AC1_input;
extern float Irms_AC2_input;
extern float vrms_AC1_input;
//期望输出参数
extern float vrms_AC1_output_target;
extern float irms_AC2_output_target;
extern float kI_o1o2;




//输出参数
extern float vrms_AC1_output_target_d;
extern float irms_AC2_output_target_d;



extern float input_AC_voltage;

extern float AC1_input_voltage;
extern float AC1_input_current;

extern float AC2_input_voltage;
extern float AC2_input_current;


extern unsigned char mode;
extern unsigned char list_index;


extern float max_duty ;
extern float min_duty ;

//SOGI锁相变量

extern sogitype sogi_v;
extern plltype  pll_v;
extern float input_voltage_alpha;
extern float input_voltage_beta;
extern float input_voltage_d;
extern float input_voltage_q;
extern float input_voltage_omega;
extern float input_voltage_theta;
//有效值测量变量

extern float vrms_AC1_input;
extern float Irms_AC1_input;
extern float Irms_AC2_input;

extern float v_temp;
extern float I1_temp;
extern float I2_temp;


//期望输出参数
extern  float vrms_AC1_output_target;
extern  float irms_AC2_output_target;
extern  float Iorms_output_target;
extern  float v_temp;
extern  float kI_o1o2;



//滤波变量
extern pointpassfiltertype ppf_vi;
extern pointpassfiltertype ppf_ii1;
extern pointpassfiltertype ppf_ii2;
extern uint16_t filter_init_count;


//显示及功能变量
extern unsigned char mode;
extern unsigned char index;
extern unsigned char page;

#endif