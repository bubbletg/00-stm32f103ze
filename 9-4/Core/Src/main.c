#include <stdio.h>
#include "main.h"
#include "./system/sys/sys.h"
#include "../../BSP/KEY/key.h"
#include "../../BSP/LED/led.h"
#include "../../BSP/IWDG/iwdg.h"
#include "../../BSP/WWDG/wwdg.h"
#include "../../BSP/TIMER/gtim.h"

extern TIM_HandleTypeDef htimx;

int main(void)
{

  uint16_t current_cnt;
  uint16_t oldcurrent_cnt;
  uint16_t key;
  uint16_t cnt = 0;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  led_init();
  usart_init(115200);
  LED1(1);
  LED0(1);
  printf("hello world\r\n");

  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  gtim_timx_pwm_chy_init(0XFFFF, 72 - 1);

  while (1)
  {
    // key0 按下，清0
    if (key == KEY0_PRES)
    {
      __HAL_TIM_SET_COUNTER(&htimx, 0); // 计数器清零
      LED1_TOGGLE();
      delay_ms(10);
    }
    current_cnt = __HAL_TIM_GET_COUNTER(&htimx); // 获取计数器的值
    if (current_cnt != oldcurrent_cnt)
    {
      oldcurrent_cnt = current_cnt;
      printf("cnt = %d\r", current_cnt);
    }

    cnt++;
    if (cnt == 1000)
    {
      cnt = 0;
      LED0_TOGGLE();
    }
    delay_ms(10);
  }
}
