/*
 * calibration.cpp
 *
 *  Created on: Dec 7, 2020
 *      Author: patrick
 */

#include "stepper.h"

#define CAL_INIT_CAL_TIMEOUT	2000
#define CAL_POS_TIMEOUT  		1000 // 1 second



uint32_t calibration_table[20] = {0};


/**
 * @brief starts calibration process for solar-powered systems within proximity of Spotlight.
 *
 * @param[in]  angle_base_min   min angle of base (represented as a fraction of 360-degrees from start)
 * @param[in]  angle_base_max   max angle of base (represented as a fraction of 360-degrees from start)
 * @param[in]  angle_led_min    min angle of led (represented as a fraction of 360-degrees from start)
 * @param[in]  angle_led_max    max angle of led (represented as a fraction of 360-degrees from start)
 *
 * @retval none.
 */
void startCal(float angle_base_min, float angle_base_max,
		float angle_led_min, float angle_led_max,
				Stepper& motor_base, Stepper& motor_led){

	/* (1) hone axis */
	motor_base.honeAxis();
	motor_led.honeAxis();

//	/* (2) multicast that calibration will begin */
//
//	// clear current calibration table
//	memset(calibration_table,0,sizeof(calibration_table));
//
//	broadcastCalStart(); //TODO: implement
//
//	/* (3) wait for nodes to ACK and populate a table with the UIDs */
//
//	// note: nodes will be replying to a Coap server (code exists in app_thread.c for populating of table)
//
//	// wait for nodes to reply with their IDs
//	osDelay(CAL_INIT_CAL_TIMEOUT);


	/* (4) Run through each position, sending an msg to each node indicating the angle is set
	 * and waiting for said nodes to respond with their solar power measurements. If node does
	 * not reply within CAL_POS_TIMEOUT, re-send msg and wait one more cycle before continuing.
	 */
		// calculate step bounds for each axis
		int32_t min_led_axis_step = (((angle_led_min) * LED_STEP_RANGE));
		int32_t max_led_axis_step = (((angle_led_max) * LED_STEP_RANGE));
		int32_t min_base_axis_step = (((angle_led_min) * BASE_STEP_RANGE));
		int32_t max_base_axis_step = (((angle_led_max) * BASE_STEP_RANGE));

		// move to desired angle
		uint8_t flip_dir = 0;
		for(int32_t base_step = min_base_axis_step; base_step <= max_base_axis_step; base_step += (BASE_CAL_STEP_MULTI * BASE_MIN_ANGLE_STEP)){
			motor_base.setAbsPos(base_step);

			if(flip_dir == 0){
				for(int32_t led_step = min_led_axis_step; led_step <= max_led_axis_step; led_step += (LED_CAL_STEP_MULTI * LED_MIN_ANGLE_STEP)){
					motor_led.setAbsPos(led_step);
				}
				flip_dir = 1;
			}
			else{
				for(int32_t led_step = max_led_axis_step; led_step >= min_led_axis_step; led_step -= (LED_CAL_STEP_MULTI * LED_MIN_ANGLE_STEP)){
					motor_led.setAbsPos(led_step);
				}
				flip_dir = 0;
			}
		}

//		// send message to nodes saying angle is reached
//		broadcastCycleStart(); //TODO: implement
//
//		// wait CAL_POS_TIMEOUT milliseconds for nodes to reply with power values
//		osDelay(CAL_POS_TIMEOUT);
//
//	/* (5) send calibration complete message to all nodes */

}

void broadcastCalStart(){
	return;
}


void broadcastCycleStart(){
	return;
}

void addNodeID(uint32_t UID, uint32_t *arr){
	int i;
	for(i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if(arr[i] == UID)
			return;
		else if(arr[i] == 0){
			arr[i] = UID;
			return;
		}
	}
}

int valueinarray(float val, float arr[])
{
    int i;
    for(i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
        if(arr[i] == val)
            return i;
        else if(arr[i] == 0)
        	return 0;
    }
    return -1;
}



