#ifndef  __DHT11_H
#define  __DHT11_H

#include "gpio.h"
#include "delay.h"

//与硬件引脚相关的定义，根据配置引脚名称会自动关联
#define		DHT11_SET_1		HAL_GPIO_WritePin(DHT11_GPIO_Port  , DHT11_Pin , GPIO_PIN_SET)
#define		DHT11_SET_0		HAL_GPIO_WritePin(DHT11_GPIO_Port  , DHT11_Pin , GPIO_PIN_RESET)
#define		DHT11_GET		HAL_GPIO_ReadPin (DHT11_GPIO_Port  , DHT11_Pin)

//延时函数从定义名称
#define 	DHT11_DelayUs 	DelayUs
#define 	DHT11_DelayMs 	DelayMs

//DHT11参数存储结构体
typedef struct 
{
	char TemH;		//温度整数部分
	char TemL;		//温度小数部分
	char HumH;		//湿度整数部分
	char HumL;		//湿度小数部分
}DHT11_TypeDef;

//DHT11全局变量声明
extern 		DHT11_TypeDef 	DHT11_Variable;

void DHT11_OutPut(void);

void DHT11_InPut(void);

void DHT11_Init(void);

unsigned char DHT11_ReadBit(void);

unsigned char DHT11_ReadByte(void);

unsigned char DHT11_ReadValue(void);

#endif


