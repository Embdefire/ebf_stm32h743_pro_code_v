/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   GPIO����һ�������
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H743 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./delay/core_delay.h" 
#include "./key/bsp_key.h" 
#include "./wwdg/bsp_wwdg.h" 
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	uint8_t wwdg_tr, wwdg_wr;
	
	/* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	//��鴰�ڿ��Ź���λ��־λ
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST) != RESET)
	{
		// ���Ź���λ��������ɫ����
		LED_RED;

		//�����λ��־λ
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}
	else
	{
		// ������������ɫ����
		LED_BLUE;
	}
	HAL_Delay(500);
	LED_RGBOFF;
	HAL_Delay(500);

	// WWDG����	
	// ��ʼ��WWDG�����ü�������ʼֵ�������ϴ���ֵ������WWDG��ʹ����ǰ�����ж�
	WWDG_Config(127,80,WWDG_PRESCALER_8);	
	
	// ����ֵ�����ڳ�ʼ����ʱ�����ó�0X5F�����ֵ����ı�
	wwdg_wr = WWDG1->CFR & 0X7F;

	while(1)
	{	

		//-----------------------------------------------------
		// �ⲿ��Ӧ��д��Ҫ��WWDG��صĳ�����γ������е�ʱ��
		// �����˴���ֵӦ�����óɶ��
		//-----------------------------------------------------
		// ��ʱ��ֵ����ʼ�������0X7F��������WWDGʱ�����ֵ�᲻�ϼ�С
		// ����������ֵ���ڴ���ֵʱι���Ļ����Ḵλ�������������ٵ�0X40
		// ��û��ι���Ļ��ͷǳ��ǳ�Σ���ˣ��������ټ�һ�ε���0X3Fʱ�͸�λ
		// ����Ҫ����������ֵ�ڴ���ֵ��0X40֮���ʱ��ι��������0X40�ǹ̶��ġ�
		wwdg_tr = WWDG1->CR & 0X7F;
		if( wwdg_tr == wwdg_wr)
		{
			// ι�����������ü�������ֵΪ���0X7F
			WWDG_Feed();
			// ����ι������ɫ����˸
			LED2_TOGGLE;
		}
	}
}
/**
  * @brief  System Clock ����
  *         system Clock ��������: 
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
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** ���õ�Դ���ø���
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** ����������ѹ�������ѹ
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** ��ʼ��CPU��AHB��APB����ʱ��
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
  }
  /** ��ʼ��CPU��AHB��APB����ʱ��
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
  }
}
/****************************END OF FILE***************************/
