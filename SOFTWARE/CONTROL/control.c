#include "stdio.h"
#include "stdlib.h"
#include "global.h"
#include "control.h"
#include "transform.h"
#include "stm32f4xx.h"
#include "main.h"
#include "math.h"
#include "arm_math.h"

float module_rate = 0.98f;
unsigned int full_duty = 4200;
unsigned int set_duty_a = 4200 / 2;
unsigned int set_duty_b = 4200 / 2;

float max_duty = 4200 * 0.95f;
float min_duty = 4200 * 0.1f;

void control()
{

	if(mode == 0)	//主机离网，恒压模式
	{//电压外环
		target_current_d = target_current_d - pid_limited(&pid1, set_out_ac_voltage + 0.2346f * input_current_peak / sqrt2, input_voltage_peak / sqrt2, target_current_d, -3.0f, 3.0f);
	}
	else if(mode == 1)	//slave
	{
		if(current_rate <= 0)
		{
			current_rate = 0;
		}
		else if(current_rate >= 2)
		{
			current_rate = 2;
		}
		target_current_d = -1.0f*port_current_peak * current_rate;
	}
	else if(mode == 2) 
	{
		current_rate = 1;
		target_current_d = -set_out_ac_current * sqrt2;
	}
	target_current_q = 0.0f;
	if(target_current_d > 3.0f)
	{
		target_current_d = 3.0f;
	}
	else if(target_current_d < -3.0f)
	{
		target_current_d = -3.0f;
	}
    //电流内环
	target_voltage_d = target_voltage_d + pid_limited(&pid2, target_current_d, input_current_d, target_voltage_d, -max_duty, max_duty);
	target_voltage_q = target_voltage_q + pid_limited(&pid3, target_current_q, input_current_q, target_voltage_q, -max_duty, max_duty);
    //三次谐波消除
//	target_voltage_d3 = target_voltage_d3 + pid_limited(&pid5, 0, input_current_d3, target_voltage_d3, -max_duty * 0.1f, max_duty * 0.1f);
//	target_voltage_q3 = target_voltage_q3 + pid_limited(&pid6, 0, input_current_q3, target_voltage_q3, -max_duty * 0.1f, max_duty * 0.1f);

	if(target_voltage_d > max_duty)
	{
		target_voltage_d = max_duty;
	}
	else if(target_voltage_d < -max_duty)
	{
		target_voltage_d = -max_duty;
	}

	if(target_voltage_q > max_duty)
	{
		target_voltage_q = max_duty;
	}
	else if(target_voltage_q < -max_duty)
	{
		target_voltage_q = max_duty;
	}

//	if(target_voltage_d3 > max_duty*0.1f)
//	{
//		target_voltage_d3 = max_duty*0.1f;
//	}
//	else if(target_voltage_d3 < -max_duty*0.1f)
//	{
//		target_voltage_d3 = -max_duty*0.1f;
//	}

//	if(target_voltage_q3 > max_duty*0.1f)
//	{
//		target_voltage_q3 = max_duty*0.1f;
//	}
//	else if(target_voltage_q3 < -max_duty*0.1f)
//	{
//		target_voltage_q3 = max_duty*0.1f;
//	}

//	if(buck_duty > max_duty)
//	{
//		buck_duty = max_duty;
//	}
//	else if(buck_duty < min_duty)
//	{
//		buck_duty = min_duty;
//	}

	dq_to_ab(target_voltage_d, target_voltage_q, theta, &target_voltage_alpha, &target_voltage_beta);
//	dq_to_ab(target_voltage_d3, target_voltage_q3, theta3, &target_voltage_alpha3, &target_voltage_beta3);

	if(target_voltage_alpha > max_duty)
	{
		target_voltage_alpha = max_duty;
	}
	else if(target_voltage_alpha < -max_duty)
	{
		target_voltage_alpha = -max_duty;
	}

//	if(target_voltage_alpha3 > max_duty*0.1f)
//	{
//		target_voltage_alpha3 = max_duty*0.1f;
//	}
//	else if(target_voltage_alpha3 < -max_duty*0.1f)
//	{
//		target_voltage_alpha3 = -max_duty*0.1f;
//	}

	arm_sqrt_f32(target_voltage_alpha * target_voltage_alpha + target_voltage_beta * target_voltage_beta, &target_voltage_peak);
}

void start_svpwm()
{
	//pwm3 --> tim1_channel 1
	//pwm2 --> tim1_channel 2
	//pwm3 pwm2互补输出。
	full_duty = 4200;
	max_duty = full_duty * 0.95f;
	min_duty = full_duty * 0.05f;

	set_duty_a = (unsigned int)((target_voltage_alpha * module_rate + full_duty) / 2.0f);
	set_duty_b = (unsigned int)((full_duty - target_voltage_alpha * module_rate) / 2.0f);

	if(set_duty_a > max_duty)
	{
		TIM1->CCR1 = max_duty;
		//TIM1->CCR2 = max_duty;
	}
	else if(set_duty_a < min_duty)
	{
		TIM1->CCR1 = min_duty;
		//TIM1->CCR2 = min_duty;
	}
	else if(set_duty_b > max_duty)
	{
		TIM1->CCR2 = max_duty;
		//TIM1->CCR2 = min_duty;
	}
	else if(set_duty_b < min_duty)
	{
		TIM1->CCR2 = min_duty;
		//TIM1->CCR2 = min_duty;
	}
	else
	{
		TIM1->CCR1 = set_duty_a;
		TIM1->CCR2 = set_duty_b;
	}

}

void stop_svpwm()
{
    GPIO_SetBits(GPIO_EN0_GPIO_Port, GPIO_EN0_Pin);
    GPIO_SetBits(GPIO_EN1_GPIO_Port, GPIO_EN1_Pin);
    GPIO_SetBits(GPIO_EN2_GPIO_Port, GPIO_EN2_Pin);
    TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE); // 通道1
    TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE); // 通道2
    TIM1->CCER &= ~(TIM_CCER_CC3E | TIM_CCER_CC3NE); // 通道3
	pid_init(&pid1, kp1, ki1, kd1);
	pid_init(&pid2, kp2, ki2, kd2);
	pid_init(&pid3, kp3, ki3, kd3);
	pid_init(&pid4, kp4, ki4, kd4);
	pid_init(&pid5, kp5, ki5, kd5);
	pid_init(&pid6, kp6, ki6, kd6);
	target_current_d = 0;
	target_current_q = 0;
	target_voltage_d = 0;
	target_voltage_q = 0;
	target_voltage_alpha = 0;
	target_voltage_beta = 0;
//	target_current_d3 = 0;
//	target_current_q3 = 0;
//	target_voltage_d3 = 0;
//	target_voltage_q3 = 0;
//	target_voltage_alpha3 = 0;
//	target_voltage_beta3 = 0;
//	buck_set_dcv_ramp = 0;
//	buck_duty = full_duty * 0.5f;
//	pll_init(&plltype3, sample_time, 1200, 2800);
}

void protect_svpwm()
{
	if(input_voltage_peak > 45.0f || input_current_peak > 4.2f*1.414f )
	{
		GPIO_SetBits(GPIO_EN0_GPIO_Port, GPIO_EN0_Pin);
		GPIO_SetBits(GPIO_EN1_GPIO_Port, GPIO_EN1_Pin);
		GPIO_SetBits(GPIO_EN2_GPIO_Port, GPIO_EN2_Pin);
		TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE); // 通道1
		TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE); // 通道2
		TIM1->CCER &= ~(TIM_CCER_CC3E | TIM_CCER_CC3NE); // 通道3
		stop_svpwm();
		on_off = 0;
		pid_init(&pid1, kp1, ki1, kd1);
		pid_init(&pid2, kp2, ki2, kd2);
		pid_init(&pid3, kp3, ki3, kd3);
		pid_init(&pid5, kp5, ki5, kd5);
		pid_init(&pid6, kp6, ki6, kd6);
		//pll_init(&plltype3, sample_time, 1200, 2800);
	}
	if(set_out_ac_voltage < input_voltage_peak)
	{
		//set_out_ac_voltage = input_voltage_peak*1.5f;
	}
	if(set_out_ac_voltage >= 2.0f*input_voltage_peak)
	{
		//set_out_ac_voltage = 2.0f*input_voltage_peak;
	}

}
