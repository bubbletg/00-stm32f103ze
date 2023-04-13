#include "main.h"
#include "./system/sys/sys.h"
#include "../../BSP/LED/led.h"
#include "../../BSP/IWDG/iwdg.h"
#include "../../BSP/WWDG/wwdg.h"
#include "../../BSP/TIMER/gtim.h"
#include <stdio.h>

extern uint8_t g_timxchy_cap_sta;  /* 输入捕获状态 */
extern uint16_t g_timxchy_cap_val; /* 输入捕获值 */

int main(void)
{
  uint32_t temp = 0;
  uint8_t t = 0;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  led_init();
  usart_init(115200);
  LED1(1);
  LED0(1);
  my_printf_pro("hello world\r\n");

  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  delay_ms(500);
  gtim_timx_pwm_chy_init(0XFFFF, 72 - 1);

  while (1)
  {
    if (g_timxchy_cap_sta & 0X80) // 捕获成功
    {
      temp = g_timxchy_cap_sta & 0X3F;
      temp *= 65536;               /* 溢出时间总和 */
      temp += g_timxchy_cap_val;   /* 得到总的高电平时间 */
      my_printf_pro("HIGH:\r\n");  /* 打印总的高点平时间 */
      my_printf_pro_int(temp); /* 打印总的高点平时间 */
      my_printf_pro("us:\r\n");    /* 打印总的高点平时间 */
      g_timxchy_cap_sta = 0;       /* 开启下一次捕获*/
    }
    t++;
    if (t > 20) /* 200ms进入一次 */
    {
      t = 0;
      LED0_TOGGLE(); /* LED0闪烁 ,提示程序运行 */
    }
    delay_ms(10);
  }
}
