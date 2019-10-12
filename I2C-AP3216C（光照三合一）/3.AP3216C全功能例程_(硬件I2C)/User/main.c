/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   AP3216C ������ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 H743 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "main.h"
#include "stm32h7xx.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"
#include "./delay/core_delay.h" 
#include "./i2c/bsp_i2c.h"
#include "./ap3216c/ap3216c.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
  /**/
  float ALS;
  uint16_t PS;
  uint16_t IR;
  uint8_t IntStatus;
  
  HAL_Init();        
  /* ����ϵͳʱ��Ϊ480 MHz */ 
  SystemClock_Config();
  LED_GPIO_Config();
  /* ��ʼ���ں���ʱ */
  HAL_InitTick(5);
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  UARTx_Config();
  
  printf("\r\n ��ӭʹ��Ұ�� STM32 H743 �����塣\r\n");	
  printf("\r\n ����һ������һ���մ������������� \r\n");
  
  printf(" оƬ��ʼ����.....\n");
  /* ��ʼ�� ���մ����� */
  ap3216c_init();

  while(1)    
  {
    IntStatus = ap3216c_get_IntStatus();    // �ȶ�״̬λ����ADC����λ�����״̬λ��Ĭ�����ã�
    ALS = ap3216c_read_ambient_light();
    PS = ap3216c_read_ps_data();
    IR = ap3216c_read_ir_data();

    printf("\n����ǿ���ǣ�%.2fLux\n����ǿ���ǣ�%d\n", ALS, IR);

    if (PS == 55555)    // IR ̫ǿ PS ������Ч
      printf("IR ̫ǿ PS ������Ч\n");
    else
    {
      printf("�ӽ������ǣ�%d\n", PS & 0x3FF);
    }
    
    if (AP_INT_Read() == 0)
      printf("���жϲ���\n");
    
    if ((PS >> 15) & 1)
      printf("����ӽ�\n");
    else
      printf("����Զ��\n");
    
    if (IntStatus & 0x1)
      printf("ALS �����ж�\n");
    
    if (IntStatus >> 1 & 0x1)
      printf("PS �����ж�\n");
    
    LED2_TOGGLE;
    HAL_Delay(200);
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
		while(1);
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
		while(1);
  }
}
/****************************END OF FILE***************************/

