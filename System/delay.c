#include "stm32f1xx_hal.h"

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
// 获取需要延时的系统时钟周期数
    // 例如 72MHz 下，延时 1us 需要 72 个时钟周期
    uint32_t ticks = xus * (SystemCoreClock / 1000000);
    uint32_t told = SysTick->VAL;  // 获取刚进入函数时的 SysTick 计数值
    uint32_t tnow;
    uint32_t tcnt = 0;             // 累计消耗的时钟周期数
    uint32_t reload = SysTick->LOAD; // SysTick 的重装载值
    
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            // 因为 SysTick 是向下计数的定时器
            if (tnow < told) 
            {
                tcnt += told - tnow;
            }
            else 
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            
            // 如果累计消耗的周期数达到了要求，退出循环
            if (tcnt >= ticks) 
            {
                break;
            }
        }
    }
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
// 直接调用 HAL 库自带的毫秒级延时，绝对安全
    HAL_Delay(xms);
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
    HAL_Delay(1000);
  }
} 
