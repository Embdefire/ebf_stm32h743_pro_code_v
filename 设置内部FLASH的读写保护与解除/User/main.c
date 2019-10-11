/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   GPIO���--ʹ�ù̼������LED��
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H743������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
/*	
�� �������������裺

1.����DAP�����������debug��ť����ʹ������RAM�����У�Ȼ�����ĵȴ�����LED������ת��
 ������������2�������ң��ȴ���5���ӻ���ת��˵���ָ�ʧ�ܣ�
2.LED�Ʊ�Ϊ�̵ƺ󣬴������������س���STM32��FLASH(����ͨ����ˮ������)
3.�������޷����أ�������1-2���衣
4.��λ�����壬�۲��µĳ����Ƿ�����(����ˮ������ʱ��LED�Ƿ�������˸)
5.��ɡ�	
*/	  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./delay/core_delay.h"
#include "./usart/bsp_debug_usart.h"
#include "./internalFlash/internalFlash_reset.h"  

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	/* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
	/* LED �˿ڳ�ʼ�� */
  LED_GPIO_Config();
	/* ���ڳ�ʼ�� */
	DEBUG_USART_Config();
	LED_BLUE;
	
	FLASH_INFO("�����򽫻ᱻ���ص�STM32���ڲ�SRAM���С�");

	FLASH_INFO("\r\n");
	FLASH_INFO("----����һ��STM32оƬ�ڲ�FLASH��������----"); 
	FLASH_INFO("������оƬ���ڲ�FLASHѡ���ֽڻָ�ΪĬ��ֵ"); 
	
	
	#if 0  //���̵��ԡ���ʾʱʹ�ã��������ʱ����Ҫ���д˺���
	WriteProtect_Test(); //�޸�д����λ������оƬ���������ó�д�����ĵĻ���
	#endif

	OptionByte_Info();
	
	/*�ָ�ѡ���ֽڵ�Ĭ��ֵ���������*/
	if(InternalFlash_Reset()==HAL_OK)
	{		
		FLASH_INFO("ѡ���ֽڻָ��ɹ�");
		FLASH_INFO("Ȼ�������һ����ͨ�ĳ������س���оƬ���ڲ�FLASH���в���"); 
		LED_GREEN;
	}	
	else
	{			
		FLASH_INFO("ѡ���ֽڻָ��ɹ�ʧ�ܣ��븴λ����");
		LED_RED;
	}

	OptionByte_Info();	

	while (1)
	{
	}
}

/**
  * @brief  System Clock ����
  *         system Clock ��������: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*ʹ�ܹ������ø��� */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* ��������ʱ��Ƶ�ʵ������ϵͳƵ��ʱ����ѹ���ڿ����Ż����ģ�
		 ����ϵͳƵ�ʵĵ�ѹ����ֵ�ĸ��¿��Բο���Ʒ�����ֲᡣ  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* ����HSE������ʹ��HSE��ΪԴ����PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {

    while(1) { ; }
  }
  
	/* ѡ��PLL��Ϊϵͳʱ��Դ����������ʱ�ӷ�Ƶ�� */
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
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
/****************************END OF FILE***************************/
