/*
 * Stepper.h - Stepper library for Wiring/Arduino - Version 1.1.0
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

// ensure this library description is only included once
#ifndef Stepper_h
#define Stepper_h
#include "stdint.h"
#include "stm32wb55xx.h"

#define NON_INVERTED 	1
#define INVERTED 		-1

/* specific parameters for Project Spotlight */
#define STEPS_FOR_MOTOR 2052 //https://www.adafruit.com/product/858

// first degree of freedom
#define LED_GEAR_RATIO 			27
#define LED_STEPS_FOR_FULL_REV 	55404 	// BASE_GEAR_RATIO * STEPS_FOR_MOTOR
#define LED_MIN_ANGLE_STEP		3 		// this equals a 0.01949 degree revolution
#define LED_STEP_RANGE 			31011

// second degree of freedom
#define BASE_GEAR_RATIO 		6
#define BASE_STEPS_FOR_FULL_REV 12312 	// LED_GEAR_RATIO * STEPS_FOR_MOTOR
#define BASE_MIN_ANGLE_STEP		1 		// this equals a 0.029 degree revolution
#define BASE_STEP_RANGE			6156	// 180-degrees (can go 360 but the LED can revolve >180-degrees so no need


// library interface description
class Stepper {
  public:
    // constructors:
	Stepper(int32_t number_of_steps, GPIO_TypeDef* motor_pin_1_port, uint16_t motor_pin_1,
												GPIO_TypeDef* motor_pin_2_port, uint16_t motor_pin_2,
												volatile uint16_t* stop_sensor, uint8_t motor_direction);
    Stepper(int32_t number_of_steps, GPIO_TypeDef* motor_pin_1_port, uint16_t motor_pin_1,
    											GPIO_TypeDef* motor_pin_2_port, uint16_t motor_pin_2,
    											GPIO_TypeDef* motor_pin_3_port, uint16_t motor_pin_3,
    											GPIO_TypeDef* motor_pin_4_port, uint16_t motor_pin_4,
												volatile uint16_t* stop_sensor, uint8_t motor_direction);
    Stepper(int32_t number_of_steps, GPIO_TypeDef* motor_pin_1_port, uint16_t motor_pin_1,
    											GPIO_TypeDef* motor_pin_2_port, uint16_t motor_pin_2,
    											GPIO_TypeDef* motor_pin_3_port, uint16_t motor_pin_3,
    											GPIO_TypeDef* motor_pin_4_port, uint16_t motor_pin_4,
    											GPIO_TypeDef* motor_pin_5_port, uint16_t motor_pin_5,
												volatile uint16_t* stop_sensor, uint8_t motor_direction);

    // speed setter method:
    void setSpeed(long whatSpeed);

    // set max steps
    void setStepBound(int32_t max_step);

    // mover method:
    uint32_t step(int number_of_steps);

    // return current relative step count
    uint32_t getStepCount(void);

    // hone axis to start
    void honeAxis(void);

    int32_t version(void);

  private:
    void stepMotor(int this_step);

    int32_t direction;            // Direction of rotation
    unsigned long step_delay; // delay between steps, in ms, based on speed
    int32_t number_of_steps;      // total number of steps this motor can take
    int32_t pin_count;            // how many pins are in use.
    int32_t step_number;          // which step the motor is on
    int32_t max_step;		  // max step range
    uint32_t step_tracker;		  // var to track step position

    // motor pin numbers:
    uint16_t motor_pin_1;
    uint16_t motor_pin_2;
    uint16_t motor_pin_3;
    uint16_t motor_pin_4;
    uint16_t motor_pin_5;          // Only 5 phase motor

    // motor pin ports:
    GPIO_TypeDef* motor_pin_1_port;
    GPIO_TypeDef* motor_pin_2_port;
    GPIO_TypeDef* motor_pin_3_port;
    GPIO_TypeDef* motor_pin_4_port;
    GPIO_TypeDef* motor_pin_5_port;

    // orientation of motor
    int8_t motor_dir;

    //end stop
    volatile uint16_t* end_stop;

    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif

