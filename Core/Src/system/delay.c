
#include "main.h"
#include "delay.h"

static uint16_t g_fac_us = 0; /* us延时倍乘数 */
static uint8_t is_start_up = 0; /* 是否已经初始化 */

/**
 * 初始化
 * @param sysclk 系统时钟频率，单位MHz,默认值72MHz
 */
void delay_init(uint16_t sysclk)
{

  if (is_start_up == 0)
    is_start_up = 1;
  else
    return;
  SysTick->CTRL = 0;                                        // 关闭计数器
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8); // SYSTICK使用内核时钟源8分频，所以计数器最大值 只有 2^24
  g_fac_us = sysclk / 8;                                    // 1us的计数次数
}

void delay_us(uint32_t nus)
{
  delay_init(72);
  uint32_t temp;

  SysTick->LOAD = nus * g_fac_us; // 计数值

  SysTick->VAL = 0x00; // 清空计数器

  // SysTick->CTRL = SysTick_CTRL_ENABLE_Msk; // 开始计数
  SysTick->CTRL |= 1 << 0 ; // 开始计数
  do
  {
    temp = SysTick->CTRL;
  } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达

  // SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 关闭计数器
   SysTick->CTRL &= ~(1 << 0) ; // 关闭计数器

  SysTick->VAL = 0X00; // 清空计数器
}

/**
 * @brief  延时 nms 毫秒
 */
void delay_ms(uint16_t nms)
{
  uint32_t repat = nms / 1000; // 1s

  uint32_t remain = nms % 1000; // 剩余的时间

  while (repat)
  {
    delay_us(1000 * 1000); // 1s
    repat--;
  }
  delay_us(remain * 1000); // 剩余的时间
}

void HAL_Delay(uint32_t Delay)
{
  delay_ms(Delay);
}
