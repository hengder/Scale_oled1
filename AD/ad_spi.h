#ifndef __AD_SPI_H__
#define __AD_SPI_H__

#include "main.h"
#include "spi.h"

/* 原理图引脚 (SPI2 默认引脚) */
// SPI2_NSS  -> PB12 (设计)
// SPI2_SCK  -> PB13
// SPI2_MISO -> PB14
// SPI2_MOSI -> PB15

#define  SPI_AD_CS_LOW()         HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_RESET)
#define  SPI_AD_CS_HIGH()        HAL_GPIO_WritePin(ADC_CS_GPIO_Port, ADC_CS_Pin, GPIO_PIN_SET)


// 超时时间宏定义
#define AD_SPI_TIMEOUT_MS        100
void AD_SPI_Init(void);



/***************************************************************************//**
 * @brief Writes data to SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data - Data represents the write buffer.
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char AD_SPI_Write(unsigned char slaveDeviceId,
                unsigned char* data,
                unsigned char bytesNumber);

								
/***************************************************************************//**
 * @brief Reads data from SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data - Data represents the write buffer as an input parameter and the
 *               read buffer as an output parameter.
 * @param bytesNumber - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
unsigned char AD_SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber);

#endif /* __AD_SPI_H__ */
