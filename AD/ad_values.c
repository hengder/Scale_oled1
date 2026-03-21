/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    AD7195原始数据预处理
	* @brief   基于ad7195读取相关重量原始数据，并进行初步滤波处理
	*          
  ******************************************************************************
  * @attention
  *
  * 
  * 
  *
  * 
  * 
  * 
  *
	*  
  ******************************************************************************
  */
/* USER CODE END Header */

#include "ad_values.h"
#include "ad7195.h"  
#include <stdio.h>


/* --- 私有变量区 --- */
// 环形队列用于滑动平均滤波 (替代原参考代码的 queue.h)
static long  filter_buffer[FILTER_WINDOW_SIZE];
static uint8_t filter_index = 0;
static long long filter_sum = 0;
static uint8_t is_buffer_full = 0;



/**
 * @brief 清空滤波器历史缓存 (极其重要：防止旧数据污染标定结果)
 */
void AD_Filter_Reset(void)
{
    for(int i = 0; i < FILTER_WINDOW_SIZE; i++) {
        filter_buffer[i] = 0;
    }
    filter_sum = 0;
    filter_index = 0;
    is_buffer_full = 0;
}

/**
 * @brief 初始化滤波队列和默认标定参数
 */
void AD_Values_Init(void)
{
		AD_Filter_Reset();
}

/**
 * @brief 滑动窗口滤波器 (吸收了原工程 Queue 的思想)
 * @param new_adc_value: 最新采集到的 ADC 值
 * @retval 滤波后的平滑 ADC 值
 */
static long AD_Filter_Process(long new_adc_value)
{
    long current_avg = 0;
    long diff = 0;
    
    // 1. 获取当前的平均值基准
    if (is_buffer_full == 1) {
        current_avg = filter_sum / FILTER_WINDOW_SIZE;
    } else if (filter_index > 0) {
        current_avg = filter_sum / filter_index;
    } else {
        current_avg = new_adc_value;
    }
    
    // 2. 计算新数据与当前平均值的绝对差值
    diff = new_adc_value - current_avg;
    if (diff < 0) diff = -diff;
    
    // 3. 【核心机制】：如果差值大于阈值 (说明放上或拿走了重物)
    if (diff > FILTER_FAST_STEP_THRESHOLD)
    {
        // 瞬间用新数据覆盖整个滤波器，打破迟滞！
        for(int i = 0; i < FILTER_WINDOW_SIZE; i++) {
            filter_buffer[i] = new_adc_value;
        }
        filter_sum = new_adc_value * FILTER_WINDOW_SIZE;
        filter_index = 0;
        is_buffer_full = 1;
        
        return new_adc_value; // 立即返回新值
    }
    
    // 4. 如果差值较小 (说明是底噪或者极慢的加水)，执行正常的滑动平均以保持平滑
    filter_sum -= filter_buffer[filter_index];
    filter_buffer[filter_index] = new_adc_value;
    filter_sum += new_adc_value;
    
    filter_index++;
    if(filter_index >= FILTER_WINDOW_SIZE) {
        filter_index = 0;
        is_buffer_full = 1;
    }
    
    if(is_buffer_full == 0) return (long)(filter_sum / filter_index);
    return (long)(filter_sum / FILTER_WINDOW_SIZE);
}

/**
 * @brief 暴露给上层的接口：获取平滑后的原始 ADC 值
 */
long AD_Get_FilteredValue(void)
{
    // 只取1次，依靠滑动窗口滤波
    long raw_adc = AD7195_ContinuousReadAvg(1); 
    return AD_Filter_Process(raw_adc);
}
