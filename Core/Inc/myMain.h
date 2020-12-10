/*
 * myMain.h
 *
 *  Created on: Nov 22, 2020
 *      Author: patrick
 */

#ifndef INC_MYMAIN_H_
#define INC_MYMAIN_H_

#include "main.h"


int myMain(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void Reset_Device( void );
void Reset_IPCC( void );
void Init_Exti( void );
void Reset_BackupDomain( void );
void Config_HSE(void);

#endif /* INC_MYMAIN_H_ */
