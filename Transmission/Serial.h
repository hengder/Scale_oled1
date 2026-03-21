#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f1xx_hal.h"

// 初始化串口中断接收
void Serial_Init(void);

// 解析串口接收到的指令 (在主循环中调用)
void Serial_Parse_Command(void);

#endif
