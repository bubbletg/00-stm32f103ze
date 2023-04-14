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
  uint16_t oldcurrent_cnt = 0;
  uint16_t key;
  uint16_t cnt = 0;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  led_init();
  usart_init(115200);
  LED1(1);
  // LED0(1);
  printf("hello world\r\n");

  // todo :  待完成
  GPIO_InitTypeDef gpio_init_struct;
  LED0_GPIO_CLK_ENABLE();                           /* LED0时钟使能 */
  gpio_init_struct.Pin = LED0_GPIO_PIN;             /* LED0引脚 */
  gpio_init_struct.Mode = GPIO_MODE_INPUT;          // 输入
  gpio_init_struct.Pull = GPIO_PULLUP;              /* 上拉 */
  gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;    /* 高速 */
  HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct); /* 初始化LED0引脚 */

  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  gtim_timx_pwm_chy_init(5000 - 1, 7200 - 1);
  atim_timx_npwm_chy_set(10);

  while (1)
  {
    key = key_scan(0);
    // key0 按下，清0
    if (key == KEY0_PRES)
    {
      atim_timx_npwm_chy_set(5);
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
