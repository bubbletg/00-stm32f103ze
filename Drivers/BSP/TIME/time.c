
#include "main.h"
#include "stm32f1xx_hal_tim.h"
#include "../LED/led.h"

TIM_HandleTypeDef htimx;

/**
 * @brief 初始化
 * @param prescaler 设置预分频器
 * @param period 设置自动重装载因子
 */
void btim_timx_init(uint16_t prescaler, uint16_t period)
{
  htimx.Instance = TIM6;            // 设置基本定时器6的基地址
  htimx.Init.Prescaler = prescaler; // 设置预分频器
  htimx.Init.Period = period;       // 设置自动重装载因子
  HAL_TIM_Base_Init(&htimx);

  HAL_TIM_Base_Start_IT(&htimx); // 使能更新中断并启动计数器
}

/**
 * @brief 定时器基础MSP初始化函数
 *
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimx)
{
  if (htimx->Instance == TIM6)
  {
    __HAL_RCC_TIM6_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM6_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);
  }
}

/**
 * 中断服务函数
 */
void TIM6_IRQHandler()
{
  HAL_TIM_IRQHandler(&htimx);
}

/* 定时器溢出中断，中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htimx)
{
  if (htimx->Instance == TIM6)
  {
    LED0_TOGGLE();
  }
}

// --------------------下面是测试--------------------------------------------------------------------

/**
 * 使用定时器6，实现500ms定时器更新中断，在中断里翻转LED0
 */
void test_tim6()
{
  // 根据 https://xuexibiji-1256533216.cos.ap-nanjing.myqcloud.com/202304111804647.png 公式,计算出来的
  // 500ms = 0.5s = 500000us = 500000 * 1000ns
  // (ARR + 1) = 0.5 * 72000000 / (72 * 1000) = 5000
  // Tout是定时器溢出时间 = ARR是自动重装载寄存器的值 * PSC是预分频器寄存器的值 / Ft是定时器的时钟源频率
  // 我们知道其中一个，求另一个
  btim_timx_init(5000 - 1, 7200 - 1);
  while ((1))
  {
    HAL_Delay(500);
  }
}
