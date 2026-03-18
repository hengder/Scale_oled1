/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// 假设你目前桌面上测试用的还是 500g 砝码
#define TEST_STANDARD_WEIGHT  500.0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_CAN_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	OLED_Fill(0,0,256,64,0x00);
	OLED_ShowString(56,28,"Power On Self Test",16,0);
	Delay_ms(500);
	/************  调试串口初始化  ************/
	Serial_Init(); 
	printf("System Boot OK! High Precision Scale Starting...\r\n");
	/*************  OLED初始化  *************/
	OLED_Init();
	LED_On();
	/************  AD7195相关初始化  ************/
  AD7195_Init();			// 1. 初始化底层外设
	AD_Values_Init();		// 2. 初始化 ADC 数据滤波层
	AD7195_Debug_Dump();// 新增调试：打印初始化后的真实状态

  Scale_App_Init();		// 3. 初始化电子秤应用层

//	printf("\r\n===================================\r\n");
//  printf("高精度电子秤 串口模拟 CAN 测试程序\r\n");
//  printf("===================================\r\n");
//  printf("请通过串口发送以下字符进行指令测试：\r\n");
//  printf(" [1] -> 零点标定 (空秤盘时发送)\r\n");
//  printf(" [2] -> 满量程标定 (放上 %.0f g 砝码后发送)\r\n", TEST_STANDARD_WEIGHT);
//  printf(" [3] -> 去皮 (扣除当前重量)\r\n");
//  printf(" [4] -> 清除皮重 (恢复毛重显示)\r\n");
//  printf(" [5] -> 日常清零 (应对微小底噪漂移)\r\n");
//  printf("===================================\r\n\r\n");
//  printf("等待标定... (标定前暂不输出重量)\r\n");

//  uint32_t last_print_time = HAL_GetTick();
//  uint8_t is_calibrated = 0; // 【新增】：控制是否允许显示示数的标志位
		
	OLED_ShowString(104,28,"Finish",16,0);
//	Delay_ms(1000);
//	OLED_Show_Test();
	Delay_ms(500);
	OLED_Fill(0,0,256,64,0x00);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
			OLED_Update_Scale_UI();
//				/* ----------------------------------------------------------- *
//         * 任务 1：处理串口中断接收到的数据 (完美对接你写的 Serial.c)
//         * ----------------------------------------------------------- */
//        // 只要你的接收长度大于 0，说明中断里抓到了新数据
//        if (Serial_RxLength > 0) 
//        {
//            uint8_t rx_cmd = Serial_RxBuffer[0]; // 提取收到的第一个字符
//            
//            // 无论收到什么，立刻清空你的缓冲区和标志位，准备迎接下一个指令
//            Serial_ClearRxBuffer(); 

//            // 过滤掉串口助手可能自动发送的回车(\r)或换行(\n)等无关字符
//            if (rx_cmd >= '1' && rx_cmd <= '5')
//            {
//                printf("\r\n>>> 收到上位机指令: '%c' <<<\r\n", rx_cmd);

//                switch(rx_cmd)
//                {
//                    case '1':
//                        printf("执行: 零点标定中...\r\n");
//                        Scale_Calibrate_Zero();
//                        printf("-> 零点标定完成！开启示数显示...\r\n\r\n");
//                        is_calibrated = 1; 
//                        break;
//                        
//                    case '2':
//                        printf("执行: 满量程标定中 (%.1f g)...\r\n", TEST_STANDARD_WEIGHT);
//                        Scale_Calibrate_Full(TEST_STANDARD_WEIGHT);
//                        printf("-> 满量程标定完成！开启示数显示...\r\n\r\n");
//                        is_calibrated = 1; 
//                        break;
//                        
//                    case '3':
//                        printf("执行: 正在去皮...\r\n");
//                        Scale_Tare();
//                        printf("-> 去皮完成！\r\n\r\n");
//                        break;
//                        
//                    case '4':
//                        printf("执行: 清除皮重...\r\n");
//                        Scale_Clear_Tare();
//                        printf("-> 皮重已清除！\r\n\r\n");
//                        break;
//                        
//                    case '5':
//                        printf("执行: 日常清零...\r\n");
//                        Scale_Zeroing();
//                        printf("-> 清零完成！\r\n\r\n");
//                        break;
//                }
//            }
//        }
//        /* ----------------------------------------------------------- *
//         * 任务 2：处理板载物理按键 (本地去皮功能)
//         * ----------------------------------------------------------- */
//        if (Button_Event_Flag == 1)
//        {
//            Button_Event_Flag = 0; // 清除标志位
//            if (is_calibrated)     // 只有标定过后按键才生效
//            {
//                printf("\r\n[按键触发] 执行本地去皮...\r\n");
//                Scale_Tare();
//                printf("-> 去皮完成！\r\n\r\n");
//            }
//        }

//        /* ----------------------------------------------------------- *
//         * 任务 3：定时获取重量并上报 (加入 is_calibrated 判断)
//         * ----------------------------------------------------------- */
//        if (is_calibrated && ((HAL_GetTick() - last_print_time) >= 200)) 
//        {
//            last_print_time = HAL_GetTick();

//            double net_weight   = Scale_Get_Net_Weight();    // 净重
//            double gross_weight = Scale_Get_Gross_Weight();  // 毛重

//            printf("【状态】 净重: %8.1f g  |  毛重: %8.1f g\r\n", net_weight, gross_weight);
//        }
//        
//        /* ----------------------------------------------------------- *
//         * 任务 4：保持底层 ADC 抽取，防止芯片数据溢出
//         * ----------------------------------------------------------- */
//        // 无论是否标定，底层必须持续抽样，保证滤波器里永远是最新数据
//        if (!is_calibrated || ((HAL_GetTick() - last_print_time) < 200))
//        {
//             AD_Get_FilteredValue();
//				}
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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
}

/* USER CODE BEGIN 4 */

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
