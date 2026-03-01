#ifndef __OLED_H
#define __OLED_H 

#include "main.h"

#define USE_HORIZONTAL 0  //设置显示方向 0：正向显示；1：旋转180度显示


//-----------------测试LED端口定义---------------- 

#define LED_ON GPIO_ResetBits(GPIOA,GPIO_Pin_15)
#define LED_OFF GPIO_SetBits(GPIOA,GPIO_Pin_15)

//SCL=SCLK 
//SDA=MOSI
//RES=RES
//DC=DC
//CS=CS
#define OLED_SCL_Pin GPIO_PIN_10
#define OLED_SCL_GPIO_Port GPIOB

#define OLED_SDA_Pin GPIO_PIN_11
#define OLED_SDA_GPIO_Port GPIOB

#define OLED_RES_Pin GPIO_PIN_7
#define OLED_RES_GPIO_Port GPIOA

#define OLED_CS_Pin GPIO_PIN_0
#define OLED_CS_GPIO_Port GPIOB

#define OLED_DC_Pin GPIO_PIN_1
#define OLED_DC_GPIO_Port GPIOB

//-----------------OLED端口定义---------------- 

#define OLED_SCL_Clr()  HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL_Pin, GPIO_PIN_RESET)//SCL
#define OLED_SCL_Set()  HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL_Pin, GPIO_PIN_SET)

#define OLED_SDA_Clr()  HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, GPIO_PIN_RESET)//SDA
#define OLED_SDA_Set()  HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, GPIO_PIN_SET)

#define OLED_RES_Clr()  HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET)//RES
#define OLED_RES_Set()  HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET)

#define OLED_DC_Clr()   HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)//DC
#define OLED_DC_Set()   HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)
 		     
#define OLED_CS_Clr()   HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET)//CS
#define OLED_CS_Set()   HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)

//显示屏功能函数
void OLED_WR_REG(uint8_t reg);//写入一个指令
void OLED_WR_Byte(uint8_t dat);//写入一个数据
void Column_Address(uint8_t a,uint8_t b);//设置列地址
void Row_Address(uint8_t a,uint8_t b);//设置行地址
void OLED_Fill(uint16_t xstr,uint8_t ystr,uint16_t xend,uint8_t yend,uint8_t color);//填充函数
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t *s,uint8_t sizey,uint8_t mode);//显示汉字串
void OLED_ShowChinese16x16(uint8_t x,uint8_t y,uint8_t *s,uint8_t sizey,uint8_t mode);//显示16x16汉字
void OLED_ShowChinese24x24(uint8_t x,uint8_t y,uint8_t *s,uint8_t sizey,uint8_t mode);//显示24x24汉字
void OLED_ShowChinese32x32(uint8_t x,uint8_t y,uint8_t *s,uint8_t sizey,uint8_t mode);//显示32x32汉字
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey,uint8_t mode);//显示字符
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *dp,uint8_t sizey,uint8_t mode);//显示字符串
uint32_t oled_pow(uint8_t m,uint8_t n);//幂函数
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t sizey,uint8_t mode);//显示整数变量
void OLED_DrawBMP(uint8_t x,uint8_t y,uint16_t length,uint8_t width,const uint8_t BMP[],uint8_t mode);//显示灰度图片
void OLED_DrawSingleBMP(uint8_t x,uint8_t y,uint16_t length,uint8_t width,const uint8_t BMP[],uint8_t mode);//显示单色图片
void OLED_Init(void);

#endif







