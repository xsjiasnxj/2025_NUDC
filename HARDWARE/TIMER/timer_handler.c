#include "stm32f4xx.h"
#include "timer_handler.h"
#include	"global.h"
#include "pid.h"
#include "adc.h"



//void TIM2_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
//	{	
//		
//		
//	}
//	TIM_ClearITPendingBit(TIM2,TIM_IT_Update); //清除中断标志位
//}
