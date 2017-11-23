#include "stdint.h"
#include <em_i2c.h>

#define STC3105_Address 0xE0

#define REG_MODE		 0
	#define PWR_SAVE_ACTIVE_MODE           0
	#define PWR_SAVE_POWER_SAVING_MODE     4
	#define ALM_ENA_DISABLED               0
	#define ALM_ENA_ENABLE	               8
	#define GG_RUN_STANDBY_MODE            0
    #define GG_RUN_OPERATING_MODE          16
#define REG_CTRL		 1
	#define IO_DATA_ALM_OFF                0
	#define IO_DATA_ALM_ON                 1
	#define GG_RST_NO_EFFECT               0
	#define GG_RST_ON                      2
	#define PORDET_RELEASE_SOFT_RESTART    0
	#define PORDET_RELEASE_ASSERT_RESTART  16
	#define ALM_SOC_CLEAR	 			   0
	#define ALM_SOC_SET					   32
	#define ALM_VOLT_CLEAR	 			   0
	#define ALM_VOLT_SET				   64
#define REG_CHARGE_LOW	 2
#define REG_CHARGE_HIGH	 3
#define REG_COUNTER_LOW	 4
#define REG_COUNTER_HIGH 5
#define REG_CURRENT_LOW	 6
#define REG_CURRENT_HIGH 7
#define REG_VOLTAGE_LOW	 8
#define REG_VOLTAGE_HIGH 9
#define REG_SOC_BASE_LOW 10
#define REG_SOC_BASE_HIGH 11
#define REG_ALARM_SOC_LOW 12
#define REG_ALARM_SOC_HIGH 13
#define REG_ALARM_VOLTAGE 14
#define REG_CURRENT_THRES 15
#define REG_RELAX_COUNT  16
#define REG_ID           24
#define REG_RAM0		 32
#define REG_RAM1		 33
#define REG_RAM2		 34
#define REG_RAM3		 35
#define REG_RAM4		 36
#define REG_RAM5		 37
#define REG_RAM6		 38
#define REG_RAM7		 39
#define REG_RAM8		 40
#define REG_RAM9		 41
#define REG_RAM10		 42
#define REG_RAM11		 43
#define REG_RAM12		 44
#define REG_RAM13		 45
#define REG_RAM14		 46
#define REG_RAM15		 47

void STC3105_Init_ConfigMode(uint8_t reg, uint8_t config);
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

void STC3105_INIT_I2C(void);
