#ifndef __GTIM_H
#define __GTIM_H

extern TIM_HandleTypeDef htimx;
void gtim_timx_pwm_chy_init(uint16_t psc, uint16_t arr);
void atim_timx_npwm_chy_set(uint8_t npwm);

#endif