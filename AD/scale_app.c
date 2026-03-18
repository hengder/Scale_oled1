#include "scale_app.h"
#include "ad_values.h"

Scale_Param_t ScaleParam;

/**
 * @brief 应用层参数初始化
 */
void Scale_App_Init(void)
{
    // 这里未来应当加入从 STM32 内部 Flash / EEPROM 读取标定参数的代码
    // 目前先填入防死机的默认值
    ScaleParam.Zero_ADC_Value  = 8388608; 
    ScaleParam.Full_ADC_Value  = 8488608; 
    ScaleParam.Full_Weight_g   = 50000.0; // 默认 50kg
    ScaleParam.Tare_ADC_Offset = 0;       // 开机无皮重
}

/**
 * @brief 零点标定 (深度平均版)
 * @note  连续采集几十次取纯数学平均，耗时约 2~3 秒，获得最纯净的物理零点
 */
void Scale_Calibrate_Zero(void)
{
    long long sum = 0;
    
    // 1. 先空读 5 次，彻底清空 ADC 内部数字滤波器里的旧数据
    for(int i = 0; i < 5; i++) {
        AD7195_ContinuousReadAvg(1);
    }
    
    // 2. 连续采集 30 次 (每次底层平均 2 下)，耗时约 2.4 秒
    for(int i = 0; i < 30; i++) {
        sum += AD7195_ContinuousReadAvg(2);
    }
    
    // 3. 计算出极其精准的平均值作为绝对零点
    ScaleParam.Zero_ADC_Value = (long)(sum / 30);
    ScaleParam.Tare_ADC_Offset = 0; 
    
    // 标定完成后，重置日常使用的自适应滤波器
    AD_Filter_Reset(); 
}

/**
 * @brief 量程标定 (深度平均版)
 */
void Scale_Calibrate_Full(double standard_weight_g)
{
    long long sum = 0;
    
    // 放上重物后，给物理传感器 1 秒钟的初始蠕变稳定时间
    HAL_Delay(1000);
    
    // 1. 先空读 5 次排空旧数据
    for(int i = 0; i < 5; i++) {
        AD7195_ContinuousReadAvg(1);
    }
    
    // 2. 深度平均采集 30 次
    for(int i = 0; i < 30; i++) {
        sum += AD7195_ContinuousReadAvg(2);
    }
    
    // 3. 记录满量程参数
    ScaleParam.Full_ADC_Value = (long)(sum / 30);
    ScaleParam.Full_Weight_g  = standard_weight_g;
    
    AD_Filter_Reset();
}

/**
 * @brief 去皮 (将当前秤上的物品视为容器，净重归零)
 */
void Scale_Tare(void)
{
    AD_Filter_Reset();
    long current_adc = AD_Get_FilteredValue();
    // 皮重 = 当前ADC - 物理零点ADC
    ScaleParam.Tare_ADC_Offset = current_adc - ScaleParam.Zero_ADC_Value;
}

/**
 * @brief 清除皮重
 */
void Scale_Clear_Tare(void)
{
    ScaleParam.Tare_ADC_Offset = 0;
}

/**
 * @brief 日常清零 (当空秤时有轻微的底噪或灰尘导致不为0时调用)
 * @note 与零点标定不同，清零通常只允许在物理零点附近极小范围内操作
 */
void Scale_Zeroing(void)
{
    AD_Filter_Reset();
    long current_adc = AD_Get_FilteredValue();
    
    // 用当前值覆盖物理零点 (通常商业秤会限制清零范围不超过满量程的 ±4%)
    ScaleParam.Zero_ADC_Value = current_adc;
    ScaleParam.Tare_ADC_Offset = 0; 
}

/**
 * @brief 内部静态函数：ADC值转重量的核心算法
 */
static double Convert_ADC_To_Weight(long target_adc)
{
    long delta_adc = target_adc - ScaleParam.Zero_ADC_Value;
    long adc_span  = ScaleParam.Full_ADC_Value - ScaleParam.Zero_ADC_Value;
    
    if(adc_span == 0) return 0.0; 
    
    double weight = (double)delta_adc * ScaleParam.Full_Weight_g / (double)adc_span;
    
    // 零点死区 (假设设置 1g 为分度值，在此消除 ±2g 的底噪)
    if(weight < 2.0 && weight > -2.0) {
        weight = 0.0;
    }
    return weight;
}

/**
 * @brief 获取毛重 (总重量，不受去皮影响)
 */
double Scale_Get_Gross_Weight(void)
{
    long current_adc = AD_Get_FilteredValue();
    return Convert_ADC_To_Weight(current_adc);
}

/**
 * @brief 获取净重 (扣除皮重后的实际物品重量，最常用的接口)
 */
double Scale_Get_Net_Weight(void)
{
    long current_adc = AD_Get_FilteredValue();
    // 减去皮重偏移量
    return Convert_ADC_To_Weight(current_adc - ScaleParam.Tare_ADC_Offset);
}
