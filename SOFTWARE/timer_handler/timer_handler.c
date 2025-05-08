#include "stm32f4xx.h"
#include "timer_handler.h"
#include "global.h"
#include "AD7606.h"
#include "main.h"
#include "spwm.h"
#include "OLED.h"
#include "gpio.h"
#include "sys.h"
#include "transform.h"
#include "arm_math.h"
#include "Vofa.h"
#include "MENU.h"
#include "control.h"
// 计数变量
int cnt = 0;
int cnt_s = 0;
int init_cnt = 0;

// 采样 + 控制
void TIM2_IRQHandler(void) {
     /********************** 作调试用 ************************************/
    cnt++;
    if (cnt >= 1 / Ts) {
      cnt_s++;
      cnt = 0;
    }
    /*******************************************************************/
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) { // 溢出中断
    
    AD7606_read();
    
    internal_theta = internal_theta + h_pi * Ts;
    if (internal_theta >= 2 * pi)
    {
    	internal_theta = internal_theta - 2 * pi;
    }
    else if (internal_theta <= 0)
    {
    	internal_theta = internal_theta + 2 * pi;
    }
    input_voltage = adc_real[0];
    input_current = adc_real[1];
    port_current = adc_real[2];

    sogi(&sogi_v, input_voltage, &input_voltage_alpha, &input_voltage_beta);
    sogi(&sogi_i, input_current, &input_current_alpha, &input_current_beta);
    sogi(&sogi_p, port_current, &port_current_alpha, &port_current_beta);

    ab_to_dq(input_current_alpha, input_current_beta, theta, &input_current_d, &input_current_q);
    ab_to_dq(port_current_alpha, port_current_beta, theta, &port_current_d, &port_current_q);

    arm_sqrt_f32(input_voltage_d * input_voltage_d + input_voltage_q * input_voltage_q, &input_voltage_peak);
    arm_sqrt_f32(input_current_d * input_current_d + input_current_q * input_current_q, &input_current_peak);
    arm_sqrt_f32(port_current_alpha * port_current_alpha + port_current_beta * port_current_beta, &port_current_peak);

    input_power_factor_angle = -1.0f * atanf(input_current_q / input_current_d); // 电压超前电流角度，弧度制。
    input_power_factor = arm_cos_f32(input_power_factor_angle);
    input_apparent_power = input_voltage_peak * input_current_peak * 0.5f;
    input_active_power = input_apparent_power * input_power_factor;
    
    if (omega > 312.0f && omega < 316.0f) {
      sogi_para_update(&sogi_v, omega);
    } else {
      sogi_para_update(&sogi_v, h_pi);
    }

    // 判断是否锁上环
    if (fabsf(omega - h_pi) <= 1.0f) {
      pll_is_locked = 1;
      LED0 = 1;
    } else {
      pll_is_locked = 0;
      LED0 = 0;
    }

    if(mode == 0)//离网模式
    {
    	theta = internal_theta;
    	ab_to_dq(input_voltage_alpha, input_voltage_beta, theta, &input_voltage_d, &input_voltage_q);
    }
    else//并网模式
    {
        ab_to_dq(input_voltage_alpha, input_voltage_beta, theta, &input_voltage_d, &input_voltage_q);
        pll(&pll_v, input_voltage_q, &omega, &theta); // 电压锁相环
    }
    if (on_off == 1) {
      protect_svpwm();
      control();
      start_svpwm();
      }
     else {
      target_voltage_d = input_voltage_peak / (set_out_ac_voltage * 2.0f) * -1.05f * full_duty * module_rate; // 电压预同步
      if (pll_is_locked && fabsf(port_theta) <= 0.01) {
        theta = port_theta;
      }
    }

    // 串口传输数据
    vofa_databuffer[0] = input_voltage_d;
    vofa_databuffer[1] = input_voltage_q;
    // vofa_databuffer[0] = target_voltage_alpha;
    // vofa_databuffer[1] = target_voltage_beta;

    Vofa_JustFloat(&vofa1, vofa_databuffer, 2);
  }
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志位
}

// 按键检测 + 显示
void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) { // 溢出中断
    
    OLED_Clear();
    OLED_ShowNum(0, 0, cnt_s, 4, 8);
    OLED_ShowFloatNum(0, 16, input_voltage_d, 3, 2, 8);
    OLED_ShowFloatNum(0, 48, input_voltage_q, 3, 2, 8);
    OLED_Update();
    
    // MENU_RunMainMenu();
    
  }
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志位
}

void TIM1_CC_IRQHandler(void) {
  if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET) {

    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
  }

  if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET) {

    TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
  }
}
