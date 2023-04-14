
#include "main.h"
#include "./gtim.h"
#include "stm32f1xx_hal_tim.h"

// 9-4 通用定时器脉冲计数实验
// 将定时器2通道1输入的高电平脉冲作为定时器2的时钟，并通过串口打印脉冲数

TIM_HandleTypeDef htimx;

/**
 * @brief  初始化
 */
void gtim_timx_pwm_chy_init(uint16_t psc, uint16_t arr)
{
  // 1. 初始化结构体
  htimx.Instance = TIM2;                       // 定时器实例
  htimx.Init.Prescaler = psc;                  // 预分频
  htimx.Init.Period = arr;                     // 自动重装载值
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP; // 递增计数模式
  HAL_TIM_IC_Init(&htimx);

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1; // 外部时钟模式1
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;    // TI1FP1 为触发输入
  sSlaveConfig.TriggerPolarity = TIM_ICPOLARITY_RISING; // 上升沿触发
  sSlaveConfig.TriggerFilter = 0;               // 滤波器
  // 3. 配置定时器从模式
  HAL_TIM_SlaveConfigSynchro(&htimx, &sSlaveConfig);

  HAL_TIM_IC_Start(&htimx, TIM_CHANNEL_1);
}

// 2.定时器输入捕获 MSP 初始化
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htimx)
{
  // 2. 定时器输入捕获 MSP 初始化
  if (htimx->Instance == TIM2)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // 2.1 GPIO 初始化
    // 为什么是 GPIOA ？因为要求使用 通用定时器5通道1， TIM5_CH1 是 PA0 的 复用功能，所以要初始化 PA0
    // 其实正常应该是：我们要确认那个按键，比如 KEY_UP 键，然后查看它的引脚，比如 PA0，然后再初始化 PA0，然后根据 PA0 来找到 TIM5_CH1
    // 然后初始化 TIM5_CH1

    // 2.1.1  使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();
    // 2.1.2  初始化 PA0
    GPIO_InitStruct.Pin = GPIO_PIN_0;             // PA0
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;         // 下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);       // 初始化
  }
}
