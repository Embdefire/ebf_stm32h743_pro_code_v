/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   DMA��ֱ�Ӵ洢����
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
#include "./mpu/bsp_mpu.h" 
#include "./usart/bsp_usart_dma.h"

extern DMA_HandleTypeDef DMA_Handle;
uint8_t SendBuff[SENDBUFF_SIZE]  __attribute__((at(0x30000000)));  

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{  
  uint16_t i;
	/* ϵͳʱ�ӳ�ʼ����480MHz */
	SystemClock_Config();
  
    /* Ĭ�ϲ����� MPU������Ҫ�������ܣ������� MPU ��ʹ�� 
   DMA ʱ��ע�� Cache �� �ڴ�����һ���Ե����⣬
   ����ע��������ο����׽̵̳� MPU ��������½� */
  
//  SCB_EnableICache();    // ʹ��ָ�� Cache
//  SCB_EnableDCache();    // ʹ������ Cache
  
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
  /* ���ڳ�ʼ�� */
  DEBUG_USART_Config();
  /* DMA ��ʼ�� */
  USART_DMA_Config();
	printf("\r\n USART1 DMA TX ���� \r\n");
  
  /*��佫Ҫ���͵�����*/
  for(i=0;i<SENDBUFF_SIZE;i++)
  {
    SendBuff[i]	 = 'A';    
  }
  

  /*Ϊ��ʾDMA�������ж�CPU���ܴ����������飬����ʹ��DMA�������ݣ����ǳ���
  *��ʱ�����п��ܻᵼ�µ��Զ˴��ڵ������ֻῨ��������ҷɵ������
  *���DMA�����е�ѭ��ģʽ��Ϊ����ģʽ*/
  
   HAL_UART_Transmit_DMA (&UartHandle,(uint8_t *)SendBuff,SENDBUFF_SIZE);
  /* ��ʱCPU�ǿ��еģ����Ը����������� */  
  //����ͬʱ����LED
	while (1)
	{
		LED1_TOGGLE
    HAL_Delay(1000);
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
	*            PLL_Q                = 2
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
