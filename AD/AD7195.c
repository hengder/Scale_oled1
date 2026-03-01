#include "ad7195.h"
#include "ad_spi.h"
#include <stdio.h>

// 读取 MISO 引脚的电平状态来判断 RDY (假设 MISO 接在 PB14)
#define AD7195_RDY_STATE   HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)

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
 * @brief  等待 RDY 变低 (使用纯 SPI 读取状态寄存器，杜绝 GPIO 复用读取失败)
 */
void AD7195_WaitRdyGoLow(void)
{
    uint32_t timeOutCnt = 0x1FFFFF; 
    
    while(timeOutCnt--)
    {
        // 0x40 是读取状态寄存器，返回的 bit 7 是 RDY 标志
        unsigned long status = AD7195_GetRegisterValue(AD7195_REG_STAT, 1, 0);
        
        // 当 RDY 位为 0 时，说明转换完成，数据就绪
        if((status & AD7195_STAT_RDY) == 0)
        {
            break; 
        }
    }
    
    if (timeOutCnt == 0) {
        printf("AD7195 Wait RDY Timeout!\r\n");
    }
}

/**
 * @brief  量程与极性设置
 */
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

/**
 * @brief  开启桥路电源开关 (BPDSW)
 */
void AD7195_StartBPDSW(void)
{
    unsigned long oldRegValue = 0x0;
    unsigned long newRegValue = 0x0;

    oldRegValue = AD7195_GetRegisterValue(AD7195_REG_GPOCON, 1, 1);
    oldRegValue &= ~(AD7195_GPOCON_BPDSW); 
    newRegValue = oldRegValue | (AD7195_GPOCON_BPDSW);
    
    AD7195_SetRegisterValue(AD7195_REG_GPOCON, newRegValue, 1, 1);
}

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
 * @brief  AD7195 整体初始化 (上电自检及配置)
 * @retval 1:成功  0:失败
 */
unsigned char AD7195_Init(void)
{
    unsigned char status = 1;
    unsigned char regVal = 0;
    
    // 1. 初始化底层的 SPI 片选状态
    AD_SPI_Init();
    HAL_Delay(10); // 上电稳定延时
    
    // 2. 软件复位芯片
    AD7195_Reset();
    HAL_Delay(5);
    
    // 3. 读取 ID 寄存器以验证 SPI 通信是否正常
    regVal = AD7195_GetRegisterValue(AD7195_REG_ID, 1, 1);
    printf("AD7195 ID Read: 0x%02X\r\n", regVal);
    
    if( (regVal & AD7195_ID_MASK) != ID_AD7195)
    {
        printf("AD7195 Init Failed!\r\n");
        return 0; // 初始化失败
    }
    
//    // 4. 开启传感器桥路电源
//    AD7195_StartBPDSW();
    
    // 5. 检查外部参考电压源
    AD7195_CheckBaseValtage();
    
    // 6. 配置传感器采集参数: 双极性(0), 增益128(7), 开启交流激励(1)
    AD7195_RangeSetup(0, AD7195_CONF_GAIN_128, 1);
    
		// 7. 【新增】选择 AIN3(+) 和 AIN4(-) 作为差分输入通道 (对应位为 0x02)
    AD7195_ChannelSelect(0x02);
		
    return status;
}

/**
 * @brief  获取连续读取模式下的多次转换平均值
 * @param  sampleNumber: 采样次数
 * @retval 转换结果的平均值
 */
unsigned long AD7195_ContinuousReadAvg(unsigned char sampleNumber)
{
    unsigned long samplesAverage = 0x0;
    unsigned long command = 0x0;
    unsigned char count = 0x0;
    
    // 配置为连续转换模式，使用内部时钟源，输出速率配置为 0x060
    command = AD7195_MODE_SEL(AD7195_MODE_CONT) | 
              AD7195_MODE_CLKSRC(AD7195_CLK_INT) |
              AD7195_MODE_RATE(0x060);
              
    SPI_AD_CS_LOW(); // 整个连续读取过程中，保持 CS 拉低
    AD7195_SetRegisterValue(AD7195_REG_MODE, command, 3, 0); // CS由本函数手动控制
    
    for(count = 0; count < sampleNumber; count++)
    {
        AD7195_WaitRdyGoLow(); // 死等 RDY 变低
        samplesAverage += AD7195_GetRegisterValue(AD7195_REG_DATA, 3, 0);
    }
    SPI_AD_CS_HIGH();
    
    return (samplesAverage / sampleNumber);
}
