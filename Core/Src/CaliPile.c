/* 06/16/2017 Copyright Tlera Corporation
 *  
 *  Created by Kris Winer
 *  
 This sketch uses SDA/SCL on pins 20/21 (back pads), respectively, and it uses the Butterfly STM32L4e33CU Breakout Board.
 The CaliPile is a simple thermipole but withon-board processing that makes it very versatile as well as low power; 
 ower consumption in only 20 microAmp
 
 Library may be used freely and without limit with attribution.
 
*/

#include "CaliPile.h"
#include "i2c.h"
#include "stm32wbxx_hal.h"
#include "stdint.h"
#include "math.h"


uint8_t _deviceAddress;
uint8_t _int;
GPIO_TypeDef* _intPort;
I2C_HandleTypeDef* i2c_periph;

uint16_t _PTAT25, _M, _U0, _CHECKSUM;
uint32_t _UOUT1;
uint8_t  _TOBJ1,  _LOOKUP;
float _k;

uint8_t rawData[3] = {0, 0, 0};
uint8_t packet[3] = {0, 0, 0};


void calipile_setup(uint8_t deviceAddress, uint8_t intPin, GPIO_TypeDef* intPort, I2C_HandleTypeDef* periph){
	_deviceAddress = deviceAddress << 1;
	_int = intPin;
	_intPort = intPort;
	i2c_periph = periph;
}

void calipile_wake(){
	// issue general call and reload command
	packet[0] = 0x00;
	HAL_I2C_Mem_Write(i2c_periph, 0x00, 0x04, I2C_MEMADD_SIZE_8BIT, packet, 1, 1000);
//	calipile_writeByte(0x04, packet);
	HAL_Delay(1);
}

//float calipile_getTPOBJ_kelvin(){
//
//	uint32_t tempAmb = calipile_getTPAMB();
//	uint32_t tempObj = calipile_getTPOBJ();
//
//	return (25+273.15) + (tempAmb - _PTAT25) * (1/_M);
//}

void calipile_initMotion(uint8_t tcLP1, uint8_t tcLP2, uint8_t TPsource, uint8_t cycTime){
	// issue general call and reload command
	uint8_t packet = 0x00;

	 // Initialize the sensor for motion and presence detection
	  // Tthr (bit 4), presence (bit(3), motion (bit 2), amb shock (bit 1), timer (bit 0) interrupts allowed
	calipile_writeByte(CALIPILE_INT_MASK, &packet);

	  // time constant for LP1 (bits 0 - 3) and LP2 (bits 4 - 7)
	  packet = tcLP2 << 4 | tcLP1;
	  calipile_writeByte(CALIPILE_SLP12, &packet);

	  // select cycle time (bits 0 - 1) for motion detection, source (bits) 2 - 3) for presence detection
      uint8_t temp;
	  calipile_readBytes(CALIPILE_SRC_SELECT, &temp, 1);
	  packet = temp | TPsource << 2 | cycTime;
	  calipile_writeByte(CALIPILE_SRC_SELECT, &packet);

	  // select motion threshold
	  packet = 0x22;  // presence threshold, set at 50 counts
	  calipile_writeByte(CALIPILE_TP_PRES_THLD, &packet);

	  packet = 0x0A; // motion threshold, set at 10 counts
	  calipile_writeByte(CALIPILE_TP_MOT_THLD, &packet);
}



void calipile_initTempThr(uint16_t Tcounts){
	// specify the over temperature interrupt threshold (2 bytes)
	calipile_writeByte(CALIPILE_TPOT_THR, &Tcounts); // 0x83 means 67,072 counts as threshold
	calipile_writeByte((CALIPILE_TPOT_THR + 1), 0x00);
	uint8_t temp;
	calipile_readByte(CALIPILE_SRC_SELECT, &temp);

	uint8_t packet = temp | 0x10;
	calipile_writeByte(CALIPILE_SRC_SELECT, &packet); // interrupt on exceeding threshold

	// Verify threshold set
	calipile_readBytes(CALIPILE_TPOT_THR, rawData, 2);
	uint16_t TPOTTHR = ((uint16_t) rawData[0] << 8) | rawData[1];
//	Serial.print("Overtemp threshold = "); Serial.println(TPOTTHR * 2);
}

uint16_t calipile_getTPAMB(){
	calipile_readBytes(CALIPILE_TPAMBIENT, rawData, 2);
	uint16_t temp = ( (uint16_t)(rawData[0] & 0x7F) << 8) | rawData[1] ;
	return temp;
}
uint16_t calipile_getTPOBJ(){
	calipile_readBytes(CALIPILE_TPOBJECT, rawData, 3);
	uint32_t temp = ( (uint32_t) ( (uint32_t)rawData[0] << 24) | ( (uint32_t)rawData[1] << 16) | ( (uint32_t)rawData[2] & 0x80) << 8) >> 15;
	return temp;
}
uint16_t calipile_getTPOBJLP1(){
	calipile_readBytes(CALIPILE_TPOBJLP1, rawData, 3);
	uint32_t temp = ( ((uint32_t) rawData[0] << 16) | ((uint32_t) rawData[1] << 8) | ( (uint32_t)rawData[2] & 0xF0) ) >> 4;
	temp /= 8;
	return temp;
}
uint16_t calipile_getTPOBJLP2(){
	calipile_readBytes(CALIPILE_TPOBJLP2, rawData, 3);;
	uint32_t temp = ((uint32_t) (rawData[0] & 0x0F) << 16) | ((uint32_t) rawData[1] << 8) | rawData[2] ;
	temp /= 8;
	return temp;
}
uint16_t calipile_getTPAMBLP3(){
	calipile_readBytes(CALIPILE_TPAMBLP3, rawData, 3);;
	uint8_t temp = ((uint16_t) rawData[0] << 8) | rawData[1];
	temp /= 2;
	return temp;
}
uint16_t calipile_getTPOBJLP2FRZN(){
	   calipile_readBytes(CALIPILE_TPOBJLP2_FRZN, rawData, 3);
	   uint32_t temp = ((uint32_t) rawData[0] << 16) | ((uint32_t) rawData[1] << 8) | rawData[2];
	   temp /= 128;
	   return temp;
}


uint8_t calipile_getTPPRESENCE(){
      uint8_t temp;
                  calipile_readByte(CALIPILE_TPPRESENCE, &temp);
      return temp;
  }


  uint8_t calipile_getTPMOTION(){
      uint8_t temp;
            calipile_readByte(CALIPILE_TPMOTION, &temp);
      return temp;
  }

    uint8_t calipile_getTPAMBSHK(){
      uint8_t temp;
      calipile_readByte(CALIPILE_TPAMB_SHOCK, &temp);
      return temp;
  }

    float calipile_getTamb(uint16_t TPAMB){
      float temp = 298.15f + ((float)TPAMB - (float) _PTAT25) * (1.0f/(float) _M);
      return temp;
  }

    float calipile_getTobj(uint32_t TPOBJ, float Tamb){
      float temp0 = powf(Tamb, 3.8f);
      float temp1 = ( ((float) TPOBJ) - ((float) _U0)  ) / _k ;
      float temp3 = powf( (temp0 + temp1), 0.2631578947f );
      return temp3;
  }


 uint8_t  calipile_checkIntStatus(){
    uint8_t temp;
                calipile_readByte(CALIPILE_INTERRUPT_STATUS, &temp);
    return temp;
  }



  void calipile_readEEPROM()
  {
 
  uint8_t rawData[2] = {0, 0};
  uint8_t packet;
  /* Start of EEPROM operations, just have to do once *************************************************** */
 // Check EEPROM protocol number as a test of I2C communication 
  packet = 0x80;
  calipile_writeByte(CALIPILE_EEPROM_CONTROL, &packet); // enable EEPROM read
   
  uint8_t c;
  calipile_readByte(CALIPILE_EEPROM_PROTOCOL, &c);
//  Serial.print("CaliPile EEPROM protocol number is "); Serial.println(c);
//  Serial.println("CaliPile EEPROM protocol number should be 3");

  uint8_t d;
  calipile_readByte(CALIPILE_SLAVE_ADDRESS, &d);
//  Serial.print("CaliPile EEPROM slave address is "); Serial.println(d);
//  Serial.println("CaliPile EEPROM slave address should be 140");
//  Serial.println(" ");

  // Read the EEPROM calibration constants

  calipile_readByte(CALIPILE_EEPROM_LOOKUPNUM, &_LOOKUP);
//  Serial.print("CaliPile LookUpNumber is "); Serial.println(_LOOKUP);

  calipile_readBytes(CALIPILE_EEPROM_PTAT25, &rawData[0], 2);
  _PTAT25 = ( (uint16_t) rawData[0] << 8) | rawData[1];
//  Serial.print("CaliPile PTAT25 is "); Serial.println(_PTAT25);

  calipile_readBytes(CALIPILE_EEPROM_M, &rawData[0], 2);
  _M = ( (uint16_t) rawData[0] << 8) | rawData[1];
  _M /= 100;
//  Serial.print("CaliPile M is "); Serial.println(_M);

  calipile_readBytes(CALIPILE_EEPROM_U0, &rawData[0], 2);
  _U0 = ( (uint16_t) rawData[0] << 8) | rawData[1];
  _U0 += 32768;
//  Serial.print("CaliPile U0 is "); Serial.println(_U0);

  calipile_readBytes(CALIPILE_EEPROM_UOUT1, &rawData[0], 2);
  _UOUT1 = ( (uint16_t) rawData[0] << 8) | rawData[1];
  _UOUT1 *= 2;
//  Serial.print("CaliPile UOUT1 is "); Serial.println(_UOUT1);

  calipile_readByte(CALIPILE_EEPROM_TOBJ1, &_TOBJ1);
//  Serial.print("CaliPile TOBJ1 is "); Serial.println(_TOBJ1);

  calipile_readBytes(CALIPILE_EEPROM_CHECKSUM, &rawData[0], 2);
  _CHECKSUM = ( (uint16_t) rawData[0] << 8) | rawData[1];
//  Serial.print("CaliPile CHECKSUM is supposed to be "); Serial.println(_CHECKSUM);

  // Calculate the checksum
  uint16_t sum = 0;
  uint8_t temp;
  for(int ii = 35; ii < 64; ii++)
  {
   calipile_readByte(ii, &temp);
   sum += temp;
  }
//  Serial.print("CaliPile CHECKSUM is "); Serial.println(sum + c);

  packet = 0x00;
  calipile_writeByte(CALIPILE_EEPROM_CONTROL, &packet); // enable EEPROM read

  /* End of EEPROM operations, just have to do once *************************************************** */

  // Construct needed calibration constants (just need to calculate once)
  _k = ( (float) (_UOUT1 - _U0) )/(powf((float)(_TOBJ1 + 273.15f), 3.8f) - powf(25.0f + 273.15f, 3.8f) );

  }

    void calipile_writeByte(uint8_t reg, uint8_t* packet){
      HAL_I2C_Mem_Write(i2c_periph, _deviceAddress, reg, I2C_MEMADD_SIZE_8BIT, packet, 1, 1000);

//      uint8_t write_byte[2];
//      write_byte[0] = reg;
//      write_byte[1] = *packet;

//  	HAL_I2C_Master_Transmit(i2c_periph, _deviceAddress, write_byte, 2, 100);

    }

    void calipile_readByte(uint8_t reg, uint8_t* packet){
    	HAL_I2C_Mem_Read(i2c_periph, _deviceAddress, reg, I2C_MEMADD_SIZE_8BIT, packet, 1, 1000);
//    	HAL_I2C_Master_Transmit(i2c_periph, _deviceAddress, &reg, 1, 100);
//    	HAL_I2C_Master_Receive(i2c_periph, _deviceAddress, packet, 1, 100);
    }

    void calipile_readBytes(uint8_t reg, uint8_t* packet, uint8_t len){
    	HAL_I2C_Mem_Read(i2c_periph, _deviceAddress, reg, I2C_MEMADD_SIZE_8BIT, packet, len, 1000);
//    	packet[0] = CALIPILE_TPOBJECT;
//    	uint8_t temp;
//    	HAL_I2C_Master_Transmit(i2c_periph, _deviceAddress, &reg, 1, 100);
//    	HAL_I2C_Master_Receive(i2c_periph, _deviceAddress, packet, len, 100);

    }




//  // I2C scan function
//  void CALIPILE::I2Cscan()
//   {
//  // scan for i2c devices
//  byte error, address;
//  int nDevices;
//
//  Serial.println("Scanning...");
//
//  nDevices = 0;
//  for(address = 1; address < 127; address++ )
//  {
//    // The i2c_scanner uses the return value of
//    // the Write.endTransmission to see if
//    // a device did acknowledge to the address.
//    Wire.beginTransmission(address);
//    error = Wire.endTransmission();
//
//    if (error == 0)
//    {
//      Serial.print("I2C device found at address 0x");
//      if (address<16)
//        Serial.print("0");
//      Serial.print(address,HEX);
//      Serial.println("  !");
//
//      nDevices++;
//    }
//    else if (error==4)
//    {
//      Serial.print("Unknown error at address 0x");
//      if (address<16)
//        Serial.print("0");
//      Serial.println(address,HEX);
//    }
//  }
//  if (nDevices == 0)
//    Serial.println("No I2C devices found\n");
//  else
//    Serial.println("done\n");
//
//  }
