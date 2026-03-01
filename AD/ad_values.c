#include "ad_values.h"
#include "AD7195.h"
#include <stdio.h>

/* 电子秤核心参数 */
static uint32_t Scale_Zero_Offset = 0x800000; // 默认零点 (双极性模式零点理论值在 8388608)
static float    Scale_Factor = 1000.0f;       // 默认比例系数 (需要通过标定获得真实的系数)

/* 滑动滤波窗口配置 */
#define FILTER_WINDOW_SIZE 10
static uint32_t filter_buffer[FILTER_WINDOW_SIZE];
static uint8_t  filter_index = 0;
static uint8_t  is_buffer_full = 0;

/**
  * @brief  获取滑动平均滤波后的 ADC 原始值
  */
uint32_t Scale_Get_Raw_Filtered(void)
{
    // 1. 获取单次原始数据 (这里直接调用你写好的均值函数，取 4 次作为基础采样)
    uint32_t new_raw = AD7195_ContinuousReadAvg(4); 
    
    // 2. 存入滑动窗口
    filter_buffer[filter_index++] = new_raw;
    if (filter_index >= FILTER_WINDOW_SIZE) 
    {
        filter_index = 0;
        is_buffer_full = 1;
    }
    
    // 3. 计算窗口内所有数据的平均值
    uint64_t sum = 0;
    uint8_t count = is_buffer_full ? FILTER_WINDOW_SIZE : filter_index;
    
    for (uint8_t i = 0; i < count; i++) 
    {
        sum += filter_buffer[i];
    }
    
    if (count == 0) return 0x800000; // 防呆保护
    
    return (uint32_t)(sum / count);
}

/**
  * @brief  去皮 (Tare) / 归零
  * @note   必须在空秤状态下调用此函数
  */
void Scale_Tare(void)
{
    printf("\r\n--- Taring (Please keep scale empty) ---\r\n");
    uint64_t sum = 0;
    
    // 连续读取 5 次滤波后的值，确保零点稳定
    for(int i = 0; i < 5; i++) 
    {
        sum += Scale_Get_Raw_Filtered();
    }
    
    Scale_Zero_Offset = sum / 5;
    printf("Tare Complete! New Zero Offset: %lu\r\n", Scale_Zero_Offset);
}

/**
  * @brief  两点法标定 (Calibration)
  * @param  known_weight_g: 秤盘上标准砝码的重量 (如 100.0g)
  */
void Scale_Calibrate(float known_weight_g)
{
    printf("\r\n--- Calibrating with %.2f g weight ---\r\n", known_weight_g);
    
    uint64_t sum = 0;
    for(int i = 0; i < 5; i++) 
    {
        sum += Scale_Get_Raw_Filtered();
    }
    uint32_t current_raw = sum / 5;
    
    if (current_raw == Scale_Zero_Offset) 
    {
        printf("ERROR: Calibration failed! No weight difference detected.\r\n");
        return;
    }
    
    // 计算比例系数 (Factor = ADC差值 / 实际重量)
    if (current_raw > Scale_Zero_Offset) {
        Scale_Factor = (float)(current_raw - Scale_Zero_Offset) / known_weight_g;
    } else {
        Scale_Factor = (float)(Scale_Zero_Offset - current_raw) / known_weight_g;
    }
    
    printf("Calibration Complete! New Scale Factor: %f\r\n", Scale_Factor);
}

/**
  * @brief  获取计算后的实际重量
  * @retval 实际重量 (克)
  */
float Scale_Get_Weight(void)
{
    uint32_t raw = Scale_Get_Raw_Filtered();
    float weight = 0.0f;
    
    if (raw > Scale_Zero_Offset) {
        weight = (float)(raw - Scale_Zero_Offset) / Scale_Factor;
    } else {
        weight = -(float)(Scale_Zero_Offset - raw) / Scale_Factor;
    }
    
    return weight;
}
