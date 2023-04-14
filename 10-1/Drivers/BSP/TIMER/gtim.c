
#include "main.h"
#include "./gtim.h"
#include "stm32f1xx_hal_tim.h"

// 实验10-1 高级定时器输出指定个数PWM实验

TIM_HandleTypeDef htimx;

static uint8_t g_npwm_remain = 0;

/**
 * @brief  初始化
 */
void gtim_timx_pwm_chy_init(uint16_t psc, uint16_t arr)
{

  htimx.Instance = TIM8;                       // 定时器实例
  htimx.Init.Prescaler = psc;                  // 预分频
  htimx.Init.Period = arr;                     // 自动重装载值
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP; // 递增计数模式
  htimx.Init.RepetitionCounter = 0;            // 重复计数器
  HAL_TIM_IC_Init(&htimx);

  TIM_OC_InitTypeDef sConfig = {0};
  sConfig.OCMode = TIM_OCMODE_PWM1;         // PWM模式1
  sConfig.Pulse = arr / 2;                  // 脉冲宽度 = arr / 2
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出极性高
  HAL_TIM_PWM_ConfigChannel(&htimx, &sConfig, TIM_CHANNEL_1);

  __HAL_TIM_ENABLE_IT(&htimx, TIM_IT_UPDATE);
  HAL_TIM_PWM_Start(&htimx, TIM_CHANNEL_1);
}

// 2.定时器输入捕获 MSP 初始化
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htimx)
{

  if (htimx->Instance == TIM8)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_TIM8_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6;             // PA0
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;           // 上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);       // 初始化

    HAL_NVIC_SetPriority(TIM8_UP_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(TIM8_UP_IRQn);
  }
}

/**
 *  高级定时器TIMx NPWM 设置 PWM 个数
 *  最大值 255 个 2^8 - 1 = 255
 */
void atim_timx_npwm_chy_set(uint8_t npwm)
{
  if (npwm > 255)
    npwm = 255;
  if (npwm <= 0)
    return;
  g_npwm_remain = npwm;

  HAL_TIM_GenerateEvent(&htimx, TIM_EVENTSOURCE_UPDATE); // 产生更新事件

  __HAL_TIM_ENABLE(&htimx); // 使能定时器
}

// 定时器8中断服务函数
void TIM8_UP_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htimx);
}

/**
 * 定时器更新中断回调函数
 */
void HAL_TIM_PERIODElapsedCallback(TIM_HandleTypeDef *htimx)
{
  if (htimx->Instance == TIM8)
  {
    if (g_npwm_remain)
    {
      TIM8->RCR = g_npwm_remain - 1;                         // 重复计数器
      HAL_TIM_GenerateEvent(&htimx, TIM_EVENTSOURCE_UPDATE); // 产生软件的更新事件
      __HAL_TIM_ENABLE(htimx); // 使能定时器
      g_npwm_remain = 0;
    }
    else
    {
      TIM8->CR1 &= ~(1 << 0); // 关闭定时器
    }
  }
}
