#include "scale_info.h"

// 实例化全局数据字典，并赋初始值
Scale_System_t g_Scale = {
		// 1. 核心测量数据
	  .gross_weight = 560,// 毛重
    .net_weight   = 0.000,// 净重
    .is_stable    = 0,		// 1:稳定(*), 0:波动(~)
    .is_zero_ok   = 0,		// 1:已校零, 0:未校零
	
		// 2. 仪器静态参数
    .max_capacity = 25,	// 量程 (如 500)
    .division_val = 0.1,	// 分度值 
	
		// 3. 设备与通信状态
    .device_id    = 1,		// 设备号
    .is_pc_link   = 0,		// 1:CAN连接正常, 0:断开
		// ======== 新增功能 ========  
		.unit_type    = 1, 		// 实时重量单位 (0:g, 1:kg, 2:mg, 3:lb)
    .cap_unit_type= 1,   	// 量程独立单位 (0:g, 1:kg, 2:mg, 3:lb)
    .is_alarm     = 1,			// 报警标志 (1:报警, 0:正常)
	
};

