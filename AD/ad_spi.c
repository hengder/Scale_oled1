#include "ad_spi.h"
#include "spi.h"
#include "stm32f1xx_hal.h"
#include "stdio.h"

/* AD读取spi为 SPI2 句柄 */
extern SPI_HandleTypeDef hspi2;


/**
 * @brief  AD_SPI 接口初始化
 * @note   HAL库中，具体的 GPIO 和 SPI 参数初始化在 main() -> MX_SPI2_Init() 中完成。
 * 此函数主要用于确保 CS 引脚处于空闲状态（高电平）。
 */
void AD_SPI_Init(void)
{	  
		/* 停止信号 AD7195: CS引脚高电平*/
    SPI_AD_CS_HIGH();
	
		/* SPI2初始化（cube已完成）*/
		//MX_SPI2_Init();
}


/**
 * @brief  向 AD7195 写数据
 * @param  slaveDeviceId: 从机ID (1表示操作AD7195，控制CS)
 * @param  data: 数据指针
 * @param  bytesNumber: 字节数
 * @retval 已写入的字节数
 */
unsigned char AD_SPI_Write(unsigned char slaveDeviceId,
                           unsigned char* data,
                           unsigned char bytesNumber)
{
    HAL_StatusTypeDef status;

    if (slaveDeviceId == 1)
    {
        SPI_AD_CS_LOW();
    }
		
    // 直接发送整个数组，极大地节省 CPU 资源
    status = HAL_SPI_Transmit(&hspi2, data, bytesNumber, AD_SPI_TIMEOUT_MS);

    if (slaveDeviceId == 1)
    {
        SPI_AD_CS_HIGH();
    }

    if (status != HAL_OK)
    {
        // printf("SPI Write Error!\r\n");
        return 0;
    }

    return bytesNumber;
}


/**
 * @brief  从 AD7195 读数据
 * @param  slaveDeviceId: 从机ID (1表示操作AD7195，控制CS)
 * @param  data: 读写缓冲区。输入时通常为 0xFF 以防止玄学错误，输出时存放读取的数据
 * @param  bytesNumber: 字节数
 * @retval 已读取的字节数
 */
unsigned char AD_SPI_Read(unsigned char slaveDeviceId,
                          unsigned char* data,
                          unsigned char bytesNumber)
{
    HAL_StatusTypeDef status;

    // 拉低片选
    if (slaveDeviceId == 1)
    {
        SPI_AD_CS_LOW();
    }
		
    // 全双工连续读写：将 data 数组发出去，同时将收到的数据覆盖回 data 数组
    status = HAL_SPI_TransmitReceive(&hspi2, data, data, bytesNumber, AD_SPI_TIMEOUT_MS);
		
    // 释放片选
    if (slaveDeviceId == 1)
    {
        SPI_AD_CS_HIGH();
    }

    if (status != HAL_OK)
    {
        // printf("SPI Read Error!\r\n");
        return 0;
    }

    return bytesNumber;
}
