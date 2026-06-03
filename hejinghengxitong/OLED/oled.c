#include "oled.h"
#include "oledfont.h"          //头文件



/*引脚初始化*/
void OLED_I2C_Init(void)
{	
	OLED_SCLK_Set();
	OLED_SDIN_Set();
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void  IIC_Start(void)
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void IIC_Stop(void)
{
	OLED_SDIN_Clr();
	OLED_SCLK_Set();
	OLED_SDIN_Set();
}

/**
  * @brief  I2C等待
  * @param  无
  * @retval 无
  */
void IIC_Wait_Ack()
{
    OLED_SCLK_Set();
    OLED_SCLK_Clr();
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void Write_IIC_Byte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOB, OLED_SDA_PIN, (GPIO_PinState)(Byte & (0x80 >> i)));
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
	OLED_SCLK_Set();	//额外的一个时钟，不处理应答信号
	OLED_SCLK_Clr();
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void Write_IIC_Command(uint8_t Command)
{
	 IIC_Start();
	Write_IIC_Byte(0x78);		//从机地址
	Write_IIC_Byte(0x00);		//写命令
	Write_IIC_Byte(Command); 
	IIC_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void Write_IIC_Data(uint8_t Data)
{
	 IIC_Start();
	Write_IIC_Byte(0x78);		//从机地址
	Write_IIC_Byte(0x40);		//写数据
	Write_IIC_Byte(Data);
	IIC_Stop();
}

/**
  * @brief  OLED写内容
  * @param  Data 要写入的数据
  * @param  Command 要写入的命令
  * @retval 无
  */
void IIC_OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    if (cmd)
    {
        Write_IIC_Data(dat);
    }
    else
    {
        Write_IIC_Command(dat);
    }
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void IIC_OLED_Set_Pos(uint8_t X, uint8_t Y)
{
	Write_IIC_Command(0xB0 | Y);					//设置Y位置
	Write_IIC_Command(0x10 | ((X & 0xF0) >> 4));	//设置X位置低4位
	Write_IIC_Command(0x00 | (X & 0x0F));			//设置X位置高4位
}

/**
  * @brief  开启OLED显示 
  * @retval 无
  */  
void IIC_OLED_Display_On(void)
{
	Write_IIC_Command(0X8D);  //SET DCDC命令
	Write_IIC_Command(0X14);  //DCDC ON
	Write_IIC_Command(0XAF);  //DISPLAY ON
}

/**
  * @brief  关闭OLED显示 
  * @retval 无
  */    
void IIC_OLED_Display_Off(void)
{
	Write_IIC_Command(0X8D);  //SET DCDC命令
	Write_IIC_Command(0X10);  //DCDC OFF
	Write_IIC_Command(0XAE);  //DISPLAY OFF
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void IIC_OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		IIC_OLED_Set_Pos(0, j);
		for(i = 0; i < 128; i++)
		{
			Write_IIC_Data(0x00);
		}
	}
}


// OLED 显示字符
// x:   行坐标 0~127
// y:   列坐标 0~63
// chr：ASCLL字符
void IIC_OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
    uint8_t t = 0, c=0;	  
    c = chr - ' '; 
    if (x > Max_Column - 1)
    {
        x = 0;
        y = y + 2;
    }
    IIC_OLED_Set_Pos(x,y);	
    //自左到右循环输入
    for(t=0;t<8;t++)
        IIC_OLED_WR_Byte(ASCII_1608[c][t],OLED_DATA);
    IIC_OLED_Set_Pos(x,y+1);	
    //自左到右循环输入
    for(t=0;t<8;t++)
        IIC_OLED_WR_Byte(ASCII_1608[c][8+t],OLED_DATA);
}
static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}
// OLED显示数字
// x:   行坐标 0~127
// y:   列坐标 0~63
// num：数字内容
// len：数字长度
void IIC_OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                IIC_OLED_ShowChar(x + 8 * t, y, ' ');
                continue;
            }
            else
                enshow = 1;
        }
        IIC_OLED_ShowChar(x + 8*t, y, temp + '0');
    }
}
// OLED显示字符串
// x:   行坐标 0~127
// y:   列坐标 0~63
// chr：ASCLL字符串
// size:字符大小 16
void IIC_OLED_ShowString(uint8_t x, uint8_t y, char *chr)
{
    unsigned char j = 0;
    while (chr[j] != '\0')
    {
        IIC_OLED_ShowChar(x, y, chr[j]);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}
// OLED显示汉字
// x:   行坐标
// y:   列坐标 
// no： 汉字序号（Hzk[]中）
void IIC_OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t t, adder = 0;
    IIC_OLED_Set_Pos(x, y);
    for (t = 0; t < 16; t++)
    {
        IIC_OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
        adder += 1;
    }
    IIC_OLED_Set_Pos(x, y + 1);
    for (t = 0; t < 16; t++)
    {
        IIC_OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
        adder += 1;
    }
}
// OLED显示图片
// x0:   行起始坐标
// y0:   列起始坐标 
// x1:   行终止坐标
// y1:   列终止坐标 
// BMP： 图片内容
void IIC_OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++)
    {
        IIC_OLED_Set_Pos(x0, y);
        for (x = x0; x < x1; x++)
        {
            IIC_OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}
///******************************************************************
//函数名：	OLED_DrawFont16
//功能：	显示单个16X16中文字体
//参数：	x,y :起点坐标 
//		s:字符串地址
//返回值：	无
//******************************************************************/
//void IIC_OLED_DrawFont16(uint16_t x, uint16_t y, char *s)
//{
//	uint8_t x0 = 0, y0 = 0;
//	uint16_t k = 0;
//	uint16_t HZnum = 0;
//	
//	//自动统计汉字数目
//	HZnum=sizeof(Tfont16)/sizeof(typFNT_GB16);	
//	
//	//循环寻找匹配的Index[2]成员值
//	for (k=0;k<HZnum;k++)
//	{
//		//对应成员值匹配
//		if((Tfont16[k].Index[0]==*(s))&&(Tfont16[k].Index[1]==*(s+1)))
//		{ 	
//			//x方向循环执行写16行，逐行式输入
//			for(y0=0;y0<2;y0++)
//			{
//				IIC_OLED_Set_Pos(x,y+y0);
//				//每行写入两个字节，自左到右
//				for(x0=0;x0<16;x0++)
//				{	
//					//一次写入1字节
//					IIC_OLED_WR_Byte(Tfont16[k].Msk[y0*16+x0],OLED_DATA);
//				}
//			}
//		//查找到对应点阵关键字完成绘字后立即break退出for循环，防止多个汉字重复取模显示		
//		break; 
//		}
//	}
//}

///******************************************************************
//函数名：	OLED_DrawFont32
//功能：	显示单个32X32中文字体
//参数：	x,y :起点坐标	 
//		s:字符串地址
//返回值：	无
//******************************************************************/ 
//void IIC_OLED_DrawFont32(uint16_t x, uint16_t y, char *s)
//{
//	uint8_t x0 = 0, y0 = 0;
//	uint16_t k = 0;
//	uint16_t HZnum = 0;
//	
//	//自动统计汉字数目
//	HZnum=sizeof(Tfont32)/sizeof(typFNT_GB32);	
//	
//	//循环寻找匹配的Index[2]成员值
//	for (k=0;k<HZnum;k++)
//	{
//		//对应成员值匹配
//		if((Tfont16[k].Index[0]==*(s))&&(Tfont16[k].Index[1]==*(s+1)))
//		{ 	
//			//x方向循环执行写16行，逐行式输入
//			for(y0=0;y0<4;y0++)
//			{
//				IIC_OLED_Set_Pos(x,y+y0);
//				//每行写入两个字节，自左到右
//				for(x0=0;x0<32;x0++)
//				{	
//					//一次写入1字节
//					IIC_OLED_WR_Byte(Tfont32[k].Msk[y0*32+x0],OLED_DATA);
//				}
//			}
//		//查找到对应点阵关键字完成绘字后立即break退出for循环，防止多个汉字重复取模显示		
//		break; 
//		}
//	}
//} 

///******************************************************************
//函数名： OLED_Show_Str
//功能：	显示一个字符串,包含中英文显示
//参数：	x,y :起点坐标
//		str :字符串	 
//		size:字体大小 16或32
//返回值：	无
//******************************************************************/ 
//void IIC_OLED_Show_Str(uint16_t x, uint16_t y, char *str,uint8_t size)
//{					
//	uint16_t x0 = x;
//  	uint8_t bHz = 0;				//字符或者中文，首先默认是字符
//	
//	if(size!=32)
//		size=16;					//默认1608
//	while(*str!=0)					//判断为否为结束符
//	{
//		if(!bHz)					//判断是字符
//		{
//			//如果x，y坐标超出预设lcd屏大小则换行显示
//			if(x>(128-size/2)) 
//			{
//				//显示靠前
//				x=0;
//				//显示换行
//				y=(y+size/8)%8;	
//			}
//			if(y>(8-size/8))
//			{
//				y=0;
//			}
//			if((uint8_t)*str>0x80)	//对显示的字符检查，判断是否为中文
//			{
//				bHz=1;				//判断为中文，则跳过显示字符改为显示中文
//			}
//			else			  		//确定为字符
//			{		  
//				if(*str==0x0D)		//判断是换行符号
//				{
//					y+=size;		//下一个显示的坐标换行
//					x=x0;			//显示靠前
//					str++;			//准备下一个字符
//				}
//				else				//判断不是换行符
//				{
//					//显示对应尺寸字符
//					IIC_OLED_ShowChar(x,y,*str);
//					//显示完后右移起始显示横坐标准备下次显示
//					x+=size/2;
//				}
//				//显示地址自增，准备下一个字符
//				str++; 
//			}
//		}
//		else						//判断是中文
//		{   
//			//如果x，y坐标超出预设lcd屏大小则换行显示
//			if(x>(128-size)) 
//			{
//				//显示靠前
//				x=0;
//				//显示换行
//				y=(y+size/8)%8;	
//			}
//			if(y>(8-size/8))
//			{
//				y=0;
//			}
//			bHz=0;					//改为默认字符用于下次字符判断 
//			if(size==32)			//判断是否为32X32大小的中文
//				//显示32X32大小的中文
//				IIC_OLED_DrawFont32(x,y,str);	 	
//			else if(size==16)		//否则为16X16大小的中文
//				//显示16X16大小的中文
//			IIC_OLED_DrawFont16(x,y,str);
//			//由于显示为中文，需要自增2个地址	
//			str+=2;	
//			//显示完后右移起始显示横坐标准备下次显示
//			x+=size;			
//		}
//	}
//}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void IIC_OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	Write_IIC_Command(0xAE);	//关闭显示
	
	Write_IIC_Command(0xD5);	//设置显示时钟分频比/振荡器频率
	Write_IIC_Command(0x80);
	
	Write_IIC_Command(0xA8);	//设置多路复用率
	Write_IIC_Command(0x3F);
	
	Write_IIC_Command(0xD3);	//设置显示偏移
	Write_IIC_Command(0x00);
	
	Write_IIC_Command(0x40);	//设置显示开始行
	
	Write_IIC_Command(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	Write_IIC_Command(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	Write_IIC_Command(0xDA);	//设置COM引脚硬件配置
	Write_IIC_Command(0x12);
	
	Write_IIC_Command(0x81);	//设置对比度控制
	Write_IIC_Command(0xCF);

	Write_IIC_Command(0xD9);	//设置预充电周期
	Write_IIC_Command(0xF1);

	Write_IIC_Command(0xDB);	//设置VCOMH取消选择级别
	Write_IIC_Command(0x30);

	Write_IIC_Command(0xA4);	//设置整个显示打开/关闭

	Write_IIC_Command(0xA6);	//设置正常/倒转显示

	Write_IIC_Command(0x8D);	//设置充电泵
	Write_IIC_Command(0x14);

	Write_IIC_Command(0xAF);	//开启显示
		
	IIC_OLED_Clear();				//OLED清屏
}
