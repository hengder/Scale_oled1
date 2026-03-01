#ifndef __AD7195_H__
#define __AD7195_H__

#include "stm32f1xx_hal.h"

// 从机 ID (匹配底层的 CS 控制逻辑)
#define AD7195_SLAVE_ID         1

/* AD7195 芯片默认 ID */
#define ID_AD7195               0xA6
#define AD7195_ID_MASK          0xFF

/* ---------------- AD7195 寄存器地址 ---------------- */
#define AD7195_REG_COMM         0 // 通信寄存器    (8位, 只写)
#define AD7195_REG_STAT         0 // 状态寄存器    (8位, 只读)
#define AD7195_REG_MODE         1 // 模式寄存器    (24位, 读写)
#define AD7195_REG_CONF         2 // 配置寄存器    (24位, 读写)
#define AD7195_REG_DATA         3 // 数据寄存器    (24位, 只读)
#define AD7195_REG_ID           4 // ID寄存器      (8位, 只读)
#define AD7195_REG_GPOCON       5 // GPOCON寄存器  (8位, 读写)
#define AD7195_REG_OFFSET       6 // 失调寄存器    (24位, 读写)
#define AD7195_REG_FULLSCALE    7 // 满量程寄存器  (24位, 读写)

/* ---------------- 通信寄存器位掩码 ---------------- */
#define AD7195_COMM_WEN         (1 << 7)           // 写使能
#define AD7195_COMM_WRITE       (0 << 6)           // 写操作
#define AD7195_COMM_READ        (1 << 6)           // 读操作
#define AD7195_COMM_ADDR(x)     (((x) & 0x7) << 3) // 寄存器地址选择
#define AD7195_COMM_CREAD       (1 << 2)           // 连续读操作

/* ---------------- 状态寄存器位掩码 ---------------- */
#define AD7195_STAT_RDY         (1 << 7) // 就绪位
#define AD7195_STAT_ERR         (1 << 6) // 错误标志
#define AD7195_STAT_NOREF       (1 << 5) // 无外部参考电压标志

/* ---------------- 模式寄存器位掩码 ---------------- */
#define AD7195_MODE_SEL(x)      (((x) & 0x7) << 21) // 模式选择
#define AD7195_MODE_CONT        0  // 连续转换模式
#define AD7195_MODE_SINGLE      1  // 单次转换模式
#define AD7195_MODE_IDLE        2  // 空闲模式
#define AD7195_MODE_PWRDN       3  // 掉电模式
#define AD7195_MODE_CAL_INT_ZERO 4 // 内部零度校准
#define AD7195_MODE_CAL_INT_FULL 5 // 内部满量程校准
#define AD7195_MODE_CAL_SYS_ZERO 6 // 系统零度校准
#define AD7195_MODE_CAL_SYS_FULL 7 // 系统满量程校准

#define AD7195_MODE_CLKSRC(x)   (((x) & 0x3) << 18) // 时钟源选择
#define AD7195_CLK_EXT_MCLK1_2  0 // 外部晶振
#define AD7195_CLK_EXT_MCLK2    1 // 外部时钟信号
#define AD7195_CLK_INT          2 // 内部时钟(引脚不输出)
#define AD7195_CLK_INT_OUT      3 // 内部时钟(MCLK2输出)

#define AD7195_MODE_RATE(x)     ((x) & 0x3FF)       // 更新速率选择

/* ---------------- 配置寄存器位掩码 ---------------- */
#define AD7195_CONF_CHOP        (1 << 23) // 斩波使能
#define AD7195_CONF_ACX         (1 << 22) // 交流激励使能
#define AD7195_CONF_REFSEL      (1 << 20) // 参考电压选择
#define AD7195_CONF_CHAN(x)     (((x) & 0xFF) << 8) // 通道选择
#define AD7195_CONF_BURN        (1 << 7)  // 烧毁电流使能
#define AD7195_CONF_REFDET      (1 << 6)  // 参考检测使能
#define AD7195_CONF_BUF         (1 << 4)  // 缓冲器使能
#define AD7195_CONF_UNIPOLAR    (1 << 3)  // 单极性操作
#define AD7195_CONF_GAIN(x)     ((x) & 0x7) // 增益选择

// 常用增益配置
#define AD7195_CONF_GAIN_1      0
#define AD7195_CONF_GAIN_8      3
#define AD7195_CONF_GAIN_16     4
#define AD7195_CONF_GAIN_32     5
#define AD7195_CONF_GAIN_64     6
#define AD7195_CONF_GAIN_128    7

/* ---------------- GPOCON 寄存器位掩码 ------------- */
#define AD7195_GPOCON_BPDSW     (1 << 6)  // 桥路电源开关

/* 函数声明 */
void AD7195_SetRegisterValue(unsigned char registerAddress, unsigned long registerValue, unsigned char bytesNumber, unsigned char modifyCS);
unsigned long AD7195_GetRegisterValue(unsigned char registerAddress, unsigned char bytesNumber, unsigned char modifyCS);
unsigned char AD7195_Init(void);
void AD7195_ChannelSelect(unsigned short channel_bits);
void AD7195_Reset(void);
void AD7195_WaitRdyGoLow(void);
void AD7195_RangeSetup(unsigned char polarity, unsigned char range, unsigned char acx);
void AD7195_StartBPDSW(void);
void AD7195_CheckBaseValtage(void);
unsigned long AD7195_ContinuousReadAvg(unsigned char sampleNumber);

#endif /* __AD7195_H__ */
