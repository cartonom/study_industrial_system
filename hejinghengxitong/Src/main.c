/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "oled.h"
#include "oledfont.h"
#include "bmp.h"
#include "dht11.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int page = 0;//页面编号
int pagestate = 0;//页面状态
int timeis1s = 0;//计时1秒的标记
uint16_t adcvalue = 0;//用于存放转换的adc值
int switchcvalue;//用于存放开关量的值
int lednum = 0;//led流水灯计数
int timeFlag1s_led = 0;//流水灯时钟
char rx_buffer;
char rx_buffer_string[100];
int rx_buffer_index=0;
int u1rxFlag1s=0;//1秒钟发一次
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void oledshowopen(void);//开机界面显示函数声明
void change_Page(void);//确定键切换页面
void showPageBackground(void);//显示页面背景
void oledshowmain(int pagestate);//主界面显示函数声明
void oledshowsensor(int pagestate);//传感器界面显示函数声明
void oledzhixingqi(int pagestate);//流水灯界面函数声明
void oledpwm(int pagestate);//pwm界面显示
void oleduarting(int pagestate);//串口界面显示
void oledzidongkongzhi(int pagestate);//自动控界面显示

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
//初始化OLED屏
IIC_OLED_Init();
//显示初始化界面
oledshowopen();
//启动adc1
HAL_ADC_Start(&hadc1);
//启动定时器
HAL_TIM_Base_Start_IT(&htim4);
//DHT11初始化
DHT11_Init();
//DHT11初始化
MX_TIM2_Init();
//启动二定时器1通道
HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
//初始化串口
MX_USART1_UART_Init();
//启动串口
HAL_UART_Transmit(&huart1,(uint8_t*)"start\r\n",strlen("start\r\n"),100);
HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_buffer, 1);//启动串口1通道
//
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			//检测确定键是否按下
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == GPIO_PIN_RESET)//PB0电位低，说明按键按下了
		{
			while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)== GPIO_PIN_RESET)//按着就卡在这里,松开后继续
				;
			change_Page();//切换页面		
			showPageBackground();//显示对应页面
		}
		
		//检测选择键是否按下
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET)//PB1电位低，说明按键按下了
		{
			while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)== GPIO_PIN_RESET)//按着就卡在这里,松开后继续
				;
			pagestate++;//选择下一状态
			if(page == 1 && pagestate > 5)//如果是1主页面，状态大于5
			{
				pagestate = 0;//回到0初始状态
			}else if(page == 2 && pagestate > 4)//如果是2传感器页面，状态大于4
			{
				pagestate = 0;//回到0初始状态
			}else if(page == 3 && pagestate > 3)//如果是3灯页面，状态大于3
			{
				pagestate = 0;//回到0初始状态
			}else if(page == 4 && pagestate > 4)//如果是4pwm页面，状态大于4
			{
				pagestate = 0;//回到0初始状态
			}else if(page == 5 && pagestate > 1)//如果是5串口页面，状态大于1
			{
				pagestate = 0;//回到0初始状态
			}else if(page == 6 && pagestate > 1)//如果是6自动控制页面，状态大于1
			{
				pagestate = 0;//回到0初始状态
			}
			showPageBackground();//显示对应页面
		}
			
		//传感器功能
		if(timeis1s == 1)//计时1秒到了
		{
			timeis1s = 0;//清空本次计时，方便下一次计时使用
			//采集adc
	    HAL_ADC_Start(&hadc1);//启动adc1		
			HAL_ADC_PollForConversion(&hadc1,100);//等待adc1采样和转换，最多等100ms
			adcvalue = HAL_ADC_GetValue(&hadc1);//获取adc的值，放入变量adcvalue
			//温湿度传感器
			DHT11_ReadValue();//采集温湿度
			switchcvalue = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6);//开关数据
			if(page == 2)//如果是2传感器页面
			{
				switch(pagestate)
				{
					case 0://0温度传感器
						IIC_OLED_ShowNum(42,6,(uint32_t)DHT11_Variable.TemH,3);
						break;
					case 1://1湿度传感器
						IIC_OLED_ShowNum(42,6,(uint32_t)DHT11_Variable.HumH,3);
						break;
					case 2://2光照传感器
						IIC_OLED_ShowNum(40,6,adcvalue,5);
						break;
					case 3://3开关传感器	
						IIC_OLED_ShowNum(64,6,switchcvalue,1);
						break;
				}
			}
		}
		//流水灯
		if(page==3){	
			switch(pagestate)
			{
				case 0://假如是1模式
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);	//全灭
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);	//全灭
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);	//全灭
				break;
		
				case 1://假如是2模式
			if( timeFlag1s_led == 1)//如果一秒到了
		  {
				timeFlag1s_led = 0;//清楚本次一秒的标记
				HAL_GPIO_WritePin(GPIOA,0xff,GPIO_PIN_SET);//全灭
				if(lednum == 3)
				{
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);//亮PA3
				}else if(lednum == 4)
				{
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);//亮PA4
				}else if(lednum == 5)
				{
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);//亮PA5
				}
				lednum++;//切换到下一个灯
				if(lednum>5)//超出范围
				{
					lednum = 3;//回到初始分支
				}
		  }
			break;
				case 2://假如是3模式
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);	//全亮
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);	//全亮
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);	//全亮
			break;		
			
		}
	}
		
	
		//pwm
	if(page==4){
	switch(pagestate)
			{
		case 0:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
		break;
		case 1:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 20);
		break;
		case 2:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 50);
		break;
		case 3:
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 80);
		break;
			}
		}
	
	//串口
		if(page==5)
		{
			if(u1rxFlag1s==1)
				{
				u1rxFlag1s=0;	
			//HAL_UART_Transmit(&huart1,(uint8_t*)"victory\r\n",strlen("victory\r\n"),100);
				HAL_UART_Transmit(&huart1,(uint8_t*)rx_buffer_string,strlen(rx_buffer_string),100);
				IIC_OLED_Clear();//擦除页面
				IIC_OLED_ShowString(0,0,rx_buffer_string);
				//IIC_OLED_Clear();//擦除页面
				memset(rx_buffer_string,0,sizeof(rx_buffer_string));
				rx_buffer_index=0;
				HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_buffer, 1);//启动串口1通道
				
				}
				
		}
			
//自动控制
			if(timeis1s == 1)//计时1秒到了
		{
			timeis1s = 0;//清空本次计时，方便下一次计时使用
			//采集adc
	    HAL_ADC_Start(&hadc1);//启动adc1		
			HAL_ADC_PollForConversion(&hadc1,100);//等待adc1采样和转换，最多等100ms
			adcvalue = HAL_ADC_GetValue(&hadc1);//获取adc的值，放入变量adcvalue
			//温湿度传感器
			DHT11_ReadValue();//采集温湿度
			switchcvalue = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6);//开关数据
				if(page==6)
				{
						if(DHT11_Variable.TemH>30)
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);//GPIOB12亮
						}else
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);//GPIOB12灭
						}
						
						if(DHT11_Variable.HumH>30)
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);//GPIOB13亮
						}else
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);//GPIOB13灭
						}
						
						if(adcvalue>2000)
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);//GPIOB14亮
						}else
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);//GPIOB14灭
						}
						
						if(switchcvalue==0)
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);//GPIOB15亮
						}else
						{
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET);//GPIOB15灭
						}
							}
				}		
				
		
				
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//确定键切换页面
void change_Page(void)
{
	if(page == 0)//在0开机界面
		{
			page = 1;//跳到1主界面
			pagestate = 0;//主界面的0状态，传感器状态
		}else if(page == 1 && pagestate == 0)//在1主界面，0传感器状态
			{
				page = 2;//跳到2传感器界面
				pagestate = 0;//状态0，温度传感器
			}else if(page == 2 && pagestate == 4)//在2传感器界面，4返回状态
				{
					page = 1;//跳到1主界面
					pagestate = 0;//主界面的0状态，传感器状态
				}
					else if(page == 1 && pagestate == 1)//在1主界面，1执行器状态
				{
					page = 3;//3执行器界面
					pagestate = 0;//执行器面的0状态，关灯
				}
				else if(page == 3 && pagestate == 3)//3执行器界面3返回状态
				{
					page = 1;//跳到1主界面
					pagestate = 1;//主界面的1状态，执行器状态
				}else if(page == 1 && pagestate == 2)//在1主界面，2pwm状态
				{
					page = 4;//跳到1主界面
					pagestate = 0;//主界面的0状态，传感器状态
				}else if(page == 4 && pagestate == 4)//4pwm界面4返回状态
				{
					page = 1;//跳到1主界面
					pagestate = 2;//主界面的1，pwm状态
				}else if(page == 1 && pagestate == 3)//1主界面，3串口界面
				{
					page = 5;//跳到pwm主界面
					pagestate = 0;//主界面的1，串口状态
				}else if(page == 5 && pagestate == 1)//5串口界面，1返回状态
				{
						page = 1;//跳到1主界面
					pagestate = 3;//主界面的1，串口状态
				}else if(page == 1 && pagestate == 4)//1主界面，4自动控制界面
				{
					page = 6;//跳到自动控制主界面
					pagestate = 0;//主界面的1，自动控制状态
				}else if(page == 6 && pagestate == 1)//6自动控制界面，1返回状态
				{
						page = 1;//跳到1主界面
					pagestate = 4;//主界面的1，自动控制状态
				}
			}

//显示页面背景
void showPageBackground(void)
{
	if(page == 1)//如果是1主界面
	{
		 IIC_OLED_Clear();//擦除页面
		oledshowmain(pagestate);//显示主界面				
	}else if(page == 2)//如果是2传感器界面
	{
		 IIC_OLED_Clear();//擦除页面
		oledshowsensor(pagestate);//显示传感器界面				
	}else if(page == 3)//如果是3执行器界面
	{
		 IIC_OLED_Clear();//擦除页面
		oledzhixingqi(pagestate);//显示执行器界面
	}else if(page == 4)//如果是4pwm界面
	{
		 IIC_OLED_Clear();//擦除页面
		oledpwm(pagestate);//显示pwm界面
	}else if(page == 5)//如果是5串口界面
	{
		 IIC_OLED_Clear();//擦除页面
		oleduarting(pagestate);//显示串口界面
	}else if(page == 6)//如果是6自动控制界面
	{
		 IIC_OLED_Clear();//擦除页面
		oledzidongkongzhi(pagestate);//显示自动控制界面
	}							
}

//定时器时间到了，中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim4){
			timeis1s=1;//计时1s标记
			timeFlag1s_led = 1;//流水灯时钟
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//串口接收中断
{
	if(huart->Instance == USART1)
	{
  rx_buffer_string[rx_buffer_index++]=rx_buffer;
	
		if(rx_buffer=='#')
		{
		u1rxFlag1s=1;
		}else{
		HAL_UART_Receive_IT(&huart1, (uint8_t*)&rx_buffer, 1);
		}
	}
}

//显示0开始界面
void oledshowopen(void)
{
	IIC_OLED_ShowCHinese(16,0,8);//智
	IIC_OLED_ShowCHinese(32,0,9);//能
	IIC_OLED_ShowCHinese(48,0,10);//门
	IIC_OLED_ShowCHinese(64,0,11);//锁
	IIC_OLED_ShowCHinese(80,0,12);//系
	IIC_OLED_ShowCHinese(96,0,13);//统
	
	IIC_OLED_ShowCHinese(48,3,14);//何
	IIC_OLED_ShowCHinese(64,3,15);//京
	IIC_OLED_ShowCHinese(80,3,16);//恒
	
	//IIC_OLED_ShowCHinese(0,5,17);//请
	IIC_OLED_ShowCHinese(0,5,18);//按
	IIC_OLED_ShowCHinese(16,5,19);//确
	IIC_OLED_ShowCHinese(32,5,20);//定
	IIC_OLED_ShowCHinese(48,5,21);//键
	IIC_OLED_ShowCHinese(64,5,22);//进
	IIC_OLED_ShowCHinese(80,5,23);//入
	IIC_OLED_ShowCHinese(96,5,24);//系
	IIC_OLED_ShowCHinese(112,5,25);//统
}

//显示主界面
void oledshowmain(int pagestate)
{
	IIC_OLED_ShowCHinese(56-16,0,26);//主
	IIC_OLED_ShowCHinese(56,0,27);//界
	IIC_OLED_ShowCHinese(56+16,0,28);//面
	switch(pagestate)
	{
		case 0://传感器
			IIC_OLED_ShowCHinese(56-16,6,29);//传
			IIC_OLED_ShowCHinese(56,6,30);//感
			IIC_OLED_ShowCHinese(56+16,6,31);//器
			IIC_OLED_DrawBMP(48,2,48+32,2+4,gImage_chuanganqi);
		break;
		case 1://灯
			IIC_OLED_ShowCHinese(56,6,32);//灯
			IIC_OLED_DrawBMP(48,2,48+29,2+4,gImage_led);
		break;
		case 2://pwm
			IIC_OLED_ShowString(56,6,"pwm");
		IIC_OLED_DrawBMP(48,2,48+117,2+4,gImage_pwm);
		break;
		case 3://串口
			IIC_OLED_ShowCHinese(56-16,6,33);//串
			IIC_OLED_ShowCHinese(56+16,6,34);//口
			IIC_OLED_DrawBMP(40,2,40+52,2+4,gImage_uart);
		break;
		case 4://自动控制
				IIC_OLED_ShowCHinese(56-16-16,6,35);//自
			IIC_OLED_ShowCHinese(56-16,6,36);//动
			IIC_OLED_ShowCHinese(56,6,37);//控
		IIC_OLED_ShowCHinese(56+16,6,38);//制
			IIC_OLED_DrawBMP(48,2,48+32,2+4,gImage_auto);
		break;
		case 5://作者
			IIC_OLED_ShowCHinese(56-16,6,14);//何
			IIC_OLED_ShowCHinese(56,6,15);//京
			IIC_OLED_ShowCHinese(56+16,6,16);//恒
			IIC_OLED_ShowString(56+16+16,6,"vx");
			IIC_OLED_DrawBMP(48,2,48+32,2+4,gImage_vx);
		break;
		default:
				IIC_OLED_ShowString(0,0,"error");
	}
}

//显示2传感器(chuanganqi)界面
void oledshowsensor(int pagestate)
{
	switch(pagestate)
	{
		case 0:
			IIC_OLED_DrawBMP(40,0,40+48,6,gImage_tmp);
			break;
		case 1:
			IIC_OLED_DrawBMP(40,0,40+48,6,gImage_hum);
			break;
		case 2:
			IIC_OLED_DrawBMP(40,0,40+48,6,gImage_sun);
			break;
		case 3:
			IIC_OLED_DrawBMP(40,0,40+48,6,gImage_kaiguan);
			break;
		case 4:
			IIC_OLED_DrawBMP(40,0,40+60,6,gImage_back);
			break;
		default:
			IIC_OLED_ShowString(0,0,"error");	
	}
}

//显示执行器灯的界面
void oledzhixingqi(int pagestate)
{
	switch(pagestate)
	{
		case 0:
			IIC_OLED_ShowCHinese(48,3,39);//全
			IIC_OLED_ShowCHinese(64,3,40);//灭
		break;
		case 1:
			IIC_OLED_ShowCHinese(48,3,41);//流
			IIC_OLED_ShowCHinese(64,3,42);//水
		break;
		case 2:
			IIC_OLED_ShowCHinese(48,3,39);//全
			IIC_OLED_ShowCHinese(64,3,43);//亮
		break;
		case 3:
			IIC_OLED_DrawBMP(40,0,40+60,6,gImage_back);
		break;
		default:
			IIC_OLED_ShowString(0,0,"error");	
	}
}

//显示pwm界面
void oledpwm(int pagestate)
{
	switch(pagestate)
	{
		case 0:
			IIC_OLED_ShowString(48,3,"PWM-0");
		break;
		case 1:
			IIC_OLED_ShowString(48,3,"PWM-20");
		break;
		case 2:
			IIC_OLED_ShowString(48,3,"PWM-50");
		break;
		case 3:
			IIC_OLED_ShowString(48,3,"PWM-80");
		break;
		case 4:
			IIC_OLED_DrawBMP(40,0,40+60,6,gImage_back);
		break;
		default:
			IIC_OLED_ShowString(0,0,"error");	
	}
}
//串口界面
void oleduarting(int pagestate)
{
	switch(pagestate)
	{
		case 0:
			IIC_OLED_DrawBMP(30,0,30+68,6,gImage_uarting);
		break;
		case 1:
			IIC_OLED_DrawBMP(40,0,40+60,6,gImage_back);
		break;
		default:
			IIC_OLED_ShowString(0,0,"error");	
	}
}

//自动控制界面
void oledzidongkongzhi(int pagestate)
{
	switch(pagestate)
	{
		case 0:
			IIC_OLED_ShowCHinese(48-32,3,44);//温
			IIC_OLED_ShowCHinese(48-16,3,45);//度
			IIC_OLED_ShowCHinese(48,3,46);//检
			IIC_OLED_ShowCHinese(48+16,3,47);//测
			IIC_OLED_ShowCHinese(48+32,3,48);//中
		break;
		case 1:
			IIC_OLED_DrawBMP(40,0,40+60,6,gImage_back);
		break;
		default:
			IIC_OLED_ShowString(0,0,"error");	
	}

}


	

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
