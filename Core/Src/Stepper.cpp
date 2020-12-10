/*
 * Stepper.cpp - Stepper library for Wiring/Arduino - Version 1.1.0
 *
 * Original library        (0.1)   by Tom Igoe.
 * Two-wire modifications  (0.2)   by Sebastian Gassner
 * Combination version     (0.3)   by Tom Igoe and David Mellis
 * Bug fix for four-wire   (0.4)   by Tom Igoe, bug fix from Noah Shibley
 * High-speed stepping mod         by Eugene Kozlenko
 * Timer rollover fix              by Eugene Kozlenko
 * Five phase five wire    (1.1.0) by Ryan Orendorff
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Drives a unipolar, bipolar, or five phase stepper motor.
 *
 * When wiring multiple stepper motors to a microcontroller, you quickly run
 * out of output pins, with each motor requiring 4 connections.
 *
 * By making use of the fact that at any time two of the four motor coils are
 * the inverse of the other two, the number of control connections can be
 * reduced from 4 to 2 for the unipolar and bipolar motors.
 *
 * A slightly modified circuit around a Darlington transistor array or an
 * L293 H-bridge connects to only 2 microcontroler pins, inverts the signals
 * received, and delivers the 4 (2 plus 2 inverted ones) output signals
 * required for driving a stepper motor. Similarly the Arduino motor shields
 * 2 direction pins may be used.
 *
 * The sequence of control signals for 5 phase, 5 control wires is as follows:
 *
 * Step C0 C1 C2 C3 C4
 *    1  0  1  1  0  1
 *    2  0  1  0  0  1
 *    3  0  1  0  1  1
 *    4  0  1  0  1  0
 *    5  1  1  0  1  0
 *    6  1  0  0  1  0
 *    7  1  0  1  1  0
 *    8  1  0  1  0  0
 *    9  1  0  1  0  1
 *   10  0  0  1  0  1
 *
 * The sequence of control signals for 4 control wires is as follows:
 *
 * Step C0 C1 C2 C3
 *    1  1  0  1  0
 *    2  0  1  1  0
 *    3  0  1  0  1
 *    4  1  0  0  1
 *
 * The sequence of controls signals for 2 control wires is as follows
 * (columns C1 and C2 from above):
 *
 * Step C0 C1
 *    1  0  1
 *    2  1  1
 *    3  1  0
 *    4  0  0
 *
 * The circuits can be found at
 *
 * http://www.arduino.cc/en/Tutorial/Stepper
 */

#include "Stepper.h"
#include "gpio.h"
#include "tim.h"
#include "arm_math.h"

/*
 * two-wire constructor.
 * Sets which wires should control the motor.
 */
//Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2)
Stepper::Stepper(int32_t number_of_steps, 	GPIO_TypeDef* motor_pin_1_port, uint16_t motor_pin_1,
											GPIO_TypeDef* motor_pin_2_port, uint16_t motor_pin_2,
											volatile uint16_t* stop_sensor, uint8_t motor_direction)
{
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0; // time stamp in us of the last step taken
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;

  // Arduino ports for the motor control pin connection:
  this->motor_pin_1_port = motor_pin_1_port;
  this->motor_pin_2_port = motor_pin_2_port;

  this->motor_dir = motor_direction;

  // setup the pins on the microcontroller:
//  pinMode(this->motor_pin_1, OUTPUT);
//  pinMode(this->motor_pin_2, OUTPUT);

  // When there are only 2 pins, set the others to 0:
  this->motor_pin_3 = 0;
  this->motor_pin_4 = 0;
  this->motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 2;

  // pin that is continously updated via DMA on the magnetic end stop value (0 means the magnet is directly aligned with the sensor)
  this->end_stop = stop_sensor;
}


/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */
//Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
//                                      int motor_pin_3, int motor_pin_4)
Stepper::Stepper(int32_t number_of_steps, 	GPIO_TypeDef* motor_pin_1_port, uint16_t motor_pin_1,
											GPIO_TypeDef* motor_pin_2_port, uint16_t motor_pin_2,
											GPIO_TypeDef* motor_pin_3_port, uint16_t motor_pin_3,
											GPIO_TypeDef* motor_pin_4_port, uint16_t motor_pin_4,
											volatile uint16_t* stop_sensor, uint8_t motor_direction)
{
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0; // time stamp in us of the last step taken
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;

  // Arduino ports for the motor control pin connection:
  this->motor_pin_1_port = motor_pin_1_port;
  this->motor_pin_2_port = motor_pin_2_port;
  this->motor_pin_3_port = motor_pin_3_port;
  this->motor_pin_4_port = motor_pin_4_port;

  this->motor_dir = motor_direction;

  // setup the pins on the microcontroller:
//  pinMode(this->motor_pin_1, OUTPUT);
//  pinMode(this->motor_pin_2, OUTPUT);
//  pinMode(this->motor_pin_3, OUTPUT);
//  pinMode(this->motor_pin_4, OUTPUT);

  // When there are 4 pins, set the others to 0:
  this->motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  this->pin_count = 4;

  // pin that is continously updated via DMA on the magnetic end stop value (0 means the magnet is directly aligned with the sensor)
  this->end_stop = stop_sensor;
}

/*
 *   constructor for five phase motor with five wires
 *   Sets which wires should control the motor.
 */
//Stepper::Stepper(int number_of_steps, int motor_pin_1, int motor_pin_2,
//                                      int motor_pin_3, int motor_pin_4,
//                                      int motor_pin_5)
Stepper::Stepper(int32_t number_of_steps, 	GPIO_TypeDef* motor_pin_1_port, uint16_t motor_pin_1,
											GPIO_TypeDef* motor_pin_2_port, uint16_t motor_pin_2,
											GPIO_TypeDef* motor_pin_3_port, uint16_t motor_pin_3,
											GPIO_TypeDef* motor_pin_4_port, uint16_t motor_pin_4,
											GPIO_TypeDef* motor_pin_5_port, uint16_t motor_pin_5,
											volatile uint16_t* stop_sensor, uint8_t motor_direction)
{
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0; // time stamp in us of the last step taken
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;
  this->motor_pin_5 = motor_pin_5;

  // Arduino ports for the motor control pin connection:
  this->motor_pin_1_port = motor_pin_1_port;
  this->motor_pin_2_port = motor_pin_2_port;
  this->motor_pin_3_port = motor_pin_3_port;
  this->motor_pin_4_port = motor_pin_4_port;
  this->motor_pin_5_port = motor_pin_5_port;

  this->motor_dir = motor_direction;

  // setup the pins on the microcontroller:
//  pinMode(this->motor_pin_1, OUTPUT);
//  pinMode(this->motor_pin_2, OUTPUT);
//  pinMode(this->motor_pin_3, OUTPUT);
//  pinMode(this->motor_pin_4, OUTPUT);
//  pinMode(this->motor_pin_5, OUTPUT);

  // pin_count is used by the stepMotor() method:
  this->pin_count = 5;

  // pin that is continously updated via DMA on the magnetic end stop value (0 means the magnet is directly aligned with the sensor)
  this->end_stop = stop_sensor;
}

/*
 * Sets the speed in revs per minute
 */
void Stepper::setSpeed(long whatSpeed)
{
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}

/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */

void Stepper::setStepBound(int32_t max_step){
	this->max_step = max_step;
}

uint32_t Stepper::step(int steps_to_move)
{
	// break if requested steps exceed maximum range
	if( ((this->step_tracker + steps_to_move) > this->max_step) || (this->step_tracker - steps_to_move) < 0 ){
		return 0;
	}

	this->step_tracker += steps_to_move;

	// compensate in case motor direction is flipped
	steps_to_move = steps_to_move * this->motor_dir;

	uint32_t steps_left = abs(steps_to_move);  // how many steps to take
//	uint32_t steps_left;
  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0) { this->direction = 1; }
  if (steps_to_move < 0) { this->direction = 0; }


  // decrement the number of steps, moving one step each time:
  while (steps_left > 0)
  {
		  //    unsigned long now = micros();
	  uint32_t now = __HAL_TIM_GET_COUNTER(&htim2);
    // move only if the appropriate delay has passed:
     if (now - this->last_step_time >= this->step_delay)
    {
      // get the timeStamp of when you stepped:
      this->last_step_time = now;
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction == 1)
      {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      }
      else
      {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, ..., {3 or 10}
      if (this->pin_count == 5)
        stepMotor(this->step_number % 10);
      else
        stepMotor(this->step_number % 4);
    }
  }

  return 1;
}

/*
 * Moves the motor forward or backwards.
 */
void Stepper::stepMotor(int thisStep)
{
  if (this->pin_count == 2) {
    switch (thisStep) {
      case 0:  // 01
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
      break;
      case 1:  // 11
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
      break;
      case 2:  // 10
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
      break;
      case 3:  // 00
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
      break;
    }
  }
  if (this->pin_count == 4) {
    switch (thisStep) {
      case 0:  // 1010
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_RESET);
      break;
      case 1:  // 0110
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_RESET);
      break;
      case 2:  //0101
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_SET);
      break;
      case 3:  //1001
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_SET);
      break;
    }
  }

  if (this->pin_count == 5) {
    switch (thisStep) {
      case 0:  // 01101
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
//        digitalWrite(motor_pin_5, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_SET);
        break;
      case 1:  // 01001
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, LOW);
//        digitalWrite(motor_pin_5, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_SET);
        break;
      case 2:  // 01011
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
//        digitalWrite(motor_pin_5, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_SET);
        break;
      case 3:  // 01010
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
//        digitalWrite(motor_pin_5, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_RESET);
        break;
      case 4:  // 11010
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, HIGH);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
//        digitalWrite(motor_pin_5, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_RESET);
        break;
      case 5:  // 10010
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, LOW);
//        digitalWrite(motor_pin_4, HIGH);
//        digitalWrite(motor_pin_5, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_RESET);
        break;
      case 6:  // 10110
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, HIGH);
//        digitalWrite(motor_pin_5, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_RESET);
        break;
      case 7:  // 10100
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
//        digitalWrite(motor_pin_5, LOW);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_RESET);
        break;
      case 8:  // 10101
//        digitalWrite(motor_pin_1, HIGH);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
//        digitalWrite(motor_pin_5, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_SET);
        break;
      case 9:  // 00101
//        digitalWrite(motor_pin_1, LOW);
//        digitalWrite(motor_pin_2, LOW);
//        digitalWrite(motor_pin_3, HIGH);
//        digitalWrite(motor_pin_4, LOW);
//        digitalWrite(motor_pin_5, HIGH);
    	  HAL_GPIO_WritePin(motor_pin_1_port, motor_pin_1, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_2_port, motor_pin_2, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_3_port, motor_pin_3, GPIO_PIN_SET);
    	  HAL_GPIO_WritePin(motor_pin_4_port, motor_pin_4, GPIO_PIN_RESET);
    	  HAL_GPIO_WritePin(motor_pin_5_port, motor_pin_5, GPIO_PIN_SET);
        break;
    }
  }
}

void Stepper::honeAxis(void){

	// set step tracker to max step to bypass over step protection in step()
	this->step_tracker = this->max_step;

	while(*(this->end_stop) != 0){
		// give time for the end stop value to be updated by the ADC via DMA
		//todo: optimize this to make this quicker (i.e., increase ADC sample frequency)
		HAL_Delay(1);



		step(-1);
	}

	step_tracker = 0;
}

void Stepper::setAbsPos(int32_t step_cnt){
	this->steps_to_move = step_cnt - this->step_tracker;
	step(this->steps_to_move);
}

uint32_t Stepper::getStepCount(void){
	return this->step_tracker;
}

/*
  version() returns the version of the library:
*/
int32_t Stepper::version(void)
{
  return 5;
}
