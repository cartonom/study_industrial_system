#include "delay.h"

static unsigned char FacUs=72;//1us执行的系统频率		   
	
//微妙延时函数
void DelayUs(uint32_t Us)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks = Us * FacUs; 						//需要的节拍数 
	told = SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    //这里注意一下SYSTICK是一个递减的计数器就可以了.
			if(tnow < told)tcnt += (told-tnow);
			else tcnt += (reload-tnow+told);	    
			told = tnow;
			if(tcnt >= ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	}
}
			 
//延时ms
void DelayMs(uint16_t Ms)
{
	HAL_Delay(Ms);	//HAL库自带的毫秒延时函数
}


































