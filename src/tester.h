#include "em_gpio.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "my_timer.h"
#include "myUSB.h"
#include <em_i2c.h>

#define ZAL_LD_RS485       128
#define ZAL_POM_I_TC	   32
#define ZAL_POM_VZAS_I	   16
#define ZAL_POM_VZAS2      8
#define ZAL_POM_VZAS1	   4
#define ZAL_POM_VBAT       2
#define ZAL_POM_VZAS2_VBAT 1

#define HZ 			1
#define DC 			2
#define EEPROM_TEST 3

#define Printf_VDD 1
#define Printf_Temp 2
#define Printf_VREF 3
#define Printf_VAV 4



///-------------------------- definicja typow kart pomiarowych ------------------------

#define PP_100mA 1
#define PP_100mV 2
#define PP_500V 3
#define PP_10V 4
#define PP_5mA 5

#define PP_VERSION 1

uint8_t PP_FLAGS;
uint8_t PP_ADC_RDY;

I2C_TransferSeq_TypeDef i2cTransfert;
I2C_TransferReturn_TypeDef ret;


typedef struct
{
    uint8_t MUX0;
    uint8_t VBIAS;
    uint8_t MUX1;
    uint8_t SYS0;
    uint8_t OFC0;
    uint8_t OFC1;
    uint8_t OFC2;
    uint8_t FSC0;
    uint8_t FSC1;
    uint8_t FSC2;
    uint8_t IDAC0;
    uint8_t IDAC1;
    uint8_t GPIOCFG;
    uint8_t GPIODIR;
    uint8_t GPIODAT;
}ADC_profile;

typedef enum external{Podlaczono=0, Niepodlaczono=1, Blad=2}External;
typedef enum pkx{Wylaczony=0, Wlaczony=1}PKX;
typedef enum teZadanie{Nieogloszone=0,Ogloszone=1,Wykonywane=2}eZadanie;

typedef struct
{
	eZadanie PK1;//Przekaznik PK1
	eZadanie PK2;
	eZadanie VZAS1;
	eZadanie VZAS2;
	eZadanie VBAT;
	eZadanie I_TC;
	eZadanie STC3105_Get_Voltage;
	eZadanie STC3105_Get_Current;
	eZadanie STC3105_Get_Counter;
	eZadanie STC3105_Get_Culomb;
	eZadanie STC3105_Restart;
	eZadanie STC3105_Get_Config;
}tZadanie;

typedef struct
{
	float VZAS1;
	float VZAS2;
	float VBAT;
	float I_TC;
	uint16_t STC3105_Voltage;
	float STC3105_Current;
	uint16_t ui16Buffor;
	uint8_t ui8Buffor_Hi;
	uint8_t ui8Buffor_Lo;
}tPomiary;

typedef struct
{
	External JEST_MZB;
	External JEST_STER;
	External JEST_PKSPP;
}Peryferia;

typedef struct
{
	PKX PK1;
	uint8_t PK1_value;
 	PKX PK2;
 	uint8_t PK2_value;
}Przekazniki;

 struct
{
	Peryferia Plytki;
	Przekazniki Przekazniki;
	tZadanie Zadanie;
	tPomiary Pomiary;
}Tester;




void Init_Tester();
void wstrzymaj(uint8_t P);


void SEL_SPI1_TC_ON();
void SEL_SPI1_TC_OFF();

void SEL_SPI1_PKSPP_ON();
void SEL_SPI1_PKSPP_OFF();

void SEL_SPI_SL_ON();
void SEL_SPI_SL_OFF();

void ZAL_TC_PKSPP_ON(void);
void ZAL_TC_PKSPP_OFF(void);

uint8_t PP_Z1(uint8_t add, uint8_t CS); //Plytka pomiarowa Z1

/*
 *
 */
void PP_ReadEEPROM(uint8_t PP_No,ADC_profile *profil,uint8_t typ,uint8_t PIN);
void PP_WriteEEPROM(uint8_t PP_No,ADC_profile *profil,uint8_t typ,uint8_t PIN);
void PP_PWR_ON(uint8_t PIN, uint8_t zalacz);
void PP_Select_EEPROM();
void PP_Select_ADC();
uint8_t PP_Select_CS_EEPROM(uint8_t PIN);
uint8_t PP_UnSelect_CS_EEPROM();
uint8_t PP_Select_CS_ADC(uint8_t PIN);
uint8_t PP_UnSelect_CS_ADC();
uint8_t PP_CheckEEPROM(uint8_t PIN);
void PP_StatusEEPROM(uint8_t PIN);
//void  PP_LoadType(uint8_t PP_No,UDRegister *typ, uint8_t PIN);


/*
 * ADC
 */
void ResetADC(uint8_t PIN);
void PP_INIT_ADC(uint8_t PIN);
uint8_t PP_StartMeasure(uint8_t PIN);
uint8_t PP_StopMeasure(uint8_t PIN);
float PP_SingleM(uint8_t PIN, ADC_profile *profil);
float PP_SingleMinfo(uint8_t PIN, ADC_profile profil);
float PP_ReadVdd(int PIN);
float PP_ReadTemp(int PIN);
float PP_ReadVref(int PIN);
float PP_ReadVav(int PIN);
void PP_INIT_ADC(uint8_t PIN);
ADC_profile PP_ReadADCConfig (uint8_t PIN, ADC_profile profil);
void PP_WriteADCConfig(uint8_t PIN, ADC_profile profil);
//void PP_FormatEEPROM(uint8_t PP_No,uint8_t PP_Type,uint8_t PP_Version,uint8_t PP_Device,uint8_t PIN);
int power2( unsigned int exp); //mnoznik PGA

void Timer2_Configure(uint16_t scaler);



/*
 * LED
 */
void Tester_LED_RED(void);
void Tester_LED_GREEN(void);
void Tester_LED_BLUE(void);

/*
 * Przekaünik
 */
void Tester_Relay_PK1_ON(void);
void Tester_Relay_PK1_OFF(void);

void Tester_Relay_PK2_ON(void);
void Tester_Relay_PK2_OFF(void);

/*
 * Dostep do peryferiow
 */
void Tester_Acces_to_ALL(void);
uint8_t Tester_Acces_to_MZB(void);
uint8_t Tester_Acces_to_STER(void);
uint8_t Tester_Acces_to_PKSPP(void);

/*
 * Sprawdzanie czy Telcor wystawia CS
 */
uint8_t Tester_Check_CS1n(void);
uint8_t Tester_Check_CS2n(void);
uint8_t Tester_Check_CS3n(void);
uint8_t Tester_Check_CS4n(void);
uint8_t Tester_Check_CS5n(void);

/*
 * Test Hardware
 */
uint8_t Hardware_ALL();
uint8_t Hardware_TEST_PORTA();
uint8_t Hardware_TEST_PORTB();
uint8_t Hardware_TEST_RDY();
uint8_t Hardware_TEST_POMIAR();
uint8_t Hardware_TEST_PWR();
uint8_t Hardware_TEST_ZER();
uint8_t Hardware_TEST_RESTART();

void Hardware_Modbus_Send_GPIO_1_SET_RDY();
void Hardware_Modbus_Send_GPIO_1_CLEAR();
void Hardware_Modbus_Send_GPIO_1_SET_POMIAR();
void Hardware_Modbus_Send_GPIO_2_SET_PWR();
void Hardware_Modbus_Send_GPIO_2_CLEAR();
void Hardware_Modbus_Send_GPIO_2_SET_ZER();
void Hardware_Modbus_Send_GPIO_5_LED_SET();
void Hardware_Modbus_Send_GPIO_5_LED_CLEAR();
void Hardware_Modbus_Send_GPIO_10_SET_CS();
void Hardware_Modbus_Send_GPIO_10_CLEAR();

