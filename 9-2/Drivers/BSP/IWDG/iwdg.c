
#include "main.h"
#include "iwdg.h"



IWDG_HandleTypeDef g_hiwdg;

/**
 *  @brief  初始化看门狗
 * @param  Prescaler: 分频系数
 * @param  Reload: 重装载值
 */
void iwdg_init(uint8_t Prescaler, uint16_t Reload)
{

  g_hiwdg.Instance = IWDG;            //
  g_hiwdg.Init.Prescaler = Prescaler; // 分频系数
  g_hiwdg.Init.Reload = Reload;       // 重装载值
  HAL_IWDG_Init(&g_hiwdg);
}

/**
 *  @brief  喂狗
 */
void iwdg_feed(void)
{
  HAL_IWDG_Refresh(&g_hiwdg);
}

/**
 * @brief  测试看门狗
 *
 */
void test_iwdg(void)
{

  // 看门狗初始化
  iwdg_init(IWDG_PRESCALER_32, 1250); // 分频系数为32，重装载值为1250，喂狗时间为1s， 1s后不喂狗会复位，1s 是通过计算得到的， 1s = 32 * 1250 / 40MHz

  my_printf("Before feeding the dog:\r\n", 24);

  while (1)
  {
    // 看门狗喂狗
    iwdg_feed();
    HAL_Delay(1000);
    my_printf("After feeding the dog:\r\n", 23);
  }
  my_printf("abcdtiangui123", 14);
}

