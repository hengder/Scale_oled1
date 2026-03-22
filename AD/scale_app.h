#ifndef __SCALE_APP_H__
#define __SCALE_APP_H__

#include "stm32f1xx_hal.h"

/* 电子秤核心参数结构体 (这些参数后续应当保存到 Flash 中防掉电) */
typedef struct {
    long   Zero_ADC_Value;   // 物理零点标定 ADC 值 (空秤盘)
    long   Full_ADC_Value;   // 满量程标定 ADC 值
    double Full_Weight_g;    // 标定使用的标准砝码重量 (克)
    
    long   Tare_ADC_Offset;  // 去皮偏移量 (容器重量)
} Scale_Param_t;

extern Scale_Param_t ScaleParam;

/* 初始化与标定接口 (需通过 CAN 总线调用) */
void   Scale_App_Init(void);
void   Scale_Calibrate_Zero(void);                      // 零点标定
void   Scale_Calibrate_Full(double standard_weight_g);  // 量程标定

/* 日常操作接口 (需通过 CAN 总线调用) */
void   Scale_Tare(void);           // 去皮 (扣除当前重量)
void   Scale_Clear_Tare(void);     // 清除皮重 (恢复毛重)
void   Scale_Zeroing(void);        // 日常清零 (应对微小零点漂移)

/* 数据获取接口 (主循环或 CAN 总线定时上传调用) */
double Scale_Get_Gross_Weight(void); // 获取毛重 (包含容器)
double Scale_Get_Net_Weight(void);   // 获取净重 (扣除容器)

void Scale_App_Task(void);
#endif
