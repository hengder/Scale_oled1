#include "Serial.h"

/* 定义全局变量 */
uint8_t Serial_RxData;                    // 存放单次中断接收到的一个字节
uint8_t Serial_RxBuffer[RX_BUFFER_SIZE];  // 接收数据大缓冲区
uint16_t Serial_RxLength = 0;             // 当前接收到的数据长度
uint8_t Serial_RxFlag = 0;                // 接收完成标志位（1：接收到完整一帧；0：未完成）

/**
  * @brief  串口初始化，启动第一次接收中断
  */
void Serial_Init(void)
{
    // 开启 USART1 的接收中断，接收到 1 个字节后触发回调函数
    HAL_UART_Receive_IT(&huart1, &Serial_RxData, 1);
}

/**
  * @brief  发送字符串（除了 printf 外的另一种发送方式）
  */
void Serial_SendString(char *str)
{
    while (*str != '\0')
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)str, 1, HAL_MAX_DELAY);
        str++;
    }
}

/**
  * @brief  清除接收缓冲区和标志位
  */
void Serial_ClearRxBuffer(void)
{
    Serial_RxLength = 0;
    Serial_RxFlag = 0;
    // 如果需要清空内容可以使用 memset，但通常清零索引即可
}



/* ==================================================================== */
/* printf 重定向底层实现逻辑                        */
/* ==================================================================== */
// 兼容 GCC (STM32CubeIDE) 和 Keil MDK 两种编译器
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    // 采用轮询方式发送一个字节
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/* ==================================================================== */
/* HAL库 串口接收完成回调函数                       */
/* ==================================================================== */
// 当 HAL_UART_Receive_IT 接收到指定数量（这里是1个字节）后，会自动调用此函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // 1. 存入缓冲区前，预留一个位置给 '\0'
        if (Serial_RxLength < RX_BUFFER_SIZE - 1)
        {
            Serial_RxBuffer[Serial_RxLength++] = Serial_RxData;
        }

        // 2. 兼容各种上位机的回车换行习惯 ('\n' 或 '\r')
        if (Serial_RxData == '\n' || Serial_RxData == '\r')
        {
            Serial_RxBuffer[Serial_RxLength] = '\0'; // 【关键】手动补上字符串结束符！
            Serial_RxFlag = 1; 
        }

        // 3. 再次开启接收中断
        HAL_UART_Receive_IT(&huart1, &Serial_RxData, 1);
    }
}
