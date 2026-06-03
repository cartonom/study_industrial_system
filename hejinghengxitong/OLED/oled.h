#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f1xx_hal.h"

#define Max_Column	128
#define OLED_CMD  0	//命令模式
#define OLED_DATA 1	//数据模式

/*I2C引脚定义*/
#define   OLED_SCL_PORT               GPIOB
#define   OLED_SCL_PIN                GPIO_PIN_8
#define   OLED_SDA_PORT               GPIOB
#define   OLED_SDA_PIN                GPIO_PIN_9



/*I2C引脚操作*/
#define OLED_SCLK_Set()		HAL_GPIO_WritePin(OLED_SCL_PORT, OLED_SCL_PIN, GPIO_PIN_SET)
#define OLED_SDIN_Set()		HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, GPIO_PIN_SET)
#define OLED_SCLK_Clr()		HAL_GPIO_WritePin(OLED_SCL_PORT, OLED_SCL_PIN, GPIO_PIN_RESET)
#define OLED_SDIN_Clr()		HAL_GPIO_WritePin(OLED_SDA_PORT, OLED_SDA_PIN, GPIO_PIN_RESET)

/*OLED操作函数声明*/
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Wait_Ack(void);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
void IIC_OLED_WR_Byte(uint8_t dat,uint8_t cmd);  
void IIC_OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);

void IIC_OLED_Display_On(void);
void IIC_OLED_Display_Off(void);	   							   		    
void IIC_OLED_Init(void);
void IIC_OLED_Clear(void);

void IIC_OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);
void IIC_OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len);
void IIC_OLED_ShowString(uint8_t x,uint8_t y, char *p);	 
void IIC_OLED_Set_Pos(unsigned char x, unsigned char y);
void IIC_OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void IIC_OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void IIC_OLED_DrawFont16(uint16_t x, uint16_t y, char *s);
void IIC_OLED_DrawFont32(uint16_t x, uint16_t y, char *s);
void IIC_OLED_Show_Str(uint16_t x, uint16_t y, char *str,uint8_t size);

#endif
