#include "stdint.h"
#include <em_i2c.h>

#define STC3105_Address 0xE0

#define STC3105_R_MODE		 			   	0
	#define PWR_SAVE_ACTIVE_MODE           	0
	#define PWR_SAVE_POWER_SAVING_MODE     	4
	#define ALM_ENA_DISABLED               	0
	#define ALM_ENA_ENABLE	               	8
	#define GG_RUN_STANDBY_MODE            	0
    #define GG_RUN_OPERATING_MODE          	16
#define STC3105_R_CTRL		 			   	1
	#define IO_DATA_ALM_OFF                	0
	#define IO_DATA_ALM_ON                 	1
	#define GG_RST_NO_EFFECT               	0
	#define GG_RST_ON                      	2
	#define PORDET_RELEASE_SOFT_RESTART    	0
	#define PORDET_RELEASE_ASSERT_RESTART  	16
	#define ALM_SOC_CLEAR	 			   	0
	#define ALM_SOC_SET					   	32
	#define ALM_VOLT_CLEAR	 			   	0
	#define ALM_VOLT_SET				   	64
#define STC3105_R_CHARGE_LOW	 			2
#define STC3105_R_CHARGE_HIGH	 			3
#define STC3105_R_COUNTER_LOW	 			4
#define STC3105_R_COUNTER_HIGH 				5
#define STC3105_R_CURRENT_LOW	 			6
#define STC3105_R_CURRENT_HIGH 				7
#define STC3105_R_VOLTAGE_LOW	 			8
#define STC3105_R_VOLTAGE_HIGH 				9
#define STC3105_R_SOC_BASE_LOW 				10
#define STC3105_R_SOC_BASE_HIGH 			11
#define STC3105_R_ALARM_SOC_LOW 			12
#define STC3105_R_ALARM_SOC_HIGH 			13
#define STC3105_R_ALARM_VOLTAGE 			14
#define STC3105_R_CURRENT_THRES 			15
#define STC3105_R_RELAX_COUNT  				16
#define STC3105_R_ID           				24
#define STC3105_R_RAM0		 				32
#define STC3105_R_RAM1		 				33
#define STC3105_R_RAM2		 				34
#define STC3105_R_RAM3		 				35
#define STC3105_R_RAM4		 				36
#define STC3105_R_RAM5		 				37
#define STC3105_R_RAM6		 				38
#define STC3105_R_RAM7		 				39
#define STC3105_R_RAM8		 				40
#define STC3105_R_RAM9		 				41
#define STC3105_R_RAM10		 				42
#define STC3105_R_RAM11		 				43
#define STC3105_R_RAM12		 				44
#define STC3105_R_RAM13		 				45
#define STC3105_R_RAM14		 				46
#define STC3105_R_RAM15		 				47

typedef struct{
	uint8_t REG_MODE;
	uint8_t REG_CTRL;
	uint8_t REG_CHARGE_Lo;
	uint8_t REG_CHARGE_Hi;
	uint8_t REG_COUNTER_Lo;
	uint8_t REG_COUNTER_Hi;
	uint8_t REG_CURRENT_Lo;
	uint8_t REG_CURRENT_Hi;
	uint8_t REG_VOLTAGE_Lo;
	uint8_t REG_VOLTAGE_Hi;
	uint8_t REG_SOC_BASE_Lo;
	uint8_t REG_SOC_BASE_Hi;
	uint8_t REG_ALARM_SOC_Lo;
	uint8_t REG_ALARM_SOC_Hi;
	uint8_t REG_ALARM_VOLTAGE;
	uint8_t REG_CURRENT_THRES;
	uint8_t REG_RELAX_COUNT;
	uint8_t REG_ID;
}tREG_STC3105;

typedef struct{
	tREG_STC3105 REG;
	uint8_t Init;
	uint8_t Access;
	uint16_t Voltage;

	uint8_t temp;

}tSTC3105;
tSTC3105 STC3105_U1;

void STC3105_Get_ID(void);
void STC3105_Init_Default(void);
void STC3105_Init_ConfigMode(uint8_t reg, uint8_t config);

void STC3105_GET_Register(tSTC3105 *U);


void STC3105_WriteRegister(uint8_t reg, uint8_t value);

uint8_t STC3105_ReadRegister(uint8_t reg);
uint8_t STC3105_GET_ALARM();
uint8_t STC3105_GET_CURRENT_THRES();
uint16_t STC3105_GET_CHARGE();
uint16_t STC3105_GET_COUNTER();
uint16_t STC3105_GET_CURRENT();
uint16_t STC3105_GET_VOLTAGE();
uint16_t STC3105_GET_SOC_BASE();
uint16_t STC3105_GET_ALARM_SOC();



void STC3105_SET_SOC_BASE(uint16_t value);
void STC3105_RESET_ALM();
void STC3105_SET_ALARM(uint8_t alarm);
void STC3105_SET_ALARM_SOC(uint16_t value);
void STC3105_SET_CURRENT_THRES(uint8_t current_thres);


