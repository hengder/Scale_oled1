#ifndef __AD_VALUES_H__
#define __AD_VALUES_H__

#include "stm32f1xx_hal.h"

/* 滤波窗口大小 (数值越大越平滑，但反应越慢。10~20 适合电子秤) */
#define FILTER_WINDOW_SIZE  20
// 根据你之前标定的数据，500g 大约对应 101931 个 ADC 字，即 1g ≈ 200字
// 我们将突变阈值设为 1000 个字 (大约 5g 的变化量)
#define FILTER_FAST_STEP_THRESHOLD  1000

/* 标定参数结构体 */
typedef struct {
    long  Zero_ADC_Value;   // 空载时的零点 ADC 值 (皮重)
    long  Full_ADC_Value;   // 放上标准砝码后的 ADC 值
    double Full_Weight_g;    // 标准砝码的真实重量 (克)
} Scale_Calibration_t;


void AD_Values_Init(void);
void AD_Filter_Reset(void);
long AD_Get_FilteredValue(void); // 提供给上层的唯一数据接口


#endif /* __AD_VALUES_H__ */
