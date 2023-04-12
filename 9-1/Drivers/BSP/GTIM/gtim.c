
#include "main.h"
#include "stm32f1xx_hal_tim.h"
#include "../LED/led.h"

TIM_HandleTypeDef htimx_currency;

/**
 * @brief 初始化
 * @param prescaler 设置预分频器
 * @param period 设置自动重装载因子
 */
void gtim_timx_init(uint16_t prescaler, uint16_t period)
{
   __HAL_RCC_TIM3_CLK_ENABLE(); // 使能TIM3时钟
  htimx_currency.Instance = TIM3;                       // 设置基本定时器6的基地址
  htimx_currency.Init.Prescaler = prescaler;            // 设置预分频器
  htimx_currency.Init.Period = period;                  // 设置自动重装载因子
  htimx_currency.Init.CounterMode = TIM_COUNTERMODE_UP; // 设置计数器模式为向上计数,递增计数模式

  HAL_TIM_Base_Init(&htimx_currency);

  HAL_NVIC_SetPriority(TIM3_IRQn, 2, 2);   // 设置中断优先级
  HAL_NVIC_EnableIRQ(TIM3_IRQn);  // 使能中断

  HAL_TIM_Base_Start_IT(&htimx_currency); // 使能更新中断并启动计数器
}


/**
 * 中断服务函数
 */
void TIM3_IRQHandler(void)
{
  if (__HAL_TIM_GET_FLAG(&htimx_currency, TIM_FLAG_UPDATE) != RESET)
  {
    LED0_TOGGLE();
    __HAL_TIM_CLEAR_IT(&htimx_currency, TIM_IT_UPDATE); // 清除中断标志位
  }
}

// --------------------下面是测试--------------------------------------------------------------------

/**
 * 使用定时器，实现500ms定时器更新中断，在中断里翻转LED0
 */
void test_gtime()
{
  // 根据 https://xuexibiji-1256533216.cos.ap-nanjing.myqcloud.com/202304111804647.png 公式,计算出来的
  // 500ms = 0.5s = 500000us = 500000 * 1000ns
  // (ARR + 1) = 0.5 * 72000000 / (72 * 1000) = 5000
  // Tout是定时器溢出时间 = ARR是自动重装载寄存器的值 * PSC是预分频器寄存器的值 / Ft是定时器的时钟源频率
  // 我们知道其中一个，求另一个
  my_printf_pro("test_gtime\r\n");
  gtim_timx_init(5000 - 1, 7200 - 1);
  while ((1))
  {
    LED1_TOGGLE();
    delay_ms(200);
  }
}
