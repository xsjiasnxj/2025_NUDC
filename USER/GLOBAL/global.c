#include "stm32f4xx.h"
#include "global.h"
#include "pid.h"

//过零点判断标志
char flag_zero;

//采样变量
uint16_t adc_buffer[8];
float adc_real[8];
uint16_t test_num;
uint16_t filter_cnt=0;
uint16_t timer_cnt=0;
float vref=1.65;

pidtype pid1,pid2,pid3,pid4;

//float input_voltage_test;
//float test_voltage_target=1.854;
//float test_voltage_target_d=1;
//输入输出电压电流
float vrms_DC_input=0;
float irms_DC_input=0;
float vrms_DC_output=0;
float irms_DC_output=0;

//输出参数
// float vrms_AC1_output_target_d;
// float irms_AC2_output_target_d;



//float input_AC_voltage;

//float AC1_input_voltage;
//float AC1_input_current;

//float AC2_input_voltage;
//float AC2_input_current;

float duty=0.5;


float vrms_DC_output_target;
float irms_DC_input_target;

float input_DC_voltage;
 
float DC_input_voltage;
float DC_input_current;

//float DC2_input_voltage;
//float DC2_input_current;
float Irms_AC1_input;
float Irms_AC2_input;
float vrms_AC1_input;

float max_duty = 0.8f;
float min_duty = 0.1f;


//有效值测量变量

//float vrms_AC1_input=0;
//float Irms_AC1_input=0;
//float Irms_AC2_input=0;

//float v_temp=0;
//float I1_temp=0;
//float I2_temp=0;

float Vrms_DC_input=0;
float Irms_DC_input=0;
float Vrms_DC_output=0;
float Irms_DC_output=0;


//期望输出参数
 float vrms_AC1_output_target=1;
 float irms_AC2_output_target=0;
 float Iorms_output_target=3;
 float v_temp;
 float kI_o1o2=1.0;



//显示及功能变量

unsigned char mode=0;
unsigned char index=0;
unsigned char page=0;


pidtype pid1,pid2;