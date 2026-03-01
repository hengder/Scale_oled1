#include "BUTTON.h"

uint8_t Button_Event_Flag = 0;        // 按键事件标志
static uint32_t Button_Last_Tick = 0; // 记录上一次触发中断的时间戳

/**
  * @brief  外部中断回调函数
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // 判断是否为复位按键引脚触发的中断
    if (GPIO_Pin == RESET_BUTTON_Pin)
    {
        uint32_t current_tick = HAL_GetTick(); // 获取当前系统运行毫秒数
        
        // 关键：计算两次下降沿中断的时间差。
        // 如果两次触发间隔大于 20ms，说明是一次真实的按键按下，而不是机械抖动
        if ((current_tick - Button_Last_Tick) > 20) 
        {
            Button_Event_Flag = 1; // 置位标志
        }
        
        // 更新时间戳
        Button_Last_Tick = current_tick; 
    }
}
