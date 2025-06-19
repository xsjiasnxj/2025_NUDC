#include "stm32f4xx.h"
#include "global.h"
#include "pid.h"

//标志变量
char flag_zero;
char flag_protect=0;
char flag_reset = 0;

//调试变量
uint16_t test_cnt=0;
char test_string[10]="11.34";
// 按键
Key_HandleTypeDef key1; //按键句柄
// 采样变量
uint16_t adc_buffer[8];
float adc_real[8];

uint16_t test_num;
uint16_t filter_cnt=0;
uint16_t timer_cnt=0;
float vref=1.65;

pidtype pid1,pid2,pid3,pid4;



//采样还原参数(k,b，补偿)
float  sample2real_k[8]={1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
float  sample2real_b[8]={0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
float  Compensation[8]={0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};


//输入输出电压、电流;输出阻抗
float vrms_DC_input=0;
float irms_DC_input=0;
float vrms_DC_output=0;
float irms_DC_output=0;
float zout=1000000.0;

//占空比
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