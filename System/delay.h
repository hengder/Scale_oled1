#ifndef __DELAY_H
#define __DELAY_H

#include "main.h"

/* 创建us级延时器*/
void Delay_us(uint32_t us);

/* 创建ms级延时器*/
void Delay_ms(uint32_t ms);

/* 创建s级延时器*/
void Delay_s(uint32_t s);

#endif
