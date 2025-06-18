#ifndef	__SPWM_H__
#define	__SPWM_H__

/**
  * @brief  参数宏
  * @param  无
  * @retval 无
  */
#define CKTIM	    ((u32)168000000uL)  //主频
#define PWM_PRSC    ((u8)0)            //TIM1分频系数
#define PWM_FREQ    ((u16) 20000)      //PWM频率(Hz)
#define PWM_PERIOD  ((u16) (CKTIM / (u32)(2 * PWM_FREQ *(PWM_PRSC+1))))
#define MODULAT	    (float)0.8           //调制度
	
/**
  * @brief  函数定义
  * @param  无
  * @retval 无
  */

#endif