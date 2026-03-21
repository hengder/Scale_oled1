/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    AD7195控制指令
	* @brief   基于ad_spi发送控制指令
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

#include "ad7195.h"
#include "ad_spi.h"
#include <stdio.h>

// 读取 MISO 引脚的电平状态来判断 RDY (目前版本 MISO 接在 PB14)
#define AD7195_RDY_STATE   HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)
// 操作SYNC引脚(目前版本 SYNC 接在 PA8)
#define AD7195_SYNC_HIGH()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)
#define AD7195_SYNC_LOW()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)
/**
 * @brief  向寄存器写数据
 * @param  registerAddress: 寄存器地址
 * @param  registerValue: 要写入的数据
 * @param  bytesNumber: 字节数
 * @param  modifyCS: 是否允许底层控制片选 (1:允许, 0:不允许)
 */
void AD7195_SetRegisterValue(unsigned char registerAddress,
                             unsigned long registerValue,
                             unsigned char bytesNumber,
                             unsigned char modifyCS)
{
    unsigned char writeCommand[5] = {0, 0, 0, 0, 0};
    unsigned char* dataPointer    = (unsigned char*)&registerValue;
    unsigned char bytesNr         = bytesNumber;
    
    // 拼装通信寄存器命令 (写模式 + 寄存器地址)
    writeCommand[0] = AD7195_COMM_WRITE | AD7195_COMM_ADDR(registerAddress);
    
    // 填充数据 (注意大小端，这里是将长整型的高低字节倒序填入发送数组)
    while(bytesNr > 0)
    {
        writeCommand[bytesNr] = *dataPointer;
        dataPointer++;
        bytesNr--;
    }
    AD_SPI_Write(AD7195_SLAVE_ID * modifyCS, writeCommand, bytesNumber + 1);
}

/**
 * @brief  读寄存器数据
 * @param  registerAddress: 寄存器地址
 * @param  bytesNumber: 字节数
 * @param  modifyCS: 是否允许底层控制片选 (1:允许, 0:不允许)
 * @retval 寄存器中的数据
 */
unsigned long AD7195_GetRegisterValue(unsigned char registerAddress,
                                      unsigned char bytesNumber,
                                      unsigned char modifyCS)
{
    unsigned char registerWord[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
    unsigned long buffer          = 0x0;
    unsigned char i               = 0;
    
    // 拼装通信寄存器命令 (读模式 + 寄存器地址)
    registerWord[0] = AD7195_COMM_READ | AD7195_COMM_ADDR(registerAddress);
    
    // 发送命令的同时读取数据
    AD_SPI_Read(AD7195_SLAVE_ID * modifyCS, registerWord, bytesNumber + 1);
    
    // 将读取到的字节拼装成完整的长整型数据
    for(i = 1; i < bytesNumber + 1; i++) 
    {
        buffer = (buffer << 8) + registerWord[i];
    }
    
    return buffer;
}

/**
 * @brief  复位 AD7195 (连续写入至少40个1)
 */
void AD7195_Reset(void)
{
    unsigned char registerWord[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    AD_SPI_Write(AD7195_SLAVE_ID, registerWord, 7);
    HAL_Delay(2); // 等待复位完成
}



/**
 * @brief  等待 RDY 变低 (转换完成)
 */
void AD7195_WaitRdyGoLow(void)
{
    // 超时计数器，防止硬件意外损坏时程序死循环
    uint32_t timeOutCnt = 0x1FFFFF; 
    
    // 不断读取 PB14(MISO) 引脚电平，直到其变为低电平 (转换完成)
    while((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) && timeOutCnt--)
    {
        // 保持空转等待即可
    }
    
    if (timeOutCnt == 0) {
        printf("AD7195 Wait RDY Timeout!\r\n");
    }
}

/**
 * @brief  量程与极性设置
 */
/***************************************************************************//**
 * @brief Selects the polarity of the conversion and the ADC input range.
 *
 * @param polarity - Polarity select bit. 
                     Example: 0 - bipolar operation is selected.
                              1 - unipolar operation is selected.
* @param range - Gain select bits. These bits are written by the user to select 
                 the ADC input range.     
 *
 * @return none.
*******************************************************************************/
void AD7195_RangeSetup(unsigned char polarity, unsigned char range, unsigned char acx)
{
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;
    
    oldRegValue = AD7195_GetRegisterValue(AD7195_REG_CONF, 3, 1);
    oldRegValue &= ~(AD7195_CONF_CHOP | AD7195_CONF_ACX | AD7195_CONF_UNIPOLAR | AD7195_CONF_GAIN(0x7));
    
    newRegValue = oldRegValue | 
                  (acx * (AD7195_CONF_ACX | AD7195_CONF_CHOP)) |
                  (polarity * AD7195_CONF_UNIPOLAR) |
                  AD7195_CONF_GAIN(range); 
                  
    AD7195_SetRegisterValue(AD7195_REG_CONF, newRegValue, 3, 1);
}

///**
// * @brief  开启桥路电源开关 (BPDSW)
// */
//void AD7195_StartBPDSW(void)
//{
//    unsigned long oldRegValue = 0x0;
//    unsigned long newRegValue = 0x0;

//    oldRegValue = AD7195_GetRegisterValue(AD7195_REG_GPOCON, 1, 1);
//    oldRegValue &= ~(AD7195_GPOCON_BPDSW); 
//    newRegValue = oldRegValue | (AD7195_GPOCON_BPDSW);
//    
//    AD7195_SetRegisterValue(AD7195_REG_GPOCON, newRegValue, 1, 1);
//}

/**
 * @brief  检测参考电压基准是否异常
 */
void AD7195_CheckBaseValtage(void)
{ 
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;
    unsigned long temRegValue = 0x0;
    
    oldRegValue = AD7195_GetRegisterValue(AD7195_REG_CONF, 3, 1);
    temRegValue = oldRegValue;
    
    temRegValue &= ~(AD7195_CONF_REFDET | AD7195_CONF_BUF | AD7195_CONF_CHOP);
    newRegValue = temRegValue | AD7195_CONF_REFDET | AD7195_CONF_BUF;
    AD7195_SetRegisterValue(AD7195_REG_CONF, newRegValue, 3, 1);
    
    temRegValue = AD7195_GetRegisterValue(AD7195_REG_STAT, 1, 1);
    if((temRegValue & AD7195_STAT_NOREF) == 0)
    {
        printf("AD7195 Ref Voltage OK.\r\n");
    }
    else
    {
        printf("ERROR: AD7195 NO Ref Voltage!\r\n");
    }
    
    // 恢复原来的配置寄存器值
    AD7195_SetRegisterValue(AD7195_REG_CONF, oldRegValue, 3, 1);
}

/**
 * @brief  选择输入通道
 * @param  channel_bits: 通道位掩码 (0x01为通道1, 0x02为通道2)
 */
void AD7195_ChannelSelect(unsigned short channel_bits)
{
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;
    
    oldRegValue = AD7195_GetRegisterValue(AD7195_REG_CONF, 3, 1);
    
    // 清除原有的通道选择位 (bit 8 ~ bit 15)
    oldRegValue &= ~(((unsigned long)0xFF) << 8); 
    
    // 写入新的通道选择位
    newRegValue = oldRegValue | (((unsigned long)channel_bits & 0xFF) << 8);
    
    AD7195_SetRegisterValue(AD7195_REG_CONF, newRegValue, 3, 1);
}


/**
 * @brief  AD7195 整体系统启动与自校准序列
 * @retval 1:成功  0:失败
 */
unsigned char AD7195_Init(void)
{
    unsigned char regVal = 0;
    
    // 1. 释放 SYNC 引脚，唤醒芯片的数字滤波器 (极度重要)
    AD7195_SYNC_HIGH();
    HAL_Delay(5);
    
    // 2. 初始化底层的 SPI CS 片选引脚
    AD_SPI_Init();
    HAL_Delay(10); 
    
    // 3. 软件复位 AD7195，清除芯片内可能的垃圾状态
    AD7195_Reset();
    HAL_Delay(5);
    
    // 4. 读取 ID 寄存器以验证 SPI 时序
    regVal = AD7195_GetRegisterValue(AD7195_REG_ID, 1, 1);
    printf("AD7195 ID Read: 0x%02X\r\n", regVal);
    if( (regVal & AD7195_ID_MASK) != ID_AD7195)
    {
        printf("ERROR: AD7195 Init Failed!\r\n");
        return 0; 
    }
    
    // 5. 写入我们精细化设计的底层配置
    AD7195_Config_Init();
    HAL_Delay(10); // 稍微延时，等待模拟前端缓冲器稳定
    
    // 6. 检查参考电压 (由于 REFDET 已经在 Config 中打开，这里直接检测即可)
    AD7195_CheckBaseValtage();
    
    // 7. 【内部零点校准】
    // 命令芯片自身短接输入端，计算出 PGA 放大器和 ADC 固有的零点漂移，并存入失调寄存器。
    printf("AD7195 Executing Internal Zero Calibration...\r\n");
    AD7195_Calibrate(AD7195_MODE_CAL_INT_ZERO, 0x02);
    printf("AD7195 Calibration Done!\r\n");
    
    return 1;
}



/**
 * @brief  AD7195 内部寄存器快照打印
 */
void AD7195_Debug_Dump(void)
{
    printf("\r\n--- AD7195 Register Dump ---\r\n");
    
    // 1. 读状态寄存器 (0x00)
    unsigned long stat = AD7195_GetRegisterValue(AD7195_REG_STAT, 1, 1);
    printf("STAT Reg (0x00): 0x%02lX\r\n", stat);
    
    // 2. 读模式寄存器 (0x01)
    unsigned long mode = AD7195_GetRegisterValue(AD7195_REG_MODE, 3, 1);
    printf("MODE Reg (0x01): 0x%06lX\r\n", mode);
    
    // 3. 读配置寄存器 (0x02)
    unsigned long conf = AD7195_GetRegisterValue(AD7195_REG_CONF, 3, 1);
    printf("CONF Reg (0x02): 0x%06lX\r\n", conf);
    
    printf("----------------------------\r\n\r\n");
}


////////////////////////////////////////////////////////////////////////////****///////////////////////////////////////////////////////////////////


///***************************************************************************//**
// * @brief Read data from temperature sensor and converts it to Celsius degrees.
// *
// * @return temperature - Celsius degrees.
//*******************************************************************************/
//unsigned long AD7195_TemperatureRead(void)
//{
//    unsigned char temperature = 0x0;
//    unsigned long dataReg = 0x0;
//    
//	  // 通道使能
//	  // 通道2
//    AD7195_ChannelSelect(AD7195_CH_TEMP_SENSOR);
//	
//	  // 读取通道数据
//    dataReg = AD7195_SingleConversion();
//	
//		Log_Hex(LOG_LVL_DEBUG, "temphex:", dataReg);
//	
//	  // 将数据转换为温度
//	  // (29 + 273) * 2815 + 8388608 = 9238738  0x8CF8D2
//	  // 0x800000 8388608
//    dataReg -= 0x800000;
//    dataReg /= 2815;   // Kelvin Temperature
//    dataReg -= 273;    //Celsius Temperature
//    temperature = (unsigned long) dataReg;
//    
//    return temperature;
//}


/* ========================================================================= *
 * 转换与控制功能接口                           *
 * ========================================================================= */

/**
 * @brief  执行一次单次转换并获取数据 (转换完成后芯片自动进入空闲/掉电模式)
 * @retval 24位ADC转换结果
 */
unsigned long AD7195_SingleConversion(void)
{
    unsigned long currentMode = 0x0;
    unsigned long regData = 0x0;

    // 1. 读取当前模式寄存器
    currentMode = AD7195_GetRegisterValue(AD7195_REG_MODE, 3, 1);
    
    // 2. 擦除旧模式，写入单次转换模式
    currentMode &= ~AD7195_MODE_SEL(0x7);
    currentMode |= AD7195_MODE_SEL(AD7195_MODE_SINGLE);
    
    // 3. 启动单次转换，必须保持 CS 拉低并死等
    SPI_AD_CS_LOW();
    AD7195_SetRegisterValue(AD7195_REG_MODE, currentMode, 3, 0); 
    
    AD7195_WaitRdyGoLow(); // 等待这一次转换结束
    
    regData = AD7195_GetRegisterValue(AD7195_REG_DATA, 3, 0);
    SPI_AD_CS_HIGH();
    
    return regData;
}

/**
 * @brief  获取连续读取模式下的多次转换平均值
 * @param  sampleNumber: 采样次数
 * @retval 转换结果的平均值
 */
unsigned long AD7195_ContinuousReadAvg(unsigned char sampleNumber)
{
    unsigned long samplesAverage = 0x0;
    unsigned long currentMode = 0x0;
    unsigned char count = 0x0;
    
    // 1. 读取当前真实的模式寄存器状态 (保留了 REJ60 和 RATE 等配置)
    currentMode = AD7195_GetRegisterValue(AD7195_REG_MODE, 3, 1);
    
    // 2. 智能判断：如果当前不是连续转换模式，才去下发命令
    if((currentMode & AD7195_MODE_SEL(0x7)) != AD7195_MODE_SEL(AD7195_MODE_CONT))
    {
        // 擦除旧模式，写入连续转换模式
        currentMode &= ~AD7195_MODE_SEL(0x7);
        currentMode |= AD7195_MODE_SEL(AD7195_MODE_CONT);
        AD7195_SetRegisterValue(AD7195_REG_MODE, currentMode, 3, 1);
    }
    
    // 3. 开始连续抓取数据
    SPI_AD_CS_LOW(); 
    for(count = 0; count < sampleNumber; count++)
    {
        AD7195_WaitRdyGoLow(); 
        samplesAverage += AD7195_GetRegisterValue(AD7195_REG_DATA, 3, 0); 
    }
    SPI_AD_CS_HIGH();
    
    return (samplesAverage / sampleNumber);
}



/**
 * @brief  设置芯片功耗模式
 * @param  pwrMode: AD7195_MODE_IDLE(空闲), AD7195_MODE_PWRDN(掉电), AD7195_MODE_CONT(连续)
 */
void AD7195_SetPower(unsigned char pwrMode)
{
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;
    
    oldRegValue = AD7195_GetRegisterValue(AD7195_REG_MODE, 3, 1);
    oldRegValue &= ~AD7195_MODE_SEL(0x7);
    newRegValue = oldRegValue | AD7195_MODE_SEL(pwrMode);
    
    AD7195_SetRegisterValue(AD7195_REG_MODE, newRegValue, 3, 1);
}

/**
 * @brief  ADC 内部/系统自动校准功能 (非常重要，用于消除零点失调和满量程误差)
 * @param  mode: 校准模式 (例如 AD7195_MODE_CAL_INT_ZERO 内部零点校准)
 * @param  channel: 要校准的通道掩码 (例如 0x02 代表 AIN3/AIN4)
 */
void AD7195_Calibrate(unsigned char mode, unsigned char channel)
{
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;
    
    // 切换到需要校准的通道
    AD7195_ChannelSelect(channel);
    
    oldRegValue = AD7195_GetRegisterValue(AD7195_REG_MODE, 3, 1);
    oldRegValue &= ~AD7195_MODE_SEL(0x7);
    newRegValue = oldRegValue | AD7195_MODE_SEL(mode);
    
    // 校准过程ADC会自己进行多次转换，时间较长，必须全程拉低CS等待
    SPI_AD_CS_LOW();
    AD7195_SetRegisterValue(AD7195_REG_MODE, newRegValue, 3, 0);
    AD7195_WaitRdyGoLow(); // 等待校准完成
    SPI_AD_CS_HIGH();
}

/**
 * @brief  【高精度电子秤定制版】AD7195 核心参数精细化配置
 * @note   直接对寄存器按位赋值，杜绝任何不确定性
 */
void AD7195_Config_Init(void)
{
    unsigned long configValue = 0x0;
    unsigned long modeValue   = 0x0;

    /* ================================================================= *
     * 1. 组装配置寄存器 (Configuration Register) - 地址 0x02
     * ================================================================= */
    // [Bit 23] CHOP(斩波)    : 开启。极大地消除ADC内部的温漂和失调误差。
    // [Bit 22] ACX(交流激励) : 关闭。硬件 REFIN- 接地，采用固定直流参考。
    // [Bit 15:8] CHAN(通道)  : 0x02。选择 AIN3(+) 和 AIN4(-) 接收传感器信号。
    // [Bit 6]  REFDET        : 开启。实时监控外部参考电压，断线自动报错。
    // [Bit 4]  BUF(缓冲器)   : 开启。极大提高输入阻抗，非常适合高阻值应变片，避免漏电流吸收信号。
    // [Bit 3]  UNIPOLAR      : 双极性。设为0，允许测量负压差，空载时数据居中于 0x800000。
    // [Bit 2:0] GAIN(增益)   : 128倍。专为微伏级别称重传感器设计的最大增益。
    configValue = AD7195_CONF_CHOP | 
                  AD7195_CONF_CHAN(0x02) | 
                  AD7195_CONF_REFDET | 
                  AD7195_CONF_GAIN(AD7195_CONF_GAIN_128);
                  
    AD7195_SetRegisterValue(AD7195_REG_CONF, configValue, 3, 1);

    /* ================================================================= *
     * 2. 组装模式寄存器 (Mode Register) - 地址 0x01
     * ================================================================= */
    // [Bit 23:21] SEL(模式)  : 空闲模式 (IDLE)。配置完先不急着转换，等待后续执行校准。
    // [Bit 19:18] CLK(时钟)  : 内部时钟。使用芯片自带的 4.92 MHz 时钟源。
    // [Bit 10] REJ60         : 开启。搭配 FS=96 时，能产生高达 130dB 的 50Hz/60Hz 空间辐射抗干扰能力。
    // [Bit 9:0] RATE(采样率) : 设为 0x060(即96)。在斩波开启下，实际出数频率约为 12.5Hz。
    //                          (注：12.5Hz 是电子秤显示的黄金频率，数据稳如泰山)
    modeValue = AD7195_MODE_SEL(AD7195_MODE_IDLE) |
                AD7195_MODE_CLKSRC(AD7195_CLK_INT) |
                AD7195_MODE_REJ60 |
                AD7195_MODE_RATE(0x060);
                
    AD7195_SetRegisterValue(AD7195_REG_MODE, modeValue, 3, 1);
}




/* ========================================================================= *
 * 寄存器便捷读取接口                           *
 * ========================================================================= */
// 0xA6
unsigned long AD7195_GetID(void)
{
	  return AD7195_GetRegisterValue(AD7195_REG_ID, 1, 1);
}

// 0x080060
unsigned long AD7195_GetMode(void)
{
		return AD7195_GetRegisterValue(AD7195_REG_MODE, 3, 1);
}

// 0x000117
unsigned long AD7195_GetConfig(void)
{
		return AD7195_GetRegisterValue(AD7195_REG_CONF, 3, 1);
}

// 0x80
unsigned long AD7195_GetStatus(void)
{
		return AD7195_GetRegisterValue(AD7195_REG_STAT, 1, 1);
}

unsigned long AD7195_GetGPOCON(void)
{
	  return AD7195_GetRegisterValue(AD7195_REG_GPOCON, 1, 1);
}

unsigned long AD7195_GetOffset(void)
{
		return AD7195_GetRegisterValue(AD7195_REG_OFFSET, 3, 1);
}

unsigned long AD7195_GetFullScale(void)
{
	  return AD7195_GetRegisterValue(AD7195_REG_FULLSCALE, 3, 1);
}
