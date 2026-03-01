#include "led.h"
#include "main.h"

static uint8_t ledStatus = 0;

/* GPIO初始化（cube已完成）*/

void LED_On(void)
{
		ledStatus = 1;
	  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
}

void LED_Off(void)
{
	  ledStatus = 0;
	  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
}

uint8_t LED_Status(void)
{
	  return ledStatus;
}
