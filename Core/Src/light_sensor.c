/**
 ******************************************************************************
 * File Name           : light_sensor.c
  * Description        : .
  ******************************************************************************

  *
  ******************************************************************************
 */


/* includes -----------------------------------------------------------*/
#include <light_sensor.h>
#include "i2c.h"
#include "stm32wbxx_hal.h"
#include "stdint.h"
#include "stdbool.h"

/* typedef -----------------------------------------------------------*/


/* defines -----------------------------------------------------------*/


/* macros ------------------------------------------------------------*/


/* function prototypes -----------------------------------------------*/


/* variables -----------------------------------------------*/
uint8_t _deviceAddress_light;
uint8_t _int;
GPIO_TypeDef* _intPort;
I2C_HandleTypeDef* i2c_periph;

bool _tcs34725Initialised;
tcs34725Gain_t _tcs34725Gain;
tcs34725IntegrationTime_t _tcs34725IntegrationTime;


/* Functions Definition ------------------------------------------------------*/


/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
#include <math.h>
#include <stdlib.h>



/*!
 *  @brief  Implements missing powf function
 *  @param  x
 *          Base number
 *  @param  y
 *          Exponent
 *  @return x raised to the power of y
 */
float powf(const float x, const float y) {
  return (float)(pow((double)x, (double)y));
}

/*!
 *  @brief  Writes a register and an 8 bit value over I2C
 *  @param  reg
 *  @param  value
 */
void light_write8(uint8_t reg, uint32_t value) {

	uint8_t packet = value;

    HAL_I2C_Mem_Write(i2c_periph, _deviceAddress_light, TCS34725_COMMAND_BIT | reg,
    		I2C_MEMADD_SIZE_8BIT, &packet, 1, 1000);

//    HAL_I2C_Master_Transmit(i2c_periph, _deviceAddress_light, &packet, 1, 1000);
//
//    packet = value;
//    HAL_I2C_Master_Transmit(i2c_periph, _deviceAddress_light, &packet, 1, 1000);


}

/*!
 *  @brief  Reads an 8 bit value over I2C
 *  @param  reg
 *  @return value
 */
uint8_t light_read8(uint8_t reg) {

	uint8_t read_reg;

	HAL_I2C_Mem_Read(i2c_periph, _deviceAddress_light, TCS34725_COMMAND_BIT | reg,
			I2C_MEMADD_SIZE_8BIT, &read_reg, 1, 1000);

	return read_reg;

}

/*!
 *  @brief  Reads a 16 bit values over I2C
 *  @param  reg
 *  @return value
 */
uint16_t light_read16(uint8_t reg) {
  uint16_t read_reg_concat;

  uint8_t read_reg[2];

//  uint8_t packet = TCS34725_COMMAND_BIT | reg;
//  HAL_I2C_Master_Transmit(i2c_periph, _deviceAddress_light, &packet, 1, 1000);
//
//
//  HAL_I2C_Master_Receive(i2c_periph, _deviceAddress_light, &read_reg[0], 2, 1000);


	HAL_I2C_Mem_Read(i2c_periph, _deviceAddress_light, TCS34725_COMMAND_BIT | reg,
		I2C_MEMADD_SIZE_8BIT, &read_reg[0], 2, 1000);


	read_reg_concat = ( ((uint16_t) read_reg[1] ) << 8) | read_reg[0];

  return read_reg_concat;
}

/*!
 *  @brief  Enables the device
 */
void light_enable() {
	light_write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
	HAL_Delay(4);
    light_write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);

  /* Set a delay for the integration time.
    This is only necessary in the case where enabling and then
    immediately trying to read values back. This is because setting
    AEN triggers an automatic integration, so if a read RGBC is
    performed too quickly, the data is not yet valid and all 0's are
    returned */
  switch (_tcs34725IntegrationTime) {
  case TCS34725_INTEGRATIONTIME_2_4MS:
	  HAL_Delay(3);
    break;
  case TCS34725_INTEGRATIONTIME_24MS:
	  HAL_Delay(24);
    break;
  case TCS34725_INTEGRATIONTIME_50MS:
	  HAL_Delay(50);
    break;
  case TCS34725_INTEGRATIONTIME_101MS:
	  HAL_Delay(101);
    break;
  case TCS34725_INTEGRATIONTIME_154MS:
	  HAL_Delay(154);
    break;
  case TCS34725_INTEGRATIONTIME_700MS:
	  HAL_Delay(700);
    break;
  }
}

/*!
 *  @brief  Disables the device (putting it in lower power sleep mode)
 */
void light_disable() {
  /* Turn the device off to save power */
  uint8_t reg = 0;
  reg = light_read8(TCS34725_ENABLE);
  light_write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
//  tcs34725Initialised = false;
}

/*!
 *  @brief  Constructor
 *  @param  it
 *          Integration Time
 *  @param  gain
 *          Gain
 */
//Adafruit_TCS34725::Adafruit_TCS34725(tcs34725IntegrationTime_t it,
//                                     tcs34725Gain_t gain) {
//  _tcs34725Initialised = false;
//  _tcs34725IntegrationTime = it;
//  _tcs34725Gain = gain;
//}


/*!
 *  @brief  Initializes I2C and configures the sensor
 *  @param  addr
 *          i2c address
 *  @param  *theWire
 *          The Wire object
 *  @return True if initialization was successful, otherwise false.
 */
void light_begin(uint8_t deviceAddress, uint8_t intPin, GPIO_TypeDef* intPort, I2C_HandleTypeDef* periph,
		tcs34725IntegrationTime_t it, tcs34725Gain_t gain) {
	_deviceAddress_light = deviceAddress << 1;
	_int = intPin;
	_intPort = intPort;
	i2c_periph = periph;
//	_tcs34725Initialised = true;
	_tcs34725IntegrationTime = it;
	 _tcs34725Gain = gain;

	 light_init();

}

/*!
 *  @brief  Initializes I2C and configures the sensor
 *  @return True if initialization was successful, otherwise false.
 */
//bool light_begin() {
//  _i2caddr = TCS34725_ADDRESS;
//  _wire = &Wire;
//
//  return init();
//}

/*!
 *  @brief  Part of begin
 *  @return True if initialization was successful, otherwise false.
 */
void light_init() {

//	  light_disable();

  /* Make sure we're actually connected */
  uint8_t x = light_read8(TCS34725_ID);
//  if ((x != 0x44) && (x != 0x10)) {
//    return 	false;
//  }
//  _tcs34725Initialised = true;

  /* Set default integration time and gain */
  light_setIntegrationTime(_tcs34725IntegrationTime);
  light_setGain(_tcs34725Gain);
//  light_setWait(0xFF);

  //light_clearInterrupt();

  /* Note: by default, the device is in power down mode on bootup */
  light_enable();

//  return true;
}

/*!
 *  @brief  Sets the integration time for the TC34725
 *  @param  it
 *          Integration Time
 */
void light_setIntegrationTime(tcs34725IntegrationTime_t it) {
//  if (!_tcs34725Initialised)
//	  light_begin();

  /* Update the timing register */
  light_write8(TCS34725_ATIME, it);

  /* Update value placeholders */
  _tcs34725IntegrationTime = it;
}

/*!
 *  @brief  Adjusts the gain on the TCS34725
 *  @param  gain
 *          Gain (sensitivity to light)
 */
void light_setGain(tcs34725Gain_t gain) {
//  if (!_tcs34725Initialised)
//	  light_begin();

  /* Update the timing register */
  light_write8(TCS34725_CONTROL, gain);

  /* Update value placeholders */
  _tcs34725Gain = gain;
}

void light_setWait(uint8_t value) {
	//  if (!_tcs34725Initialised)
	//	  light_begin();

	  /* Update the timing register */
	  light_write8(TCS34725_WTIME, value);

	}

/*!
 *  @brief  Reads the raw red, green, blue and clear channel values
 *  @param  *r
 *          Red value
 *  @param  *g
 *          Green value
 *  @param  *b
 *          Blue value
 *  @param  *c
 *          Clear channel value
 */
void light_getRawData(uint16_t *r, uint16_t *g, uint16_t *b,
                                   uint16_t *c) {
//  if (!_tcs34725Initialised)
//	  light_begin();

  *c = light_read16(TCS34725_CDATAL);
  *r = light_read16(TCS34725_RDATAL);
  *g = light_read16(TCS34725_GDATAL);
  *b = light_read16(TCS34725_BDATAL);

  /* Set a delay for the integration time */
  switch (_tcs34725IntegrationTime) {
  case TCS34725_INTEGRATIONTIME_2_4MS:
	  HAL_Delay(3);
    break;
  case TCS34725_INTEGRATIONTIME_24MS:
	  HAL_Delay(24);
    break;
  case TCS34725_INTEGRATIONTIME_50MS:
	  HAL_Delay(50);
    break;
  case TCS34725_INTEGRATIONTIME_101MS:
	  HAL_Delay(101);
    break;
  case TCS34725_INTEGRATIONTIME_154MS:
	  HAL_Delay(154);
    break;
  case TCS34725_INTEGRATIONTIME_700MS:
	  HAL_Delay(700);
    break;
  }
}

/*!
 *  @brief  Reads the raw red, green, blue and clear channel values in
 *          one-shot mode (e.g., wakes from sleep, takes measurement, enters
 *          sleep)
 *  @param  *r
 *          Red value
 *  @param  *g
 *          Green value
 *  @param  *b
 *          Blue value
 *  @param  *c
 *          Clear channel value
 */
void light_getRawDataOneShot(uint16_t *r, uint16_t *g, uint16_t *b,
                                          uint16_t *c) {
//  if (!_tcs34725Initialised)
//	  light_begin();

  light_enable();
  light_getRawData(r, g, b, c);
  light_disable();
}

/*!
 *  @brief  Read the RGB color detected by the sensor.
 *  @param  *r
 *          Red value normalized to 0-255
 *  @param  *g
 *          Green value normalized to 0-255
 *  @param  *b
 *          Blue value normalized to 0-255
 */
void light_getRGB(float *r, float *g, float *b) {
  uint16_t red, green, blue, clear;
  light_getRawData(&red, &green, &blue, &clear);
  uint32_t sum = clear;

  // Avoid divide by zero errors ... if clear = 0 return black
  if (clear == 0) {
    *r = *g = *b = 0;
    return;
  }

  *r = (float)red / sum * 255.0;
  *g = (float)green / sum * 255.0;
  *b = (float)blue / sum * 255.0;
}

/*!
 *  @brief  Converts the raw R/G/B values to color temperature in degrees Kelvin
 *  @param  r
 *          Red value
 *  @param  g
 *          Green value
 *  @param  b
 *          Blue value
 *  @return Color temperature in degrees Kelvin
 */
uint16_t light_calculateColorTemperature(uint16_t r, uint16_t g,
                                                      uint16_t b) {
  float X, Y, Z; /* RGB to XYZ correlation      */
  float xc, yc;  /* Chromaticity co-ordinates   */
  float n;       /* McCamy's formula            */
  float cct;

  if (r == 0 && g == 0 && b == 0) {
    return 0;
  }

  /* 1. Map RGB values to their XYZ counterparts.    */
  /* Based on 6500K fluorescent, 3000K fluorescent   */
  /* and 60W incandescent values for a wide range.   */
  /* Note: Y = Illuminance or lux                    */
  X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
  Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
  Z = (-0.68202F * r) + (0.77073F * g) + (0.56332F * b);

  /* 2. Calculate the chromaticity co-ordinates      */
  xc = (X) / (X + Y + Z);
  yc = (Y) / (X + Y + Z);

  /* 3. Use McCamy's formula to determine the CCT    */
  n = (xc - 0.3320F) / (0.1858F - yc);

  /* Calculate the final CCT */
  cct =
      (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

  /* Return the results in degrees Kelvin */
  return (uint16_t)cct;
}

/*!
 *  @brief  Converts the raw R/G/B values to color temperature in degrees
 *          Kelvin using the algorithm described in DN40 from Taos (now AMS).
 *  @param  r
 *          Red value
 *  @param  g
 *          Green value
 *  @param  b
 *          Blue value
 *  @param  c
 *          Clear channel value
 *  @return Color temperature in degrees Kelvin
 */
uint16_t light_calculateColorTemperature_dn40(uint16_t r,
                                                           uint16_t g,
                                                           uint16_t b,
                                                           uint16_t c) {
  uint16_t r2, b2; /* RGB values minus IR component */
  uint16_t sat;    /* Digital saturation level */
  uint16_t ir;     /* Inferred IR content */

  if (c == 0) {
    return 0;
  }

  /* Analog/Digital saturation:
   *
   * (a) As light becomes brighter, the clear channel will tend to
   *     saturate first since R+G+B is approximately equal to C.
   * (b) The TCS34725 accumulates 1024 counts per 2.4ms of integration
   *     time, up to a maximum values of 65535. This means analog
   *     saturation can occur up to an integration time of 153.6ms
   *     (64*2.4ms=153.6ms).
   * (c) If the integration time is > 153.6ms, digital saturation will
   *     occur before analog saturation. Digital saturation occurs when
   *     the count reaches 65535.
   */
  if ((256 - _tcs34725IntegrationTime) > 63) {
    /* Track digital saturation */
    sat = 65535;
  } else {
    /* Track analog saturation */
    sat = 1024 * (256 - _tcs34725IntegrationTime);
  }

  /* Ripple rejection:
   *
   * (a) An integration time of 50ms or multiples of 50ms are required to
   *     reject both 50Hz and 60Hz ripple.
   * (b) If an integration time faster than 50ms is required, you may need
   *     to average a number of samples over a 50ms period to reject ripple
   *     from fluorescent and incandescent light sources.
   *
   * Ripple saturation notes:
   *
   * (a) If there is ripple in the received signal, the value read from C
   *     will be less than the max, but still have some effects of being
   *     saturated. This means that you can be below the 'sat' value, but
   *     still be saturating. At integration times >150ms this can be
   *     ignored, but <= 150ms you should calculate the 75% saturation
   *     level to avoid this problem.
   */
  if ((256 - _tcs34725IntegrationTime) <= 63) {
    /* Adjust sat to 75% to avoid analog saturation if atime < 153.6ms */
    sat -= sat / 4;
  }

  /* Check for saturation and mark the sample as invalid if true */
  if (c >= sat) {
    return 0;
  }

  /* AMS RGB sensors have no IR channel, so the IR content must be */
  /* calculated indirectly. */
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;

  /* Remove the IR component from the raw RGB values */
  r2 = r - ir;
  b2 = b - ir;

  if (r2 == 0) {
    return 0;
  }

  /* A simple method of measuring color temp is to use the ratio of blue */
  /* to red light, taking IR cancellation into account. */
  uint16_t cct = (3810 * (uint32_t)b2) / /** Color temp coefficient. */
                     (uint32_t)r2 +
                 1391; /** Color temp offset. */

  return cct;
}

/*!
 *  @brief  Converts the raw R/G/B values to lux
 *  @param  r
 *          Red value
 *  @param  g
 *          Green value
 *  @param  b
 *          Blue value
 *  @return Lux value
 */
uint16_t light_calculateLux(uint16_t r, uint16_t g, uint16_t b) {
  float illuminance;

  /* This only uses RGB ... how can we integrate clear or calculate lux */
  /* based exclusively on clear since this might be more reliable?      */
  illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

  return (uint16_t)illuminance;
}

/*!
 *  @brief  Sets inerrupt for TCS34725
 *  @param  i
 *          Interrupt (True/False)
 */
void light_setInterrupt(bool i) {
  uint8_t r = light_read8(TCS34725_ENABLE);
  if (i) {
    r |= TCS34725_ENABLE_AIEN;
  } else {
    r &= ~TCS34725_ENABLE_AIEN;
  }
  light_write8(TCS34725_ENABLE, r);
}

/*!
 *  @brief  Clears inerrupt for TCS34725
 */
void light_clearInterrupt() {

	uint8_t packet = TCS34725_COMMAND_BIT | 0x66;
	HAL_I2C_Master_Transmit(i2c_periph, _deviceAddress_light, &packet, 1, 1000);

}

/*!
 *  @brief  Sets inerrupt limits
 *
 *  @param  low
 *          Low limit
 *  @param  high
 *          High limit
 */
void light_setIntLimits(uint16_t low, uint16_t high) {
	light_write8(0x04, low & 0xFF);
	light_write8(0x05, low >> 8);
  light_write8(0x06, high & 0xFF);
  light_write8(0x07, high >> 8);
}



/*************************************************************
 *
 * FREERTOS WRAPPER FUNCTIONS
 *
*************************************************************/
