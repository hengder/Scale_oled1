#ifndef __AD_VALUES_H__
#define __AD_VALUES_H__

#include "main.h"

// 去皮（归零）操作
void Scale_Tare(void);

// 标定操作 (传入当前放置在秤盘上的砝码重量)
void Scale_Calibrate(float known_weight_g);

// 获取当前的实际重量 (单位：克)
float Scale_Get_Weight(void);

// 获取滤波后的 24 位原始数据 (供调试观察)
uint32_t Scale_Get_Raw_Filtered(void);

#endif
