/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

#include "main.h"
#include "./system/sys/sys.h"
#include "../../BSP/LED/led.h"
// #include "../../BSP/EXTI/exti.h"
#include "../../BSP/IWDG/iwdg.h"
#include "../../BSP/WWDG/wwdg.h"
#include "../../BSP/TIME/time.h"

#include <stdio.h>


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  // HAL 初始化
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  MX_GPIO_Init();
  led_init();
  // exti_init();
  usart_init(115200);
  LED1(0);
  LED0(0);
  // test_iwdg();
  // Test_WWDG();
  // test_tim6();
  my_printf_pro("hello world\r\n");
  while (1)
  {
    LED0(1);
    LED1(1);
    delay_ms(500);
    my_printf_pro("hello world~~~\r\n");
    LED1(0);
    LED0(0);
    delay_ms(500);
  }
}

