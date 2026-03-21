#include "Serial.h"
#include "usart.h"      // 引入huart1
#include "scale_info.h" // 引入全局数据字典 g_Scale
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 串口接收缓冲区
uint8_t rx_byte;
char rx_buffer[64];
uint8_t rx_index = 0;
uint8_t rx_complete = 0;

// 重定向 printf，方便给上位机回传确认信息
int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

// 开启第一次串口接收中断
void Serial_Init(void) {
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

// HAL库串口接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        // 如果接收到回车或换行符，认为一条指令结束
        if (rx_byte == '\n' || rx_byte == '\r') {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0'; // 添加字符串结束符
                rx_complete = 1;            // 标记接收完成
            }
        } else {
            // 防止缓冲区溢出
            if (rx_index < 63) {
                rx_buffer[rx_index++] = rx_byte;
            }
        }
        // 再次开启中断，等待下一个字符
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

// 模拟系统底层的两个关键计算参数
float tare_weight = 0.0f;       // 记录的皮重
float scale_factor = 1.0f;      // 标定比例系数 (模拟)

// 解析上位机指令并修改系统状态
void Serial_Parse_Command(void) {
    if (rx_complete) {
        
        // 1. 模拟重量变化 (指令: SIM:120.5)
        if (strncmp(rx_buffer, "SIM:", 4) == 0) {
            g_Scale.gross_weight = atof(&rx_buffer[4]); 
            // 每次毛重变化，净重自动减去皮重
            g_Scale.net_weight = g_Scale.gross_weight - tare_weight;  
            printf(">> OK: Weight Simulated to %.3f g\r\n", g_Scale.gross_weight);
        }
        
        // 2. 去皮操作 (指令: TARE)
        else if (strcmp(rx_buffer, "TARE") == 0) {
            tare_weight = g_Scale.gross_weight; // 记录当前毛重为皮重
            g_Scale.net_weight = 0.0f;          // 此时净重归零
            printf(">> OK: Tared. Tare Weight = %.3f g\r\n", tare_weight);
        }
        
        // 3. 清除皮重 (指令: CLEARTARE)
        else if (strcmp(rx_buffer, "CLEARTARE") == 0) {
            tare_weight = 0.0f;
            g_Scale.net_weight = g_Scale.gross_weight; // 净重恢复等于毛重
            printf(">> OK: Tare Cleared.\r\n");
        }
        
        // 4. 零点标定 (指令: CAL_ZERO)
        else if (strcmp(rx_buffer, "CAL_ZERO") == 0) {
            g_Scale.gross_weight = 0.0f;
            g_Scale.net_weight = 0.0f;
            tare_weight = 0.0f;
            g_Scale.is_zero_ok = 1; // 点亮屏幕的“零点已校准”标志
            printf(">> OK: Zero Calibrated.\r\n");
        }
        
        // 5. 量程标定 (指令: CAL_FULL:100.0) -> 使用100g砝码标定
        else if (strncmp(rx_buffer, "CAL_FULL:", 9) == 0) {
            float standard_weight = atof(&rx_buffer[9]);
            if (standard_weight > 0) {
                // 模拟算出了新的比例系数
                scale_factor = standard_weight / 100000.0f; // 假设10万ADC对应标准砝码
                g_Scale.gross_weight = standard_weight;     // 屏幕直接显示标准砝码重量
                g_Scale.net_weight = standard_weight;
                printf(">> OK: Full Scale Calibrated with %.3f g.\r\n", standard_weight);
            }
        }
        
        // 6. 其它 UI 控制测试指令
        else if (strncmp(rx_buffer, "UNIT:", 5) == 0) {
            g_Scale.unit_type = atoi(&rx_buffer[5]);
            printf(">> OK: Unit Changed to Index %d\r\n", g_Scale.unit_type);
        }
        else if (strncmp(rx_buffer, "WARN:", 5) == 0) {
            g_Scale.is_alarm = atoi(&rx_buffer[5]);
            printf(">> OK: Alarm State = %d\r\n", g_Scale.is_alarm);
        }
        else if (strncmp(rx_buffer, "STAB:", 5) == 0) {
            g_Scale.is_stable = atoi(&rx_buffer[5]);
            printf(">> OK: Stable State = %d\r\n", g_Scale.is_stable);
        }
        // 未知指令
        else {
            printf(">> ERR: Unknown Command: %s\r\n", rx_buffer);
        }

        // 清空状态，等待下一条指令
        rx_index = 0;
        rx_complete = 0;
    }
}
