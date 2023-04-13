
#include "main.h"
#include "./gtim.h"
#include "stm32f1xx_hal_tim.h"

// 通用定时器 PWM 输出实验

TIM_HandleTypeDef htimx;

// 通用定时器 PWM 输出初始化

void gtim_timx_pwm_chy_init(uint16_t psc, uint16_t arr)
{

  htimx.Instance = TIM3;                       // 定时器 3
  htimx.Init.Prescaler = psc;                  // 时钟预分频器
  htimx.Init.Period = arr;                     // 自动重装载值
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数模式
  HAL_TIM_PWM_Init(&htimx);


  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1; // PWM 模式 1
  sConfigOC.Pulse = arr / 2;                // 初始占空比 50%
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; // 输出极性低, 低电平有效
  HAL_TIM_PWM_ConfigChannel(&htimx, &sConfigOC, TIM_CHANNEL_2); // 通道 2

  HAL_TIM_PWM_Start(&htimx, TIM_CHANNEL_2); // 使能通道 2 PWM 输出
}



void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (htim->Instance == TIM3)
  {
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE(); // 使能定时器 3 时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();  // 使能 GPIOB 时钟
    /**TIM3 GPIO Configuration
    PB5     ------> TIM3_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5; // PB5
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 复用推挽输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // 初始化 GPIO


    // 设置复用重映射
    __HAL_RCC_AFIO_CLK_ENABLE(); // 使能 AFIO 时钟
    __HAL_AFIO_REMAP_TIM3_PARTIAL(); // TIM3_CH3 重映射到 PB5
  }
}

