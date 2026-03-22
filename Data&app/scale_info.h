#ifndef __SCALE_INFO_H
#define __SCALE_INFO_H

#include <stdint.h>

// 电子秤全局系统状态结构体
typedef struct {
    // 1. 核心测量数据
    float gross_weight;   // 毛重
    float net_weight;     // 净重
    uint8_t is_stable;    // 1:稳定(*), 0:波动(~)
    uint8_t is_zero_ok;   // 1:已校零, 0:未校零
    
    // 2. 仪器静态参数
    uint16_t max_capacity;// 量程 (如 500)
    float division_val;   // 分度值 (如 0.001)
    
    // 3. 设备与通信状态
    uint8_t device_id;    // 设备号
    uint8_t is_pc_link;   // 1:CAN连接正常, 0:断开
	
		// ======== 新增功能 ========
    uint8_t unit_type;      // 实时重量单位 (0:g, 1:kg, 2:mg, 3:lb)
    uint8_t cap_unit_type;  // 量程独立单位 (0:g, 1:kg, 2:mg, 3:lb)
    uint8_t is_alarm;       // 报警标志 (1:报警, 0:正常)
	
		// ======== 4. 硬件底层与标定参数 ========
    float tare_weight;     // 皮重 (去皮时记录的重量)
    int32_t raw_adc;       // AD7195当前读取的滤波后内码
    int32_t zero_adc;      // 空载时的零点内码 (b)
    float scale_factor;    // 标定比例系数 (k)
	
} Scale_System_t;

// 声明一个全局结构体变量供外部调用
extern Scale_System_t g_Scale; 

#endif
