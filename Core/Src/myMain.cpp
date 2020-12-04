/*
 * myMain.cpp
 *
 *  Created on: Nov 22, 2020
 *      Author: patrick
 */

#include "myMain.h"
#include "tim.h"
#include "gpio.h"
#include "stepper.h"
#include "adc.h"


#define STEPS 2052
#define STEPS_LED_RANGE 31011
volatile uint16_t timer;

struct ADC_DATA{
	uint16_t mag_base;
	uint16_t mag_led;
	uint16_t temp_main;
	uint16_t temp_led;
};

struct ADC_DATA adc_data;

//uint16_t adc_data[4];

int myMain(void){

	HAL_TIM_Base_Start_IT(&htim2);

	HAL_GPIO_WritePin(LED_EN_GPIO_Port, LED_EN_Pin, GPIO_PIN_SET);
	HAL_TIM_Base_Start(&htim17);
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);

	Stepper motor_base(STEPS, AIN1_1_GPIO_Port, AIN1_1_Pin,
			AIN2_1_GPIO_Port, AIN2_1_Pin,
			BIN1_1_GPIO_Port, BIN1_1_Pin,
			BIN2_1_GPIO_Port, BIN2_1_Pin, &adc_data.mag_base, INVERTED);
	motor_base.setSpeed(12);
	motor_base.setStepBound(BASE_STEP_RANGE);

	Stepper motor_led(STEPS, AIN1_2_GPIO_Port, AIN1_2_Pin,
				AIN2_2_GPIO_Port, AIN2_2_Pin,
				BIN1_2_GPIO_Port, BIN1_2_Pin,
				BIN2_2_GPIO_Port, BIN2_2_Pin,  &adc_data.mag_led, NON_INVERTED);
	motor_led.setSpeed(16);
	motor_led.setStepBound(LED_STEP_RANGE);

	uint32_t counter = 0;
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_data, 4);
	HAL_Delay(100);
	while (1)
		  {
			counter += 1;
//			HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_data, 4);
//			HAL_Delay(100);

//			while(1){
//				motor_led.honeAxis();
//				motor_led.step(-STEPS_LED_RANGE);
//			}
//			while(1){
//				motor_base.honeAxis();
//				motor_base.step(-3078);
//				motor_base.step(-3078);
//			}

			while(1){};

//			motor_led.step(-STEPS_LED_RANGE);
//			motor_led.step(STEPS_LED_RANGE);



//			if(adc_data.mag_led != 0){
////				while(1);
//				motor_led.step(50);
//			}else{
//				while(1);
//			}
			//
			//motor_base.step(-31011);
//			motor_base.step(-10);
//			motor_base.step(STEPS_LED_RANGE);
//			while(1){};
//			motor_base.step(-1);
//			if(adc_data.mag_led == 0){
//				temp = 0;
//			}
//			else{
//				temp=1;
//			}


//			motor_led.step(-STEPS);
//			HAL_Delay(500);
//			HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
//			HAL_Delay(500);
//			HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
//			HAL_Delay(500);
//			HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_RESET);
//			HAL_Delay(500);
//			HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
		  }
}

//uint32_t test_cnt = 0;
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  // Check which version of the timer triggered this callback and toggle LED
//  if (htim == &htim17 )
//  {
//	  test_cnt += 1;
//	  if( (test_cnt % 1000000) == 0){
//		  HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
//
//	  }
//
//  }
//}
