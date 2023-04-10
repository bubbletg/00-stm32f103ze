
#include "main.h"
#include "wwdg.h"
#include "../LED/led.h"

WWDG_HandleTypeDef g_hwwdg;

void delay_ms(int32_t nms)

{

  int32_t temp;

  SysTick->LOAD = 8000 * nms;

  SysTick->VAL = 0X00; // 清空计数器

  SysTick->CTRL = 0X01; // 使能，减到零是无动作，采用外部时钟源

  do

  {

    temp = SysTick->CTRL; // 读取当前倒计数值

  }

  while ((temp & 0x01) && (!(temp & (1 << 16)))); // 等待时间到达

  SysTick->CTRL = 0x00; // 关闭计数器

  SysTick->VAL = 0X00; // 清空计数器
}

/**
 * @brief WWDG Initialization Function
 */
void WWDG_Init(uint8_t window, uint8_t counter, long unsigned int prescaler)
{

  g_hwwdg.Instance = WWDG;                 // WWDG 基地址
  g_hwwdg.Init.Prescaler = prescaler;      // 分频系数
  g_hwwdg.Init.Window = window;            // 窗口值
  g_hwwdg.Init.Counter = counter;          // 计数器值
  g_hwwdg.Init.EWIMode = WWDG_EWI_DISABLE; // 窗口模式

  HAL_WWDG_Init(&g_hwwdg);
}

/**
 * @brief WWDG MSP 回调函数
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
  if (hwwdg->Instance == WWDG)
  {
    /* Peripheral clock enable */
    __HAL_RCC_WWDG_CLK_ENABLE();
    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 2); // 设置中断优先级
    HAL_NVIC_EnableIRQ(WWDG_IRQn);         // 使能中断
  }
}

/**
 * @brief WWDG 中断处理函数
 */
void WWG_IRQHandler(void)
{
  HAL_WWDG_IRQHandler(&g_hwwdg);
}

/**
 * @brief WWDG  提前唤醒中断回调函数
 *
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
  HAL_WWDG_Refresh(&g_hwwdg); // 喂狗
  LED1(1);                    // LED1 灭
  LED0(1);                    // LED0 灭
}

void Test_WWDG()
{
  // __HAL_RCC_GET_FLAG 用于判断是否是看门狗复位，如果是看门狗复位，那么就清除复位标志，否则就打印复位信息
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) // 判断是否是看门狗复位
  {
    my_printf_pro("WWDG reset~~~~~~~2\r\n");
    __HAL_RCC_CLEAR_RESET_FLAGS(); // 清除复位标志
    // HAL_Delay(1000);
  }
  else
  {
    my_printf("WWDG not reset\r\n", 16);
  }
  HAL_Delay(2000);
  my_printf("Before-feeding-the-dog:\r\n", 24);
  WWDG_Init(0x5F, 0x7F, WWDG_PRESCALER_8);
  while (1)
  {
    delay_ms(20);
    HAL_WWDG_Refresh(&g_hwwdg); // 喂狗函数
    my_printf("After-feeding-the-dog:\r\n", 23);
  }
}
