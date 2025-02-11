/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   LTDC—液晶显示中文
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h"
#include "./sdram/bsp_sdram.h" 
#include "./lcd/bsp_lcd.h"
#include "string.h"
#include "./i2c/bsp_i2c.h"
#include "./camera/bsp_ov2640.h"
#include "./delay/core_delay.h"  
#include "./mpu/bsp_mpu.h"

/*简单任务管理*/
#define TASK_ENABLE 0
#define NumOfTask 3

#if USE_ExtFlash_Single
__IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);
#endif

extern uint32_t Task_Delay[NumOfTask];
uint32_t Task_Delay[NumOfTask]={0};
uint8_t dispBuf[100];
uint8_t fps=0;
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{  
	OV2640_IDTypeDef OV2640_Camera_ID;
	
	/* 系统时钟初始化成480MHz */
	SystemClock_Config();
	
  /* 默认不配置 MPU，若需要更高性能，当配置 MPU 后，使用 
   DMA 时需注意 Cache 与 内存内容一致性的问题，
   具体注意事项请参考配套教程的 MPU 配置相关章节 */
  Board_MPU_Config(0, MPU_Normal_WT, 0xD0000000, MPU_32MB);
  Board_MPU_Config(1, MPU_Normal_WT, 0x24000000, MPU_128KB);
	/* 开启I-Cache */
	SCB_EnableICache();
	/* 开启D-Cache */
	SCB_EnableDCache();
	/* 启动SysTick */
	SysTick_Init();
	
	/* LED 端口初始化 */
	LED_GPIO_Config();
	/* 配置串口1为：115200 8-N-1 */
	UARTx_Config();	
	
	printf("\r\n 欢迎使用野火  STM32 H743 开发板。\r\n");		 
	printf("\r\n野火STM32H743 OV2640摄像头测试例程\r\n");
	/*蓝灯亮，表示正在读写SDRAM测试*/
	LED_BLUE;
	/* LCD 端口初始化 */ 
	LCD_Init();
	/* LCD 第一层初始化 */ 
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,RGB565);
	/* LCD 第二层初始化 */ 
	LCD_LayerInit(1, LCD_FB_START_ADDRESS+(LCD_GetXSize()*LCD_GetYSize()*4),ARGB8888);
	/* 使能LCD，包括开背光 */ 
	LCD_DisplayOn(); 

	/* 选择LCD第一层 */
	LCD_SelectLayer(0);

	/* 第一层清屏，显示全蓝色 */ 
	LCD_Clear(LCD_COLOR_BLUE);  

	/* 选择LCD第二层 */
	LCD_SelectLayer(1);

	/* 第二层清屏，显示透明 */ 
	LCD_Clear(TRANSPARENCY);

	/* 配置第一和第二层的透明度,0位完全透明，255为完全不透明*/
	LCD_SetTransparency(0, 255);
	LCD_SetTransparency(1, 255);
	
	LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
	LCD_DisplayStringLine_EN_CH(1,(uint8_t* )" 模式:UXGA 800x480");
	CAMERA_DEBUG("STM32H743 DCMI 驱动OV2640例程");
	I2CMaster_Init();
	OV2640_HW_Init();			
	//初始化 I2C
	
	/* 读取摄像头芯片ID，确定摄像头正常连接 */
	OV2640_ReadID(&OV2640_Camera_ID);

	if(OV2640_Camera_ID.PIDH  == 0x26)
	{
		CAMERA_DEBUG("%x%x",OV2640_Camera_ID.PIDH ,OV2640_Camera_ID.PIDL);
	}
	else
	{
		LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
		LCD_DisplayStringLine_EN_CH(8,(uint8_t*) "         没有检测到OV2640，请重新检查连接。");
		CAMERA_DEBUG("没有检测到OV2640摄像头，请重新检查连接。");
		while(1);  
	}
		/* 配置摄像头输出像素格式 */
	OV2640_UXGAConfig();
	/* 初始化摄像头，捕获并显示图像 */
	OV2640_Init();
	//重置
	fps =0;
	Task_Delay[0]=1000;
	
	while(1)
	{
		if(Task_Delay[0]==0)
		{
			LCD_SelectLayer(1);       
			LCD_SetColors(LCD_COLOR_WHITE,TRANSPARENCY);
			sprintf((char*)dispBuf, " 帧率:%d FPS", fps/1);
			LCD_ClearLine(2);
			/*输出帧率*/
			LCD_DisplayStringLine_EN_CH(2,dispBuf);
			//重置
			fps =0;
			Task_Delay[0]=1000; //此值每1ms会减1，减到0才可以重新进来这里
			
		}		
	}  
}

/**
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 240000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  120MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  120MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  120MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  120MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 192
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 启用电源配置更新
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** 配置主内稳压器输出电压
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* 启用HSE振荡器并使用HSE作为源激活PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) { ; }
  }
  
	/* 选择PLL作为系统时钟源并配置总线时钟分频器 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | \
																 RCC_CLOCKTYPE_HCLK    | \
																 RCC_CLOCKTYPE_D1PCLK1 | \
																 RCC_CLOCKTYPE_PCLK1   | \
                                 RCC_CLOCKTYPE_PCLK2   | \
																 RCC_CLOCKTYPE_D3PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    while(1) { ; }
  }
}

/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (HAL_SYSTICK_Config(SystemCoreClock / 1000))
	{ 
		/* Capture error */ 
		while (1);
	}
}

/****************************END OF FILE***************************/
