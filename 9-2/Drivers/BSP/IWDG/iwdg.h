#ifndef __IWDC_H__
#define __IWDC_H__

extern IWDG_HandleTypeDef g_hiwdg;
/**
 *  @brief  初始化看门狗
 * @param  Prescaler: 分频系数
 * @param  Reload: 重装载值
 */
void iwdg_init(uint8_t Prescaler, uint16_t Reload);

/**
 *  @brief  喂狗
 */
void iwdg_feed(void);


void test_iwdg(void); // 测试看门狗

#endif