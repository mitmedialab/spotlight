#ifdef __cplusplus
extern "C" {
#endif

#include "spotlight_config.h"
#include "stdio.h"
#include "cmsis_os2.h"
#include "usbd_cdc_if.h"
#include "myMain.h"
#include "app_thread.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef SOLAR_SENSOR_NODE
#include "i2c.h"
#endif


#define USB_UART_ACTIVE			1


uint8_t position[2];

//uint16_t power;
//uint16_t current;
//uint16_t shuntVoltage;
//uint16_t busVoltage;

struct SerialMsg{
	struct MeasMsg measMsg;
	char endOfLine[4];
};

extern volatile struct MeasMsg measMsg;
volatile struct SerialMsg serialMsg = {
		.endOfLine="\n\r"
};

char* endOfLine = "\n\r";


void measMsgReceivedFromNode(struct MeasMsg* msg){
	// send over UART if USB UART active
#if USB_UART_ACTIVE
	uint8_t retry = 0;
//	CDC_Transmit_FS(position, sizeof(position));
//	taskENTER_CRITICAL();
	memcpy((struct MeasMsg*) &serialMsg.measMsg, msg, sizeof(struct MeasMsg));
	while(CDC_Transmit_FS((uint8_t* ) msg, sizeof(struct MeasMsg)) != USBD_OK){
		if(retry == 5){
			retry = 0;
			break;
		}
		osDelay(1);
		retry+=1;

	}
	while(CDC_Transmit_FS((uint8_t* ) endOfLine, sizeof(endOfLine)) != USBD_OK){
		if(retry == 5){
			retry = 0;
			break;
		}
		osDelay(1);
		retry+=1;
	}
//	CDC_Transmit_FS((uint8_t* ) msg, sizeof(struct  MeasMsg));
//	osDelay(1);
//	CDC_Transmit_FS((uint8_t* ) endOfLine, sizeof(endOfLine));
//	osDelay(1);
//	taskEXIT_CRITICAL();
#endif


	// TODO add node to table if not already in table

	// TODO check if power rating in table is lower than current one
	//		if so, replace and record current angle measurements

	// TODO if all expected nodes have replied, move to next Spotlight position
	osThreadFlagsSet (defaultTaskHandle, CAL_THREAD_FLAG);
}

#ifdef SOLAR_SENSOR_NODE
void sendPowerMeasurement(otIp6Address peerAddr, int32_t angle_1, int32_t angle_2){
#ifndef SOLAR_SENSOR_NODE_I2C_DISABLE
	measMsg.angle_1 = angle_1;
	measMsg.angle_2 = angle_2;
	measMsg.power = getPower();
	measMsg.current = getCurrent();
	measMsg.shuntVoltage = getShuntVoltage();
	measMsg.busVoltage = getBusVoltage();
#endif
	APP_THREAD_SendCoapMsg(&measMsg, sizeof(struct MeasMsg),
			&multicastAddr, nodeSpotResource,
			NO_ACK, OT_COAP_CODE_PUT, 1U);
}

void powerMeasSetup(void){
	  uint8_t packet[3];
	  packet[0]= 0x05;
	  packet[0]= 0x66;
	  packet[0]= 0xD0;
	  HAL_I2C_Master_Transmit(&hi2c1, POWER_MEAS_ADDR << 1, packet, 3, 100);
}

uint16_t getPower(){

  uint8_t packet = 0x03;
  HAL_I2C_Master_Transmit(&hi2c1, POWER_MEAS_ADDR << 1, &packet, 1, 100);


  // read 1 byte, from address 0
	  uint8_t receiveBuffer[2];
  HAL_I2C_Master_Receive(&hi2c1, POWER_MEAS_ADDR << 1, receiveBuffer, (uint8_t)(2), 100);

  uint16_t measurement;
  measurement = receiveBuffer[0] << 8;
  measurement = measurement | receiveBuffer[1];

  return measurement;
}

uint16_t getCurrent(){
	uint8_t packet = 0x04;
  HAL_I2C_Master_Transmit(&hi2c1, POWER_MEAS_ADDR << 1, &packet, 1, 100);

  // read 1 byte, from address 0
	  uint8_t receiveBuffer[2];
  HAL_I2C_Master_Receive(&hi2c1, POWER_MEAS_ADDR << 1, receiveBuffer, (uint8_t)(2), 100);

  uint16_t measurement;
  measurement = receiveBuffer[0] << 8;
  measurement = measurement | receiveBuffer[1];

  return measurement;
}

uint16_t getBusVoltage(){
	uint8_t packet = 0x02;
	  HAL_I2C_Master_Transmit(&hi2c1, POWER_MEAS_ADDR << 1, &packet, 1, 100);

  // read 1 byte, from address 0
	  uint8_t receiveBuffer[2];
  HAL_I2C_Master_Receive(&hi2c1, POWER_MEAS_ADDR << 1, receiveBuffer, (uint8_t)(2), 100);

  uint16_t measurement;
  measurement = receiveBuffer[0] << 8;
  measurement = measurement | receiveBuffer[1];

  return measurement;
}

uint16_t getShuntVoltage(){
	uint8_t packet = 0x01;
	  HAL_I2C_Master_Transmit(&hi2c1, POWER_MEAS_ADDR << 1, &packet, 1, 100);

  // read 1 byte, from address 0
	  uint8_t receiveBuffer[2];
  HAL_I2C_Master_Receive(&hi2c1, POWER_MEAS_ADDR << 1, receiveBuffer, (uint8_t)(2), 100);

  uint16_t measurement;
  measurement = receiveBuffer[0] << 8;
  measurement = measurement | receiveBuffer[1];

  return measurement;
}

#endif

#ifdef __cplusplus
}
#endif

