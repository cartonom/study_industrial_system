#include "dht11.h"

DHT11_TypeDef DHT11_Variable;; //全局变量


/**
 * ************************************************************************
 * @brief DHT11引脚设置为推挽输出
 * 
 * ************************************************************************
 */
void DHT11_OutPut(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
 * ************************************************************************
 * @brief DHT11引脚设置为浮空输入
 * 
 * ************************************************************************
 */
void DHT11_InPut(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
 * ************************************************************************
 * @brief DHT11初始化
 * 
 * ************************************************************************
 */
void DHT11_Init(void)
{
	DHT11_OutPut();
	DHT11_Variable.TemH = 0;
	DHT11_Variable.TemL = 0;
	DHT11_Variable.HumH = 0;
	DHT11_Variable.HumL = 0; 		
}

/**
 * ************************************************************************
 * @brief 读取DHT11温湿度一位
 * 
 * @retval unsigned char	一位数据 0 或者 1
 * ************************************************************************
 */
unsigned char DHT11_ReadBit(void)
{
 	unsigned char timeout = 0;
	while(DHT11_GET && timeout < 100)//等待变为低电平
	{
		timeout ++;
		DHT11_DelayUs(1);
	}
	timeout = 0;
	while(!DHT11_GET && timeout < 100)//等待变高电平
	{
		timeout ++;
		DHT11_DelayUs(1);
	}
	DHT11_DelayUs(30);//等待30us
	if( DHT11_GET )
		return 1;
	else 
		return 0;	
}

/**
 * ************************************************************************
 * @brief 读取DHT11温湿度一个字节
 * 
 * @retval unsigned char	一个字节
 * ************************************************************************
 */
unsigned char DHT11_ReadByte(void)
{
	unsigned char i,retdata;
	retdata=0;
	for (i = 0 ; i < 8 ; i++) 
	{
		retdata <<= 1; 
		retdata |= DHT11_ReadBit();
	}						    
	return retdata;
}


/**
 * ************************************************************************
 * @brief 读取DHT11温湿度一个字节
 * 
 * @retval unsigned char	一位标是 0（获取失败） 或者 1（获取成功）
 * ************************************************************************
 */
unsigned char DHT11_ReadValue(void)
{
	unsigned short int timeout = 0;
	unsigned int temph, templ, humh, huml, check;

	//设置为IO口输出模式
	DHT11_OutPut();
	DHT11_SET_1;
	//MCU开始起始信号
	DHT11_SET_0;
	DHT11_DelayMs(20);		//拉低至少18ms	
	//设置为IO口输入模式
	DHT11_InPut();
	//检测应答信号
	timeout=0;
	while(DHT11_GET && ++timeout);
	if (timeout == 0) return 0;
	
	//总线由上拉电阻拉高 主机延时20us
	//主机设为输入 判断从机响应信号  
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  	 	
	timeout=0;
	while(!DHT11_GET && ++timeout);
	if (timeout == 0) return 0;	
	timeout=0;
	while(DHT11_GET && ++timeout);
	if (timeout == 0) return 0;
	
	//读取8bit的湿度整数数据
	humh = DHT11_ReadByte();
	//读取8bit的湿度小数数据
	huml = DHT11_ReadByte();
	//读取8bit的温度整数数据
	temph = DHT11_ReadByte();
	//读取8bit的温度小数数据
	templ = DHT11_ReadByte();
	//读取8bit的校验和
	check = DHT11_ReadByte();
	if(check == (humh + huml + temph + templ))
	{
		DHT11_Variable.HumH = humh;
		DHT11_Variable.HumL = huml;
		DHT11_Variable.TemH = temph;
		DHT11_Variable.TemL = templ;	
		return 1;
	}
	else
		return 0;
}

