#include "Serial.h"
#include "usart.h"      
#include "scale_info.h" 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ad_values.h"

// 串口接收相关变量
uint8_t rx_byte;
char rx_buffer[64];
uint8_t rx_index = 0;
uint8_t rx_complete = 0;

// 重定向 printf
int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

// 开启接收中断 (在 main 中调用一次)
void Serial_Init(void) {
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
}

// 串口中断回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (rx_byte == '\n' || rx_byte == '\r') {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0'; 
                rx_complete = 1;            
            }
        } else {
            if (rx_index < 63) {
                rx_buffer[rx_index++] = rx_byte;
            }
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

// 指令解析器 (包含完整的校准与UI控制)
void Serial_Parse_Command(void) {
    if (rx_complete) {
        
        // 1. 去皮 (Tare)
        if (strcmp(rx_buffer, "TARE") == 0) {
            g_Scale.tare_weight = g_Scale.gross_weight; 
            printf(">> OK: Tared. Tare = %.3f\r\n", g_Scale.tare_weight);
        }
        
        // 2. 清除皮重
        else if (strcmp(rx_buffer, "CLEARTARE") == 0) {
            g_Scale.tare_weight = 0.0f;
            printf(">> OK: Tare Cleared.\r\n");
        }
        
        // 3. 真实零点标定 (记录皮重底噪)
        else if (strcmp(rx_buffer, "CAL_ZERO") == 0) {
            AD_Filter_Reset();                  // 清空历史滤波池
            g_Scale.zero_adc = g_Scale.raw_adc; // 记录当前无负载的 ADC 为零点
            g_Scale.tare_weight = 0.0f;
            g_Scale.is_zero_ok = 1;             // 点亮零点 "√"
            printf(">> OK: Zero Calib! Base ADC = %d\r\n", g_Scale.zero_adc);
        }
        
        // 4. 取消/重置零点标定状态
        else if (strcmp(rx_buffer, "RESET_ZERO") == 0) {
            g_Scale.is_zero_ok = 0;             // 熄灭零点 "√"，变为 "×"
            printf(">> OK: Zero Status Reset.\r\n");
        }
        
        // 5. 真实量程标定
        else if (strncmp(rx_buffer, "CAL_FULL:", 9) == 0) {
            float standard_weight = atof(&rx_buffer[9]);
            int32_t delta_adc = g_Scale.raw_adc - g_Scale.zero_adc;
            
            if (standard_weight > 0 && delta_adc != 0) {
                g_Scale.scale_factor = standard_weight / (float)delta_adc; 
                printf(">> OK: Calibrated! Factor = %.8f\r\n", g_Scale.scale_factor);
            } else {
                printf(">> ERR: Calib Failed (Check ADC or Weight)\r\n");
            }
        }
        
        // 6. 切换【实时重量】单位 (0:g, 1:kg, 2:mg, 3:lb)
        else if (strncmp(rx_buffer, "UNIT:", 5) == 0) {
            g_Scale.unit_type = atoi(&rx_buffer[5]);
            printf(">> OK: Live Unit Changed to Index %d\r\n", g_Scale.unit_type);
        }
        
        // 7. 设置最大量程数值
        else if (strncmp(rx_buffer, "CAP:", 4) == 0) {
            g_Scale.max_capacity = atoi(&rx_buffer[4]);
            printf(">> OK: Capacity Set to %d\r\n", g_Scale.max_capacity);
        }
        
        // ================== 本次修复新增 ==================
        // 8. 独立切换【量程】专属单位 (0:g, 1:kg, 2:mg, 3:lb)
        else if (strncmp(rx_buffer, "CAPUNIT:", 8) == 0) {
            g_Scale.cap_unit_type = atoi(&rx_buffer[8]);
            printf(">> OK: Capacity Unit Changed to Index %d\r\n", g_Scale.cap_unit_type);
        }
        // ==================================================
        
        // 9. 设置分度值 (精度) 
        else if (strncmp(rx_buffer, "DIV:", 4) == 0) {
            g_Scale.division_val = atof(&rx_buffer[4]);
            printf(">> OK: Division Set to %.3f\r\n", g_Scale.division_val);
        }
        
        // 10. 报警测试
        else if (strncmp(rx_buffer, "WARN:", 5) == 0) {
            g_Scale.is_alarm = atoi(&rx_buffer[5]);
            printf(">> OK: Alarm = %d\r\n", g_Scale.is_alarm);
        }
        
        // 未知指令捕捉
        else {
            printf(">> ERR: Unknown Cmd: %s\r\n", rx_buffer);
        }
        
        // 状态清零，准备接收下一条
        rx_index = 0;
        rx_complete = 0;
    }
}