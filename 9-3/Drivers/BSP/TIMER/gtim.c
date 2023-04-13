
#include "main.h"
#include "./gtim.h"
#include "stm32f1xx_hal_tim.h"

// 9-3 通用定时器输入捕获实验
// 通用定时器5通道1来捕获按键高电平脉宽时间，通过串口打印出来

TIM_HandleTypeDef htimx;

/**
 * @brief  初始化
 */
void gtim_timx_pwm_chy_init(uint16_t psc, uint16_t arr)
{
  // 1. 初始化结构体
  htimx.Instance = TIM5;                       // 定时器实例
  htimx.Init.Prescaler = psc;                  // 预分频
  htimx.Init.Period = arr;                     // 自动重装载值
  htimx.Init.CounterMode = TIM_COUNTERMODE_UP; // 递增计数模式
  HAL_TIM_IC_Init(&htimx);

  // 3.配置输入通道映射、捕获边沿等
  TIM_IC_InitTypeDef sConfigIC = {0};
  sConfigIC.ICPolarity = TIM_ICPOLARITY_RISING;     // 上升沿捕获
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI; // 选择输入通道1
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;           // 不分频
  sConfigIC.ICFilter = 0x00;                        // 不滤波
  HAL_TIM_IC_ConfigChannel(&htimx, &sConfigIC, TIM_CHANNEL_1);

  // 5.使能定时器更新中断
  __HAL_TIM_ENABLE_IT(&htimx, TIM_IT_UPDATE);
  // 6.使能捕获、捕获中断及计数器
  HAL_TIM_IC_Start_IT(&htimx, TIM_CHANNEL_1);
}

// 2.定时器输入捕获 MSP 初始化
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htimx)
{
  // 2. 定时器输入捕获 MSP 初始化
  if (htimx->Instance == TIM5)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // 2.1 GPIO 初始化
    // 为什么是 GPIOA ？因为要求使用 通用定时器5通道1， TIM5_CH1 是 PA0 的 复用功能，所以要初始化 PA0
    // 其实正常应该是：我们要确认那个按键，比如 KEY_UP 键，然后查看它的引脚，比如 PA0，然后再初始化 PA0，然后根据 PA0 来找到 TIM5_CH1
    // 然后初始化 TIM5_CH1

    // 2.1.1  使能 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM5_CLK_ENABLE();
    // 2.1.2  初始化 PA0
    GPIO_InitStruct.Pin = GPIO_PIN_0;             // PA0
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;         // 下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);       // 初始化

    // 4. 设置优先级，使能中断
    // 4.1 设置中断优先级
    HAL_NVIC_SetPriority(TIM5_IRQn, 1, 3);
    // 4.2 使能中断
    HAL_NVIC_EnableIRQ(TIM5_IRQn);
  }
}

// 7.编写中断服务函数
void TIM5_IRQHandler()
{
  HAL_TIM_IRQHandler(&htimx);
}

// 8.编写更新中断和捕获回调函数

/* 输入捕获状态(g_timxchy_cap_sta)
 * [7]  :0,没有成功的捕获;1,成功捕获到一次.
 * [6]  :0,还没捕获到高电平;1,已经捕获到高电平了.
 * [5:0]:捕获高电平后溢出的次数,最多溢出63次,所以最长捕获值 = 63*65536 + 65535 = 4194303
 *       注意:为了通用,我们默认ARR和CCRy都是16位寄存器,对于32位的定时器(如:TIM5),也只按16位使用
 *       按1us的计数频率,最长溢出时间为:4194303 us, 约4.19秒
 *
 *      (说明一下：正常32位定时器来说,1us计数器加1,溢出时间:4294秒)
 */
uint8_t g_timxchy_cap_sta = 0;  /* 输入捕获状态 */
uint16_t g_timxchy_cap_val = 0; /* 输入捕获值 */

/**
 * @brief       定时器输入捕获中断处理回调函数
 * @param       htimx:定时器句柄指针
 * @note        该函数在HAL_TIM_IRQHandler中会被调用
 * @retval      无
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM5)
  {
    // my_printf_pro("HAL_TIM_IC_CaptureCallback:\r\n");
    // 判断是否捕获成功, 0X80 即 1000 0000，即第7位为1，表示成功捕获到一次
    // & 与运算，只有两个位都为1，结果才为1，否则为0
    if ((g_timxchy_cap_sta & 0X80) == 0)
    { // 还未成功捕获
      // 这里逻辑是：
      // 1.一开始，g_timxchy_cap_sta = 0，即第7位为0，表示还未成功捕获
      // 2.当捕获到第一个上升沿时，g_timxchy_cap_sta = 0X40，即第6位为1，表示捕获到了上升沿
      // 3.当捕获到第一个上升沿后，我们需要捕获到一个下降沿，所以要设置捕获下降沿，先清除上升沿，然后设置下降沿

      if (g_timxchy_cap_sta & 0X40)
      {
        g_timxchy_cap_sta |= 0X80;                                             // 标记成功捕获到一次高电平脉宽
        g_timxchy_cap_val = __HAL_TIM_GET_COMPARE(&htimx, TIM_CHANNEL_1);      // 获取捕获值
        TIM_RESET_CAPTUREPOLARITY(&htimx, TIM_CHANNEL_1);                      // 重新设置捕获极性
        TIM_SET_CAPTUREPOLARITY(&htimx, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING); // 设置捕获上升沿
      }
      else // 捕获到一个上升沿,第一次上升沿
      {
        // 逻辑，1.清空计数器，2.设置捕获成功标记，3.设置捕获下降沿
        g_timxchy_cap_sta = 0;
        g_timxchy_cap_val = 0;                                                  // 清空
        g_timxchy_cap_sta |= 0X40;                                              // 标记捕获到了上升沿， 0X40 即 0100 0000，即第6位为1，表示捕获到了上升沿
        __HAL_TIM_DISABLE(&htimx);                                              // 关闭定时器
        __HAL_TIM_SET_COUNTER(&htimx, 0);                                       // 计数器清零
        TIM_RESET_CAPTUREPOLARITY(&htimx, TIM_CHANNEL_1);                       // 重新设置捕获极性
        TIM_SET_CAPTUREPOLARITY(&htimx, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING); // 下降沿捕获
        __HAL_TIM_ENABLE(&htimx);                                               // 使能定时器
      }
    }
  }
}

/**
 * @brief       定时器更新中断回调函数
 * @param        htim:定时器句柄指针
 * @note        此函数会被定时器中断函数共同调用的
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM5)
  {
    // my_printf_pro("HAL_TIM_PeriodElapsedCallback:\r\n");
    if ((g_timxchy_cap_sta & 0X80) == 0) // 还未成功捕获
    {
      if (g_timxchy_cap_sta & 0X40) // 已经捕获到了高电平，上升沿
      {
        if ((g_timxchy_cap_sta & 0X3F) == 0X3F) // 溢出次数
        {
          // 这里已经溢出了63次，即溢出时间为:63*65536 + 65535 = 4194303 us, 约4.19秒
          TIM_RESET_CAPTUREPOLARITY(&htimx, TIM_CHANNEL_1);                      // 重新设置捕获极性
          TIM_SET_CAPTUREPOLARITY(&htimx, TIM_CHANNEL_1, TIM_ICPOLARITY_RISING); // 设置捕获上升沿
          g_timxchy_cap_sta |= 0X80;                                             // 标记成功捕获到一次高电平脉宽
          g_timxchy_cap_val = 0XFFFF;                                            // 计数器的值
        }
        else
        {
          g_timxchy_cap_sta++; // 溢出次数增加
        }
      }
    }
  }
}

