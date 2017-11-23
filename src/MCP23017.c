
#include "MCP23017.h"
#include "tester.h"
#include "em_i2c.h"
#include "em_timer.h"

void MCP23017_Configure_Default(uint8_t add)
{
	/*
	 * 	bit7 -> Bank
	 * 	bit6 -> Mirror
	 * 	bit5 -> SEQOP
	 * 	bit4 -> DISSLW
	 * 	bit3 -> HAEN
	 * 	bit2 -> ODR
	 * 	bit1 -> INTPO00
	 * 	bit0 -> Unimplemented
	 */

	//              76543210
	uint8_t con = 0x42; //0100 0010
	uint8_t Tab[2];
	Tab[0] = MCP23017_IOCONA;
	Tab[1] = con;

	i2cTransfert.addr = add;
	i2cTransfert.flags = I2C_FLAG_WRITE_WRITE;
	i2cTransfert.buf[0].data = Tab;
	i2cTransfert.buf[0].len = 1;

	i2cTransfert.buf[1].data = Tab+1;
	i2cTransfert.buf[1].len = 1;

	ret = I2C_TransferInit(I2C1, &i2cTransfert);
	while ( ret == i2cTransferInProgress) {
	      ret = I2C_Transfer(I2C1);
	}

}

uint8_t MCP23017_Write_Register(uint8_t reg, uint8_t add,uint8_t value)
{
	uint8_t ui8result = 0;
	uint8_t Tab[2];
	Tab[0] = reg;
	Tab[1] = value;

	i2cTransfert.addr = add;// Chip address
	i2cTransfert.flags = I2C_FLAG_WRITE_WRITE;
	i2cTransfert.buf[0].data = Tab;
	i2cTransfert.buf[0].len = 1;

	i2cTransfert.buf[1].data = Tab+1;
	i2cTransfert.buf[1].len = 1;

	ret = I2C_TransferInit(I2C1, &i2cTransfert);
	while ( ret == i2cTransferInProgress) {
	      ret = I2C_Transfer(I2C1);
	}
	return ui8result;

}
uint8_t MCP23017_Read_Register(uint8_t reg, uint8_t address)
{
	uint8_t ui8result =0;
	uint8_t regid;

	i2cTransfert.addr = address;
	i2cTransfert.flags = I2C_FLAG_WRITE_READ;/* Select register to be read */
	regid = reg;
	i2cTransfert.buf[0].data = &regid;
	i2cTransfert.buf[0].len = 1;

	i2cTransfert.buf[1].data=&ui8result;
	i2cTransfert.buf[1].len = 1;

	/* Do a polled transfer */
	ret = I2C_TransferInit(I2C1, &i2cTransfert);
	while (ret == i2cTransferInProgress){
		ret = I2C_Transfer(I2C1);
	}

	return ui8result;
}

void MCP23017_Init_I2C(void)
{
	//I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
	GPIO_PinModeSet(gpioPortC, 4, gpioModeWiredAndPullUpFilter, 1); //sda pc4 e0
	GPIO_PinModeSet(gpioPortC, 5,gpioModeWiredAndPullUpFilter, 1); //scl pc5 e1
	GPIO_PinOutSet(gpioPortC,4);
	GPIO_PinOutSet(gpioPortC,5);

	CMU_ClockEnable(cmuClock_I2C1,true);
	I2C_Reset(I2C1);

	I2C1->CTRL = (1 << 2) | (1 << 0);           // auto acknowledge, enable I2C module
	I2C1->CMD = (1 << 7) | (1 << 6) | (1 << 5); // Clear pending commands, clear TX buffer and shift reg, issue abort command to clear bus busy bit
	I2C1->CLKDIV = 0x1FF;                          // allows for 100kHz I2C clock
	//					       //                else    I2C1->CLKDIV = 0x0;
	I2C1->ROUTE = 0x03;//I2C_ROUTE_LOCATION_LOC0|I2C_ROUTE_SCLPEN|I2C_ROUTE_SDAPEN;
}
