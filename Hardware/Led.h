#ifndef   _LED_H
#define   _LED_H

#include "main.h"
#include "stdint.h"

//-----------------测试LED端口定义---------------- 
//原理图低电平点亮
#define USER_LED_Pin GPIO_PIN_15
#define USER_LED_GPIO_Port GPIOA

//void LED_Init(void);	/* GPIO初始化（cube已完成）*/
void LED_On(void);
void LED_Off(void);

uint8_t LED_Status(void);

#endif

