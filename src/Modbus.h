/*
 * Modbus.h
 *
 *  Created on: 13 paü 2017
 *      Author: mikolajp
 */
#include "stdint.h"
#include "stdbool.h"

#ifndef SRC_MODBUS_H_
#define SRC_MODBUS_H_

#define Modbus_Address_Device     0x10
#define Modbus_Function_Write     0x10
#define Modbus_Function_Read      0x03
#define Modbus_Data_OFFSET        0x00
//------------------------------------
//00 Reset
//01
//02
//03
//50 Temperatura
//52 Test
uint16_t Modbus_Register[100];

void Modbus_Main(void);
uint8_t Modbus_Buffor_Tab[20];

uint16_t CRC16 (const uint8_t *nData, uint16_t wLength);
void     ModbusWrite(uint8_t* SendBuffor,uint8_t len);


#endif /* SRC_MODBUS_H_ */
