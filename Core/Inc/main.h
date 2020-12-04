/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_PWM_Pin GPIO_PIN_9
#define LED_PWM_GPIO_Port GPIOB
#define MAG_BASE_Pin GPIO_PIN_0
#define MAG_BASE_GPIO_Port GPIOC
#define MAG_LIGHT_Pin GPIO_PIN_1
#define MAG_LIGHT_GPIO_Port GPIOC
#define MAIN_BOARD_TEMP_Pin GPIO_PIN_2
#define MAIN_BOARD_TEMP_GPIO_Port GPIOC
#define AIN2_2_Pin GPIO_PIN_0
#define AIN2_2_GPIO_Port GPIOA
#define AIN1_2_Pin GPIO_PIN_1
#define AIN1_2_GPIO_Port GPIOA
#define LED_TEMP_Pin GPIO_PIN_2
#define LED_TEMP_GPIO_Port GPIOA
#define INT_LED_DISC_Pin GPIO_PIN_3
#define INT_LED_DISC_GPIO_Port GPIOA
#define LED_EN_Pin GPIO_PIN_4
#define LED_EN_GPIO_Port GPIOA
#define AIN2_1_Pin GPIO_PIN_7
#define AIN2_1_GPIO_Port GPIOA
#define AIN1_1_Pin GPIO_PIN_8
#define AIN1_1_GPIO_Port GPIOA
#define BIN1_1_Pin GPIO_PIN_9
#define BIN1_1_GPIO_Port GPIOA
#define BIN1_2_Pin GPIO_PIN_10
#define BIN1_2_GPIO_Port GPIOB
#define BIN2_2_Pin GPIO_PIN_11
#define BIN2_2_GPIO_Port GPIOB
#define BLUE_LED_Pin GPIO_PIN_0
#define BLUE_LED_GPIO_Port GPIOB
#define GREEN_LED_Pin GPIO_PIN_1
#define GREEN_LED_GPIO_Port GPIOB
#define RED_LED_Pin GPIO_PIN_4
#define RED_LED_GPIO_Port GPIOE
#define BIN2_1_Pin GPIO_PIN_14
#define BIN2_1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
