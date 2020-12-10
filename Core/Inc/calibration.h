/*
 * calibration.h
 *
 *  Created on: Dec 7, 2020
 *      Author: patrick
 */

#ifndef INC_CALIBRATION_H_
#define INC_CALIBRATION_H_

void startCal(float angle_base_min, float angle_base_max,
		float angle_led_min, float angle_led_max,
				Stepper& motor_base, Stepper& motor_led);

void addNodeID(uint32_t UID, uint32_t *arr);
int valueinarray(float val, float arr[]);
void broadcastCycleStart();
void broadcastCalStart();

#endif /* INC_CALIBRATION_H_ */
