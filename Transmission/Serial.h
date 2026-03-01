#ifndef __SERIAL_H
#define __SERIAL_H

#include "main.h"
#include "usart.h"
#include "stdio.h"

/* 串口接收缓冲区大小 */
#define RX_BUFFER_SIZE 128

/* 全局变量 */
extern uint8_t Serial_RxBuffer[RX_BUFFER_SIZE];
extern uint16_t Serial_RxLength;
extern uint8_t Serial_RxFlag;

/* 函数声明 */
void Serial_Init(void);
void Serial_SendString(char *str);
void Serial_ClearRxBuffer(void);

#endif
