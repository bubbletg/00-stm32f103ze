#include "main.h"
#include "./system/sys/sys.h"
#include "../../BSP/LED/led.h"
#include "../../BSP/IWDG/iwdg.h"
#include "../../BSP/WWDG/wwdg.h"
#include "../../BSP/TIMER/gtim.h"
#include <stdio.h>


extern TIM_HandleTypeDef htimx;

int main(void)
{
  uint16_t ledrpwmval = 0;
  uint8_t dir = 1;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  led_init();
  usart_init(115200);
  LED1(0);
  LED0(0);
  my_printf_pro("hello world\r\n");
  gtim_timx_pwm_chy_init(500 - 1, 72 - 1);

  while (1)
  {
    delay_ms(10);
    if (dir) ledrpwmval++;   // 递增
    else ledrpwmval--;  // 递减

    if (ledrpwmval > 72 - 1) dir = 0;
    if (ledrpwmval == 0) dir = 1;

    __HAL_TIM_SET_COMPARE(&htimx, TIM_CHANNEL_2, ledrpwmval); // 设置比较值
  }
}
