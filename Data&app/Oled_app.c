/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OLED显示应用层
	* @brief   显示界面测试及UI设计
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

#include "oled_app.h"
#include "oled.h"
#include "delay.h"
#include "scale_info.h" // 引入全局数据字典
#include <stdio.h>  // 必须包含此头文件以使用 sprintf


// 定义一个常量单位字符串数组 (定长2字符，方便擦除旧数据)
const char* UNIT_STRS[] = {"g ", "kg", "mg", "lb"};

// 电子秤主界面刷新函数
void OLED_Update_Scale_UI(void)
{
    char left_buf[32];  
    char right_buf[16]; 
    char stab_mark;     
    
    // 1. 分别获取【实时重量单位】和【量程单位】
    uint8_t unit_idx = (g_Scale.unit_type <= 3) ? g_Scale.unit_type : 0;
    const char* cur_unit = UNIT_STRS[unit_idx];
    
    uint8_t cap_unit_idx = (g_Scale.cap_unit_type <= 3) ? g_Scale.cap_unit_type : 0;
    const char* cap_unit = UNIT_STRS[cap_unit_idx];

    // 2. 核心算法：根据分度值推导需要保留的小数位数 (避免浮点数精度坑)
    uint8_t dp; // Decimal Places (小数位数)
    if (g_Scale.division_val < 0.009f)       dp = 3;  // 例如 d=0.001, d=0.005 -> 3位小数
    else if (g_Scale.division_val < 0.09f)   dp = 2;  // 例如 d=0.01, d=0.02 -> 2位小数
    else if (g_Scale.division_val < 0.9f)    dp = 1;  // 例如 d=0.1, d=0.5 -> 1位小数
    else                                     dp = 0;  // 例如 d=1, d=2 -> 0位小数

    // 判断稳定状态
    if (g_Scale.is_stable) {
        stab_mark = '*'; 
    } else {
        stab_mark = '~'; 
    }

    // ================= 1. 左侧核心数据区 =================
    
    // 第一行 (Y=0) - 动态选择毛重的小数位数
    if (dp == 3)      sprintf(left_buf, "%c B/G:%7.3f %s", stab_mark, g_Scale.gross_weight, cur_unit);
    else if (dp == 2) sprintf(left_buf, "%c B/G:%7.2f %s", stab_mark, g_Scale.gross_weight, cur_unit);
    else if (dp == 1) sprintf(left_buf, "%c B/G:%7.1f %s", stab_mark, g_Scale.gross_weight, cur_unit);
    else              sprintf(left_buf, "%c B/G:%7.0f %s", stab_mark, g_Scale.gross_weight, cur_unit);
    OLED_ShowString(0, 0, (uint8_t *)left_buf, 24, 0);

    // 第二行 (Y=24) - 动态选择净重的小数位数
    if (dp == 3)      sprintf(left_buf, "Net:%7.3f %s", g_Scale.net_weight, cur_unit);
    else if (dp == 2) sprintf(left_buf, "Net:%7.2f %s", g_Scale.net_weight, cur_unit);
    else if (dp == 1) sprintf(left_buf, "Net:%7.1f %s", g_Scale.net_weight, cur_unit);
    else              sprintf(left_buf, "Net:%7.0f %s", g_Scale.net_weight, cur_unit);
    OLED_ShowString(24, 24, (uint8_t *)left_buf, 24, 0);

    // 报警显示区 - (保留了你极其完美的12号汉字适配！)
    if (g_Scale.is_alarm) {
        OLED_ShowChinese(0, 24, (uint8_t *)"报警", 12, 1); 
    } else {
					OLED_Fill(0,24,24,36,0x00);
//        OLED_ShowString(0, 24, (uint8_t *)"    ", 12, 0); 
    }

    // 第三行 (Y=50) - 量程跟随专属的 cap_unit
    OLED_ShowChinese(0, 50, (uint8_t *)"量程", 12, 0);
    sprintf(left_buf, ":%d%s ", g_Scale.max_capacity, cap_unit); 
    OLED_ShowString(24, 48, (uint8_t *)left_buf, 16, 0);   
    
    // 分度值的显示也必须联动小数位数！(否则d=0.1时这里会显示0.10)
    OLED_ShowChinese(76, 50, (uint8_t *)"分度值", 12, 0);   
    if (dp == 3)      sprintf(left_buf, ":%.3f ", g_Scale.division_val);
    else if (dp == 2) sprintf(left_buf, ":%.2f ", g_Scale.division_val);
    else if (dp == 1) sprintf(left_buf, ":%.1f ", g_Scale.division_val);
    else              sprintf(left_buf, ":%.0f ", g_Scale.division_val);
    OLED_ShowString(112, 48, (uint8_t *)left_buf, 16, 0); 
    
    OLED_ShowChinese(160, 50, (uint8_t *)"零点", 12, 0);    
    if(g_Scale.is_zero_ok) {
        OLED_ShowChinese(184, 50, (uint8_t *)"√", 12, 0); 
    } else {
        OLED_ShowChinese(184, 50, (uint8_t *)"×", 12, 1);
    }

    // ================= 2. 视觉分割线 (X=200) =================
    OLED_ShowChar(200, 0,  '|', 16, 0);
    OLED_ShowChar(200, 16, '|', 16, 0);
    OLED_ShowChar(200, 32, '|', 16, 0);
    OLED_ShowChar(200, 48, '|', 16, 0);

    // ================= 3. 右侧状态栏 =================
    OLED_ShowChinese(216, 4, (uint8_t *)"设备号", 12, 0);    
    sprintf(right_buf, "%03d", g_Scale.device_id);
    OLED_ShowString(216, 18, (uint8_t *)right_buf, 16, 0); 
    
    OLED_ShowChinese(216, 36, (uint8_t *)"通信", 12, 0); 
    if(g_Scale.is_pc_link) {
        OLED_ShowChinese(224, 50, (uint8_t *)"√", 12, 0);   
    } else {
        OLED_ShowChinese(224, 50, (uint8_t *)"×", 12, 1); 
    }
}

void OLED_Show_Test(void)
{
	
	OLED_Fill(0,0,256,64,0x00);
	Delay_ms(1000);

	OLED_ShowChinese(32,14,"高精度电子称",32,1);
	Delay_ms(1000);
	OLED_ShowString(64,48,"power by hengder",16,0);
	Delay_ms(1000);

}