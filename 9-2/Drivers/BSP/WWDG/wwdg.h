#ifndef __WWDG_H
#define __WWDG_H
#include "stm32f1xx_hal_wwdg.h"

/**
 * @brief  WWDG Init
*/
void WWDG_Init(uint8_t window, uint8_t counter, long unsigned int prescaler);


/**
 * @brief  WWDG Test
*/
void Test_WWDG(void);



#endif