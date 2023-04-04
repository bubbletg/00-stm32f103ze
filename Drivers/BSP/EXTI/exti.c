#include "./exti.h"
#include "../LED/led.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_cortex.h"

/**
 * @brief       初始化外部中断
 */
void EXTI_Init()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOE_CLK_ENABLE(); // 使能GPIOE时钟

  GPIO_InitStruct.Pin = GPIO_PIN_3;            // PE3
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // 下升沿触发中断
  GPIO_InitStruct.Pull = GPIO_PULLUP;          // 上拉
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);      // 初始化GPIOE

  // PE4
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); // 初始化GPIOE

  HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 2); // 设置中断优先级
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);         // 使能中断

  HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 2); // 设置中断优先级
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);         // 使能中断
}

void EXTI3_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4); // 调用HAL库中断处理函数, 清除中断标志位
  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);   // HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发
}

/**
 * @brief  EXTI行检测回调
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  HAL_Delay(20); // 延时消抖
  if (GPIO_Pin == GPIO_PIN_3)
  {
    // PE3 按键按下
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == 0)
    {
      LED0_TOGGLE();
    }
  }
  else if (GPIO_Pin == GPIO_PIN_4)
  {
    // PE4 按键按下
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == 0)
    {
      LED1_TOGGLE();
    }
  }
}
