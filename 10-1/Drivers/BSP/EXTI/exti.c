
#include "../KEY/key.h"
#include "../LED/led.h"
#include "./exti.h"

/**
 * @brief       KEY0 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY0_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_INT_GPIO_PIN);         /* 调用中断处理公用函数 清除KEY0所在中断线 的中断标志位 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_INT_GPIO_PIN);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       KEY1 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY1_INT_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY1_INT_GPIO_PIN);         /* 调用中断处理公用函数 清除KEY1所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_INT_GPIO_PIN);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}


/**
 * @brief       WK_UP 外部中断服务程序
 * @param       无
 * @retval      无
 */
void WKUP_INT_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(WKUP_INT_GPIO_PIN);        /* 调用中断处理公用函数 清除KEY_UP所在中断线 的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(WKUP_INT_GPIO_PIN);        /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief  EXTI行检测回调
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  HAL_Delay(20); // 延时消抖
  switch (GPIO_Pin)
  {
  case KEY0_INT_GPIO_PIN:
    if (KEY0 == 0)
    {
      // LED0_TOGGLE();
      LED0(1);
      LED1(0);
    }
    break;
  case KEY1_INT_GPIO_PIN:
    if (KEY1 == 0)
    {
      // LED1_TOGGLE();
      LED1(0);
      LED0(0);
    }
    break;
  case WKUP_INT_GPIO_PIN:
    if (WK_UP == 1)
    {
      LED0_TOGGLE();
      LED1_TOGGLE();
    }
    break;
  default:
    break;
  }
}

/**
 * @brief       初始化外部中断
 */
void exti_init()
{
  // 使能按键引脚时钟
  KEY0_INT_GPIO_CLK_ENABLE();
  KEY1_INT_GPIO_CLK_ENABLE();
  WKUP_INT_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // 配置按键引脚为输入模式
  GPIO_InitStruct.Pin = KEY0_INT_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // 下降沿触发中断
  GPIO_InitStruct.Pull = GPIO_PULLUP;          // 上拉
  HAL_GPIO_Init(KEY0_INT_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = KEY1_INT_GPIO_PIN;
  HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = WKUP_INT_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // 上升沿触发中断
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;       // 下拉
  HAL_GPIO_Init(WKUP_INT_GPIO_PORT, &GPIO_InitStruct);

  // 配置中断优先级
  HAL_NVIC_SetPriority(KEY0_INT_IRQn, 2, 2); // 抢占优先级2, 子优先级2
  HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);         // 使能中断

  HAL_NVIC_SetPriority(KEY1_INT_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(KEY1_INT_IRQn);

  HAL_NVIC_SetPriority(WKUP_INT_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(WKUP_INT_IRQn);
}

