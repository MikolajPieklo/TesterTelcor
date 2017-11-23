#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "em_device.h"
#include "em_assert.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "bsp.h"
#include "segmentlcd.h"
#include "bsp_trace.h"

#include "em_usb.h"
#include "msdd.h"
#include "msddmedia.h"
#include "vud.h"
#include "cdc.h"
#include "descriptors.h"

#include "efm32gg990f1024.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_timer.h"
#include "em_system.h"
#include "em_i2c.h"
#include "em_adc.h"
#include "em_rmu.h"
#include "em_dbg.h"
#include "em_usart.h"
#include "gpiointerrupt.h"

#include "tester.h"
#include "zadania.h"
#include "MCP23017.h"
#include "STC3105.h"
#include "ADS1247.h"
#include "ADC_EFM32.h"
#include "RS485.h"
#include "myUSB.h"
#include "Modbus.h"
//#include "eeprom.h"


ADC_profile profile2;
void PrintfUSBfloat2char(float value, uint8_t typ_pomiaru, uint8_t PIN);
void PP_Read_Voltage(uint8_t PIN);
void Main_Zadanie(void);
void _exit(int code) ///New lib SysCall for GCC
{
	while (1)
		;
}

volatile uint32_t msTicks;
void Delay(uint32_t dlyTicks);
void SysTick_Handler(void) {
	msTicks++; /* increment counter necessary in Delay()*/
}

void Delay(uint32_t dlyTicks) {
	while (msTicks < dlyTicks)
		;
	msTicks = 0;
}

void I2C1_INIT() {
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_I2C1, true);

	GPIO->P[2].MODEL = (11 << 20) | (11 << 16) | (4 << 12); // Configure PC4 and PC5 as open drain output, PC3 as push-pull output
	GPIO->P[2].DOUTSET = (1 << 5) | (1 << 4) | (1 << 3); // Initialize PC3, PC4, and PC5 high

	//I2C1->CTRL = (1<<8)|(1 << 2) | (1 << 0);           // auto acknowledge, enable I2C module
	//I2C1->CMD = (1 << 7) | (1 << 6) | (1 << 5); // Clear pending commands, clear TX buffer and shift reg, issue abort command to clear bus busy bit
	//I2C1->CLKDIV = 29;                          // allows for 100kHz I2C clock
	I2C1->ROUTE = 0x3;               // use location #0, enable SDA and SCK pins

	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
	I2C_Init(I2C1, &i2cInit);
	//I2C_TransferSeq_TypeDef i2cTransfert;
	// I2C_TransferReturn_TypeDef ret;
}

void SPI_INIT() {
	// usart1
	// CMU_ClockSelectSet(cmuClock_HFPER,cmuSelect_LFXO);	  /* Enable LE clock for CPU access to BURTC registers */

	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	//  CMU->CTRL |= (1 << 14);                             // Set HF clock divider to /1 to keep core frequency <32MHz
	//  CMU->OSCENCMD |= 0x4;                               // Enable XTAL Oscillator
	//  while(! (CMU->STATUS & 0x8) );                      // Wait for XTAL osc to stabilize
	//  CMU->CMD = 0x2;                                     // Select HF XTAL osc as system clock source. 48MHz XTAL, but we divided the system clock by 2, therefore our HF clock should be 24MHz
	CMU_ClockEnable(cmuClock_GPIO, true);
	CMU_ClockEnable(cmuClock_USART1, true);
	GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 0);

	USART_InitSync_TypeDef spiInit =	 //= USART_INITSYNC_DEFAULT;
			{ .enable = usartEnable, .refFreq = 0, .baudrate = 2000000,
					.databits = usartDatabits8, .master = true, .msbf = true,
					.clockMode = usartClockMode1,  //CPOL=0 CPHA =1
					.prsRxEnable = false, /* Enable USART Rx via Peripheral Reflex System */
					.prsRxCh = usartPrsRxCh0, /* Select PRS channel if enabled */
					.autoTx = false, };
	USART_Reset(USART1);
	USART_InitSync(USART1, &spiInit);
	USART1->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN
			| USART_ROUTE_LOCATION_LOC1;

	USART0->CTRL |= _USART_CTRL_TXDELAY_SINGLE;
	USART_Enable(USART1, usartEnable);
}

void gpioCallback() {
	GPIO->IFC = 1 << 11;
	PP_StopMeasure(2);
	PP_ADC_RDY = 0;  // rdy low
}

int SetupCmd(const USB_Setup_TypeDef *setup);
void StateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState);


static const USBD_Callbacks_TypeDef callbacks =
{
		.usbReset = NULL,
		.usbStateChange = StateChangeEvent,
		.setupCmd = SetupCmd,
		.isSelfPowered = NULL,
		.sofInt = NULL
};

const USBD_Init_TypeDef usbInitStruct =
{
		.deviceDescriptor  = &USBDESC_deviceDesc,
		.configDescriptor  = USBDESC_configDesc,
		.stringDescriptors = USBDESC_strings,
		.numberOfStrings   = sizeof(USBDESC_strings) / sizeof(void*),
		.callbacks         = &callbacks,
		.bufferingMultiplier =	USBDESC_bufferingMultiplier,
		.reserved = 0
};

/* Messages to send when the user presses buttons on the kit */
//EFM32_ALIGN(4)
uint8_t button0message[] = "PB0 pressed!";
//EFM32_ALIGN(4)
uint8_t button1message[] = "PB1 pressed!";
uint8_t MODBUS_CS1_ok[] = "CS1_OK";
uint8_t MODBUS_CS2_ok[] = "CS2_OK";
uint8_t MODBUS_CS3_ok[] = "CS3_OK";
uint8_t MODBUS_CS4_ok[] = "CS4_OK";
uint8_t MODBUS_CS5_ok[] = "CS5_OK";
uint8_t MODBUS_Blad[] = "Brak";

uint8_t bufferodbior[15];

void gpioInit(void) {
	/* Enable clock to GPIO */
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Enable interrupt on push button 0 */
	/*GPIO_PinModeSet(gpioPortB, 9, gpioModeInput, 0);
	 GPIO_IntConfig(gpioPortB, 9, false, true, true);
	 NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	 NVIC_EnableIRQ(GPIO_ODD_IRQn);*/

	/* Enable interrupt on push button 1 */
	/*GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0);
	 GPIO_IntConfig(gpioPortB, 10, false, true, true);
	 NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	 NVIC_EnableIRQ(GPIO_EVEN_IRQn);*/

	GPIOINT_Init();

	/*NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
	 NVIC_EnableIRQ(GPIO_ODD_IRQn);
	 NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	 NVIC_EnableIRQ(GPIO_EVEN_IRQn);*/

	GPIO_PinModeSet(gpioPortB, 11, gpioModeInput, 0);
	GPIOINT_CallbackRegister(11, &gpioCallback);
	GPIO_IntConfig(gpioPortB, 11, true, false, true); //set rising edge
	GPIO_IntEnable(11);

}
static int myCallback(USB_Status_TypeDef status, uint32_t xferred,uint32_t remaining) {
	(void) remaining;

	if (status == USB_STATUS_OK) {
		/* Process data */
	}
	if (USBD_GetUsbState() == USBD_STATE_CONFIGURED) {
		USBD_Read(0x01, Modbus_Buffor_Tab, sizeof(Modbus_Buffor_Tab), myCallback);

		if(Modbus_Buffor_Tab[0]==Modbus_Address_Device)//Jezeli ramka do tego urzadzenia
		{
			if(Modbus_Buffor_Tab[1]==Modbus_Function_Write){				//zapis
				uint16_t lCRC16=CRC16(Modbus_Buffor_Tab,9);
				if(lCRC16==(Modbus_Buffor_Tab[9]<<8)|Modbus_Buffor_Tab[10]){

					switch(Modbus_Buffor_Tab[2]+Modbus_Buffor_Tab[3]){
						case 0x0010:
							Tester.Zadanie.PK1=1;
							Tester.Przekazniki.PK1_value = Modbus_Buffor_Tab[7];
						break;
						case 0x0011:
							Tester.Zadanie.PK2 = 1;
							Tester.Przekazniki.PK2_value = Modbus_Buffor_Tab[7];
						break;
					}//end_switch
				lCRC16=CRC16(Modbus_Buffor_Tab,6);
				Modbus_Buffor_Tab[6]=(lCRC16>>8);
				Modbus_Buffor_Tab[7]=(lCRC16&0xFF);
				USBD_Write(0x81,Modbus_Buffor_Tab,8,dataSentCallback);
				}//END_CRC16
			}//END_WRITE
			if(Modbus_Buffor_Tab[1]==Modbus_Function_Read){					//odczyt
				uint16_t lCRC16=CRC16(Modbus_Buffor_Tab,6);
				if(lCRC16==(Modbus_Buffor_Tab[6]<<8)|Modbus_Buffor_Tab[7])
				{
					switch(Modbus_Buffor_Tab[2]+Modbus_Buffor_Tab[3]){
					case 0x0000: //zadanie status
						//Tester_Acces_to_ALL();
						Modbus_Buffor_Tab[2]=0x04;
						Modbus_Buffor_Tab[3]=Tester.Plytki.JEST_PKSPP;
						Modbus_Buffor_Tab[4]=Tester.Plytki.JEST_MZB;
						Modbus_Buffor_Tab[5]=Tester.Plytki.JEST_STER;
						Modbus_Buffor_Tab[6]=0;
						lCRC16=CRC16(Modbus_Buffor_Tab,7);
						Modbus_Buffor_Tab[7]=(lCRC16>>8);
						Modbus_Buffor_Tab[8]=(lCRC16&0xFF);
						USBD_Write(0x81,Modbus_Buffor_Tab,9,dataSentCallback);
						break;
					case 0x0003:
						Modbus_Buffor_Tab[2]=0x06;
						Modbus_Buffor_Tab[3]='A';
						Modbus_Buffor_Tab[4]='T';
						Modbus_Buffor_Tab[5]='R';
						Modbus_Buffor_Tab[6]='E';
						Modbus_Buffor_Tab[7]='M';
						Modbus_Buffor_Tab[8]=0x00;
						lCRC16=CRC16(Modbus_Buffor_Tab,9);
						Modbus_Buffor_Tab[9]=(lCRC16>>8);
						Modbus_Buffor_Tab[10]=(lCRC16&0xFF);
						USBD_Write(0x81,Modbus_Buffor_Tab,11,dataSentCallback);
					break;
					case 0x0004:
						Modbus_Buffor_Tab[2]=0x06;
						Modbus_Buffor_Tab[3]='T';
						Modbus_Buffor_Tab[4]='E';
						Modbus_Buffor_Tab[5]='L';
						Modbus_Buffor_Tab[6]='K';
						Modbus_Buffor_Tab[7]='O';
						Modbus_Buffor_Tab[8]='R';
						lCRC16=CRC16(Modbus_Buffor_Tab,9);
						Modbus_Buffor_Tab[9]=(lCRC16>>8);
						Modbus_Buffor_Tab[10]=(lCRC16&0xFF);
						USBD_Write(0x81,Modbus_Buffor_Tab,11,dataSentCallback);
					break;

					case 0x0012://Zadanie Zasilanie
						Tester.Zadanie.VZAS1=Ogloszone;
					break;

					case 0x0014:
						Tester.Zadanie.VZAS2=Ogloszone;
						break;
					case 0x0016:
						Tester.Zadanie.VBAT =Ogloszone;
						break;
					case 0x0018:
						Tester.Zadanie.I_TC =Ogloszone;
						break;

					}//end_switch
				}//end_CRC16
			}//Modbus_Read_END
		}//Modbus_Address_END

		/*switch (bufferodbior[0] + bufferodbior[1]) {
		case 160: //zadanie PP_X{s1s2s3s4s5s6s7}1 Pomiar napiec na PP
			if (bufferodbior[3] == 'V') {
				if (bufferodbior[5] == '1')	flaga_zadan.PP_Vol |= 1;
				if (bufferodbior[6] == '1')	flaga_zadan.PP_Vol |= 2;
				if (bufferodbior[7] == '1')	flaga_zadan.PP_Vol |= 4;
				if (bufferodbior[8] == '1')	flaga_zadan.PP_Vol |= 8;
				if (bufferodbior[9] == '1')	flaga_zadan.PP_Vol |= 16;
				if (bufferodbior[10] == '1')flaga_zadan.PP_Vol |= 32;
				if (bufferodbior[11] == '1')flaga_zadan.PP_Vol |= 64;
			}
			if (bufferodbior[3] == 'E') { //Sprawdz eprom
				if (bufferodbior[5] == '1')	flaga_zadan.PP_Epro |= 1;
				if (bufferodbior[6] == '1')	flaga_zadan.PP_Epro |= 2;
				if (bufferodbior[7] == '1')	flaga_zadan.PP_Epro |= 4;
				if (bufferodbior[8] == '1')	flaga_zadan.PP_Epro |= 8;
				if (bufferodbior[9] == '1')	flaga_zadan.PP_Epro |= 16;
				if (bufferodbior[10] == '1')flaga_zadan.PP_Epro |= 32;
				if (bufferodbior[11] == '1')flaga_zadan.PP_Epro |= 64;
			}
			break;

		case 132: //Zadanie BB_B{s1s2s3s4s5s6s7}1
			if (bufferodbior[3] == 'B') {
				if (bufferodbior[5] == '1')flaga_zadan.BB_B |= 1;
				if (bufferodbior[6] == '1')flaga_zadan.BB_B |= 2;
				if (bufferodbior[7] == '1')flaga_zadan.BB_B |= 4;
				if (bufferodbior[8] == '1')flaga_zadan.BB_B |= 8;
				if (bufferodbior[9] == '1')flaga_zadan.BB_B |= 16;
			}
			break;
		case 0x9A: //Zadanie Modbus
			if (bufferodbior[3] == 'R') {
				if (bufferodbior[4] == '1' && bufferodbior[5] == '1')flaga_zadan.MM_R |= 1;
				if (bufferodbior[4] == '1' && bufferodbior[5] == '2')flaga_zadan.MM_R |= 2;
				if (bufferodbior[4] == '1' && bufferodbior[5] == '3')flaga_zadan.MM_R |= 4;
			}
			if (bufferodbior[3] == 'W') {
				if (bufferodbior[4] == '1' && bufferodbior[5] == '1')flaga_zadan.MM_W |= 1;
			}
			break;
		case 0xA8: //Zadanie Check Hardware
			if (bufferodbior[3] == 'G') {
				if (bufferodbior[4] == '0' && bufferodbior[5] == '0')flaga_zadan.TT_G |= 0x01;
				if (bufferodbior[4] == '0' && bufferodbior[5] == '1')flaga_zadan.TT_G |= 0x02;
				if (bufferodbior[4] == '0' && bufferodbior[5] == '2')flaga_zadan.TT_G |= 0x04;
				if (bufferodbior[4] == '0' && bufferodbior[5] == '3')flaga_zadan.TT_G |= 0x08;
				if (bufferodbior[4] == '0' && bufferodbior[5] == '4')flaga_zadan.TT_G |= 0x10;
				if (bufferodbior[4] == '0' && bufferodbior[5] == '5')flaga_zadan.TT_G |= 0x20;
				if (bufferodbior[4] == '0' && bufferodbior[5] == '6')flaga_zadan.TT_G |= 0x40;
				if (bufferodbior[4] == '0' && bufferodbior[5] == '7')flaga_zadan.TT_G |= 0x80;
			}
		}*/
		//USBD_Write(0x81,Modbus_Buffor_Tab,sizeof(Modbus_Buffor_Tab),dataSentCallback);
		uint8_t i=0;
		for(i=0;i<sizeof(Modbus_Buffor_Tab);i++)
		{
			Modbus_Buffor_Tab[i]=0;
		}
	}
	return status;
}

int main(void) {

	CHIP_Init();
	/* If first word of user data page is non-zero, enable eA Profiler trace */
	BSP_TraceProfilerSetup();

	//if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;

	CMU_OscillatorEnable(cmuOsc_HFXO, true, true); // Enable the External Oscillator and Wati till it stabilizes
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO); // Set the HFXO as the Clock Source
	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);  //Disable the HFRCO
	CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
	CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_4); // Prescale the HFPERCLK -> HF/4 = 32/4 = 8Mhz
	CMU_ClockEnable(cmuClock_GPIO, true);        // Enable clock for GPIO module
	//CMU_ClockEnable(cmuClock_TIMER2, true);
	//GPIO_PinModeSet(gpioPortE,2,gpioModePushPull,0);    // Init the LEDs

	/* Initialize LCD driver */
	SegmentLCD_Init(false);
	SegmentLCD_Write("TestTelcor");
	SegmentLCD_Symbol(LCD_SYMBOL_GECKO, true);

	/* Initialize LED driver */
	BSP_LedsInit();
	//gpioInit();

	/* Initialize the Mass Storage Media. */
	if (!MSDDMEDIA_Init()) {
		EFM_ASSERT(false);
		for (;;) {
		}
	}

	VUD_Init(); /* Initialize the vendor unique device. */
	CDC_Init(); /* Initialize the communication class device. */
	MSDD_Init(-1, 0);             /* Initialize the Mass Storage Device.  */

	/* Initialize and start USB device stack. */
	USBD_Init(&usbInitStruct);

	I2C1_INIT();
	SPI_INIT();
	RS485_Init();
	PP_ADC_RDY = 1;

	Init_Tester();
	SEL_SPI1_PKSPP_ON();
	//SEL_SPI1_TC_ON();
	//SEL_SPI_SL_ON();

	Init_ADC_EFM();
	ADC_Conversation(ZAL_POM_VZAS2, ZAL_POM_VZAS2_VBAT);
	Tester_LED_BLUE();

	/*
	 * When using a debugger it is practical to uncomment the following three
	 * lines to force host to re-enumerate the device.
	 */
	/* USBD_Disconnect();         */
	/* USBTIMER_DelayMs( 1000 );  */
	/* USBD_Connect();            */


	for (;;) {
		Tester_LED_RED();
		USBTIMER_DelayMs(100);
		Tester_LED_GREEN();
		USBTIMER_DelayMs(10);
		Tester_LED_BLUE();
		USBTIMER_DelayMs(10);
		/*Tester_Relay_PK1_ON();
		USBTIMER_DelayMs(500);
		Tester_Relay_PK1_OFF();
		USBTIMER_DelayMs(500);
		Tester_Relay_PK2_ON();
		USBTIMER_DelayMs(500);
		Tester_Relay_PK2_OFF();
		USBTIMER_DelayMs(500);*/

		/*RS485_MM_R11();
		 USBTIMER_DelayMs(10);*/
		/*Tester_Relay_PK1_ON();
		USBTIMER_DelayMs(500);
		Tester_Relay_PK1_OFF();
		 //Tester_LED_GREEN();

		 USBTIMER_DelayMs(1000);
		 Tester_Relay_PK2_ON();
		// Tester_LED_RED();
		 USBTIMER_DelayMs(500);
		 Tester_Relay_PK2_OFF();
		 USBTIMER_DelayMs(1000);*/
		//Tester_Acces_to_STER();
		Tester_Acces_to_ALL();
		//EEPROM_Read(I2C1,Address_MCP23017_U30,MCP23017_GPIOA,&testread,1);
		Main_Zadanie();
		MSDD_Handler(); /* Serve the MSD device. */
	}
}

/***************************************************************************//**
 * @brief
 *   Called whenever a USB setup command is received.
 *
 * @param[in] setup
 *   Pointer to an USB setup packet.
 *
 * @return
 *   An appropriate status/error code. See USB_Status_TypeDef.
 ******************************************************************************/
int SetupCmd(const USB_Setup_TypeDef *setup) {
	int retVal;

	/* Call SetupCmd handlers for all functions within the composite device. */

	retVal = MSDD_SetupCmd(setup);

	if (retVal == USB_STATUS_REQ_UNHANDLED) {
		retVal = VUD_SetupCmd(setup);
	}

	if (retVal == USB_STATUS_REQ_UNHANDLED) {
		retVal = CDC_SetupCmd(setup);
	}

	return retVal;
}

/***************************************************************************//**
 * @brief
 *   Called whenever the USB device has changed its device state.
 *
 * @param[in] oldState
 *   The device USB state just leaved. See USBD_State_TypeDef.
 *
 * @param[in] newState
 *   New (the current) USB device state. See USBD_State_TypeDef.
 ******************************************************************************/
void StateChangeEvent(USBD_State_TypeDef oldState, USBD_State_TypeDef newState) {
	if(newState == USBD_STATE_CONFIGURED)
	{
		USBD_Read(0x01,Modbus_Buffor_Tab,20,myCallback);
	}
	MSDD_StateChangeEvent(oldState, newState);
	CDC_StateChangeEvent(oldState, newState);
}
void PrintfUSBfloat2char(float value, uint8_t typ_pomiaru, uint8_t PIN) {
	uint8_t Results[9];
	uint8_t index = 0;
	float int_part;
	float frc_part;
	uint8_t charPIN[1];
	switch (PIN) {
	case 2:
		charPIN[0] = '1';
		break;
	case 4:
		charPIN[0] = '2';
		break;
	case 8:
		charPIN[0] = '3';
		break;
	case 16:
		charPIN[0] = '4';
		break;
	case 32:
		charPIN[0] = '5';
		break;
	case 64:
		charPIN[0] = '6';
		break;
	case 128:
		charPIN[0] = '7';
		break;
	}

	if (typ_pomiaru == 1 && value != 0) {
		frc_part = modf(value, &int_part);
		for (index = 0; index < value; index++)
			;
		index -= 1;
		Results[0] = 'V';
		Results[1] = 'D';
		Results[2] = 'D';
		Results[3] = charPIN[0];
		Results[4] = (uint8_t) (48 + index);
		Results[5] = 46; //kropka
		Results[6] = ((uint8_t) 10 * frc_part) + 48;
		Results[7] =
				(uint8_t) ((100 * frc_part) - (10 * (Results[6] - 48)) + 48);
		Results[8] = ' ';
		USBD_Write(0x81, Results, sizeof(Results), dataSentCallback);
	}
	if (typ_pomiaru == 2 && value != 0) {
		for (index = 0; index < value; index++)
			;
		index -= 1;
		frc_part = modf(value, &int_part);
		Results[0] = 'T';
		Results[1] = 'E';
		Results[2] = 'M';
		Results[3] = charPIN[0];
		Results[4] = (uint8_t) (((index - (index % 10)) / 10) + 48);
		Results[5] = (index % 10) + 48;
		Results[6] = 46; //kropka
		Results[7] = ((uint8_t) 10 * frc_part) + 48;
		Results[8] = ' ';
		USBD_Write(0x81, Results, sizeof(Results), dataSentCallback);
	}

	if (typ_pomiaru == 3 && value != 0) {
		frc_part = modf(value, &int_part);
		for (index = 0; index < value; index++)
			;
		index -= 1;

		Results[0] = 'V';
		Results[1] = 'R';
		Results[2] = 'E';
		Results[3] = charPIN[0];
		Results[4] = (uint8_t) (48 + index);
		Results[5] = 46; //kropka
		Results[6] = ((uint8_t) 10 * frc_part) + 48;
		Results[7] =
				(uint8_t) ((100 * frc_part) - (10 * (Results[6] - 48)) + 48);
		Results[8] = ' ';
		USBD_Write(0x81, Results, sizeof(Results), dataSentCallback);

	}

	if (typ_pomiaru == 4 && value != 0) {
		frc_part = modf(value, &int_part);
		for (index = 0; index < value; index++)
			;
		index -= 1;

		Results[0] = 'V';
		Results[1] = 'A';
		Results[2] = 'V';
		Results[3] = charPIN[0];
		Results[4] = (uint8_t) (48 + index);
		Results[5] = 46; //kropka
		Results[6] = ((uint8_t) 10 * frc_part) + 48;
		Results[7] =
				(uint8_t) ((100 * frc_part) - (10 * (Results[6] - 48)) + 48);
		Results[8] = ' ';
		USBD_Write(0x81, Results, sizeof(Results), dataSentCallback);
	}
}

void PP_Read_Voltage(uint8_t PIN) {
	float testvdd = 0;
	float testtemp = 0;
	float testvref = 0;
	float testvav = 0;

	PP_PWR_ON(PIN, 1);

	profile2.MUX0 = 0x00;
	profile2.MUX1 = 0x23;
	profile2.SYS0 = 8;
	profile2.VBIAS = 0;
	profile2.IDAC1 = 204;
	PP_WriteADCConfig(PIN, profile2);

	profile2 = PP_ReadADCConfig(PIN, profile2);
	USBTIMER_DelayMs(10);
	testvdd = PP_ReadVdd(PIN);
	USBTIMER_DelayMs(10);
	testtemp = PP_ReadTemp(PIN);
	USBTIMER_DelayMs(10);
	testvref = PP_ReadVref(PIN);
	USBTIMER_DelayMs(10);
	testvav = PP_ReadVav(PIN);

	USBTIMER_DelayMs(10);
	PrintfUSBfloat2char(testvdd, Printf_VDD, PIN);
	USBTIMER_DelayMs(10);
	PrintfUSBfloat2char(testtemp, Printf_Temp, PIN);
	USBTIMER_DelayMs(10);
	PrintfUSBfloat2char(testvref, Printf_VREF, PIN);
	USBTIMER_DelayMs(10);
	PrintfUSBfloat2char(testvav, Printf_VAV, PIN);
	USBTIMER_DelayMs(10);
}

void Main_Zadanie() {

	uint8_t *p;
	uint16_t lCRC16;

	uint8_t val_zadanie[8];
	uint8_t val_zadanie_eprom[5];
	uint8_t test[3];
	uint16_t voltage;


	if(Tester.Zadanie.PK1 == Ogloszone){
		Tester.Zadanie.PK1=Wykonywane;
		if(Tester.Przekazniki.PK1_value==0x00){Tester_Relay_PK1_OFF();Tester.Przekazniki.PK1=0;}
		if(Tester.Przekazniki.PK1_value==0xFF){Tester_Relay_PK1_ON(); Tester.Przekazniki.PK1=1;}
		Tester.Zadanie.PK1=Nieogloszone;
	}//END_Tester.Zadanie.PK1

	if(Tester.Zadanie.PK2 == Ogloszone){
		Tester.Zadanie.PK2=Wykonywane;
		if(Tester.Przekazniki.PK2_value==0x00){Tester_Relay_PK2_OFF();Tester.Przekazniki.PK2=0;}
		if(Tester.Przekazniki.PK2_value==0xFF){Tester_Relay_PK2_ON(); Tester.Przekazniki.PK2=1;}
		Tester.Zadanie.PK2=Nieogloszone;
	}//END_Tester.Zadanie.PK2

	if(Tester.Zadanie.VZAS1 == Ogloszone){
		Tester.Zadanie.VZAS1=Wykonywane;

		Tester.Pomiary.VZAS1 = ADC_Conversation(ZAL_POM_VZAS1, ZAL_POM_VZAS_I);
		Tester.Pomiary.VZAS1 = 4 * Tester.Pomiary.VZAS1;
		p=&Tester.Pomiary.VZAS1;
		Modbus_Buffor_Tab[0]=0x10;
		Modbus_Buffor_Tab[1]=0x03;
		Modbus_Buffor_Tab[2]=0x04;
		Modbus_Buffor_Tab[3]=*(p);
		Modbus_Buffor_Tab[4]=*(p+1);
		Modbus_Buffor_Tab[5]=*(p+2);
		Modbus_Buffor_Tab[6]=*(p+3);
		lCRC16=CRC16(Modbus_Buffor_Tab,7);
		Modbus_Buffor_Tab[7]=(lCRC16>>8);
		Modbus_Buffor_Tab[8]=(lCRC16&0xFF);
		USBD_Write(0x81,Modbus_Buffor_Tab,9,dataSentCallback);
		Tester.Zadanie.VZAS1=Nieogloszone;
	}//END_Tester.Zadanie.VZAS1

	if(Tester.Zadanie.VZAS2 == Ogloszone){
		Tester.Zadanie.VZAS2=Wykonywane;

		Tester.Pomiary.VZAS2= ADC_Conversation(ZAL_POM_VZAS2_VBAT, ZAL_POM_VZAS2);
		Tester.Pomiary.VZAS2 = 4 * Tester.Pomiary.VZAS2;
		p=&Tester.Pomiary.VZAS2;
		Modbus_Buffor_Tab[0]=0x10;
		Modbus_Buffor_Tab[1]=0x03;
		Modbus_Buffor_Tab[2]=0x04;
		Modbus_Buffor_Tab[3]=*(p);
		Modbus_Buffor_Tab[4]=*(p+1);
		Modbus_Buffor_Tab[5]=*(p+2);
		Modbus_Buffor_Tab[6]=*(p+3);
		lCRC16=CRC16(Modbus_Buffor_Tab,7);
		Modbus_Buffor_Tab[7]=(lCRC16>>8);
		Modbus_Buffor_Tab[8]=(lCRC16&0xFF);
		USBD_Write(0x81,Modbus_Buffor_Tab,9,dataSentCallback);

		Tester.Zadanie.VZAS2=Nieogloszone;
	}//END_Tester.Zadanie.VZAS2

	if(Tester.Zadanie.VBAT == Ogloszone){
		Tester.Zadanie.VBAT=Wykonywane;

		if(Tester.Plytki.JEST_MZB == Podlaczono){
			Tester.Pomiary.VBAT = ADC_Conversation(ZAL_POM_VBAT, ZAL_POM_VZAS2_VBAT);//Modul_fizyczny
			Tester.Pomiary.VBAT = 4 * Tester.Pomiary.VBAT;
		}
		p=&Tester.Pomiary.VBAT;
		Modbus_Buffor_Tab[0]=0x10;
		Modbus_Buffor_Tab[1]=0x03;
		Modbus_Buffor_Tab[2]=0x04;
		Modbus_Buffor_Tab[3]=*(p);
		Modbus_Buffor_Tab[4]=*(p+1);
		Modbus_Buffor_Tab[5]=*(p+2);
		Modbus_Buffor_Tab[6]=*(p+3);
		lCRC16=CRC16(Modbus_Buffor_Tab,7);
		Modbus_Buffor_Tab[7]=(lCRC16>>8);
		Modbus_Buffor_Tab[8]=(lCRC16&0xFF);
		USBD_Write(0x81,Modbus_Buffor_Tab,9,dataSentCallback);

		Tester.Zadanie.VBAT=Nieogloszone;
	}//END_Tester.Zadanie.VBAT

	if(Tester.Zadanie.I_TC == Ogloszone){
		Tester.Zadanie.I_TC=Wykonywane;
		Tester_Relay_PK1_ON();
		//delay
		USBTIMER_DelayMs( 500 );
		Tester.Pomiary.I_TC = ADC_Conversation(ZAL_POM_I_TC, ZAL_POM_VZAS_I);
		Tester.Pomiary.I_TC = Tester.Pomiary.I_TC - 0.7015;
		p=&Tester.Pomiary.I_TC;
		Modbus_Buffor_Tab[0]=0x10;
		Modbus_Buffor_Tab[1]=0x03;
		Modbus_Buffor_Tab[2]=0x04;
		Modbus_Buffor_Tab[3]=*(p);
		Modbus_Buffor_Tab[4]=*(p+1);
		Modbus_Buffor_Tab[5]=*(p+2);
		Modbus_Buffor_Tab[6]=*(p+3);
		lCRC16=CRC16(Modbus_Buffor_Tab,7);
		Modbus_Buffor_Tab[7]=(lCRC16>>8);
		Modbus_Buffor_Tab[8]=(lCRC16&0xFF);
		USBD_Write(0x81,Modbus_Buffor_Tab,9,dataSentCallback);
		Tester_Relay_PK1_OFF();
		Tester.Zadanie.I_TC=Nieogloszone;
	}//END_Tester.Zadanie.I_TC





	//---------
	while (flaga_zadan.PO_Vol != 0) {
			float result = 0;
			float frc_part = 0;
			float int_part = 0;
			uint8_t index = 0;

			if ((flaga_zadan.PO_Vol & 0x01) == 1) { //Nap bat symulacyjnej podlaczone do TelCor 3...3,7
				if (Tester_Acces_to_STER()) {

					frc_part = modf(result, &int_part);
					for (index = 0; index < result; index++)
						;
					index -= 1;
					val_zadanie[3] = (uint8_t) (48 + index);
					val_zadanie[4] = 46; //kropka
					val_zadanie[5] = ((uint8_t) 10 * frc_part) + 48;
					val_zadanie[6] = (uint8_t) ((100 * frc_part)
							- (10 * (val_zadanie[5] - 48)) + 48);
					val_zadanie[7] = (uint8_t) ((1000 * frc_part)
							- (100 * (val_zadanie[5] - 48))
							- (10 * (val_zadanie[6] - 48))) + 48;
					USBD_Write(0x81, val_zadanie, sizeof(val_zadanie),
							dataSentCallback);
				}
			}

			if ((flaga_zadan.PO_Vol & 0x02) == 2) { //Nap bat fizycznej podl. do testera 3...3,7

					frc_part = modf(result, &int_part);
					for (index = 0; index < result; index++)
						;
					index -= 1;

					val_zadanie[3] = (uint8_t) (48 + index);
					val_zadanie[4] = 46; //kropka
					val_zadanie[5] = ((uint8_t) 10 * frc_part) + 48;
					val_zadanie[6] = (uint8_t) ((100 * frc_part)
							- (10 * (val_zadanie[5] - 48)) + 48);
					val_zadanie[7] = (uint8_t) ((1000 * frc_part)
							- (100 * (val_zadanie[5] - 48))
							- (10 * (val_zadanie[6] - 48))) + 48;


			}
			if ((flaga_zadan.PO_Vol & 0x04) == 4) //Pomiar pradu Telcor
					{

				frc_part = modf(result, &int_part);
				for (index = 0; index < result; index++)
					;
				index -= 1;
				val_zadanie[0] = 'I';
				val_zadanie[1] = 'Z';
				val_zadanie[2] = 'T';
				val_zadanie[3] = (uint8_t) (48 + index);
				val_zadanie[4] = 46; //kropka
				val_zadanie[5] = ((uint8_t) 10 * frc_part) + 48;
				val_zadanie[6] = (uint8_t) ((100 * frc_part)
						- (10 * (val_zadanie[5] - 48)) + 48);
				val_zadanie[7] = (uint8_t) ((1000 * frc_part)
						- (100 * (val_zadanie[5] - 48))
						- (10 * (val_zadanie[6] - 48))) + 48;

			}
			if ((flaga_zadan.PO_Vol & 0x08) == 8) { //Nap telcor 3.3V VZAS2
					frc_part = modf(result, &int_part);
					for (index = 0; index < result; index++)
						;
					index -= 1;

					val_zadanie[3] = (uint8_t) (48 + index);
					val_zadanie[4] = 46; //kropka
					val_zadanie[5] = ((uint8_t) 10 * frc_part) + 48;
					val_zadanie[6] = (uint8_t) ((100 * frc_part)
							- (10 * (val_zadanie[5] - 48)) + 48);
					val_zadanie[7] = (uint8_t) ((1000 * frc_part)
							- (100 * (val_zadanie[5] - 48))
							- (10 * (val_zadanie[6] - 48))) + 48;

			}
		}
	//---------




















	while (flaga_zadan.PP_Epro != 0) {
		val_zadanie_eprom[0] = 'E';
		val_zadanie_eprom[1] = 'P';
		val_zadanie_eprom[2] = 'R';
		//if(Tester_Acces_to_PKSPP()){
		if ((flaga_zadan.PP_Epro & 0x01) == 1) //Slot 1
				{
			flaga_zadan.PP_Epro &= 0x7E; //Clear flag
			val_zadanie_eprom[3] = '1';
			val_zadanie_eprom[4] = PP_CheckEEPROM(_GPIOA1S);
			USBD_Write(0x81, val_zadanie_eprom, sizeof(val_zadanie_eprom),
					dataSentCallback);
		}
		if ((flaga_zadan.PP_Epro & 0x02) == 2) //Slot 2
				{
			flaga_zadan.PP_Epro &= 0x7D; //Clear flag
			val_zadanie_eprom[3] = '2';
			val_zadanie_eprom[4] = PP_CheckEEPROM(_GPIOA2S);
			USBD_Write(0x81, val_zadanie_eprom, sizeof(val_zadanie_eprom),
					dataSentCallback);
		}
		if ((flaga_zadan.PP_Epro & 0x04) == 4) //Slot 3
				{
			flaga_zadan.PP_Epro &= 0x7B; //Clear flag
			val_zadanie_eprom[3] = '3';
			val_zadanie_eprom[4] = PP_CheckEEPROM(_GPIOA3S);
			USBD_Write(0x81, val_zadanie_eprom, sizeof(val_zadanie_eprom),
					dataSentCallback);
		}
		if ((flaga_zadan.PP_Epro & 0x08) == 8) //Slot 4
				{
			flaga_zadan.PP_Epro &= 0x77; //Clear flag
			val_zadanie_eprom[3] = '4';
			val_zadanie_eprom[4] = PP_CheckEEPROM(_GPIOA4S);
			USBD_Write(0x81, val_zadanie_eprom, sizeof(val_zadanie_eprom),
					dataSentCallback);
		}
		if ((flaga_zadan.PP_Epro & 0x10) == 16) //Slot 5
				{
			flaga_zadan.PP_Epro &= 0x6F; //Clear flag
			val_zadanie_eprom[3] = '5';
			val_zadanie_eprom[4] = PP_CheckEEPROM(_GPIOA5S);
			USBD_Write(0x81, val_zadanie_eprom, sizeof(val_zadanie_eprom),
					dataSentCallback);
		}
		if ((flaga_zadan.PP_Epro & 0x20) == 32) //Slot 6
				{
			flaga_zadan.PP_Epro &= 0x5F; //Clear flag
			val_zadanie_eprom[3] = '6';
			val_zadanie_eprom[4] = PP_CheckEEPROM(_GPIOA6S);
			USBD_Write(0x81, val_zadanie_eprom, sizeof(val_zadanie_eprom),
					dataSentCallback);
		}
		if ((flaga_zadan.PP_Epro & 0x40) == 64) //Slot 6
				{
			flaga_zadan.PP_Epro &= 0x3F; //Clear flag
			val_zadanie_eprom[3] = '7';
			val_zadanie_eprom[4] = PP_CheckEEPROM(_GPIOA7S);
			USBD_Write(0x81, val_zadanie_eprom, sizeof(val_zadanie_eprom),
					dataSentCallback);
		}
		/*}else{
		 flaga_zadan.PP_Epro &= 0x00;
		 val_zadanie_eprom[3]='B';	val_zadanie_eprom[4]='P'; //brak peryferium
		 }*/
	}
	while (flaga_zadan.PP_Vol != 0) {
		if ((flaga_zadan.PP_Vol & 0x01) == 1) //Slot 1
				{
			flaga_zadan.PP_Vol &= 0x7E; //Clear flag
			PP_Read_Voltage(_GPIOA1S);
		}
		if ((flaga_zadan.PP_Vol & 0x02) == 2) //Slot 2
				{
			flaga_zadan.PP_Vol &= 0x7D; //Clear flag
			PP_Read_Voltage(_GPIOA2S);
		}
		if ((flaga_zadan.PP_Vol & 0x04) == 4) //Slot 3
				{
			flaga_zadan.PP_Vol &= 0x7B; //Clear flag
			PP_Read_Voltage(_GPIOA3S);
		}
		if ((flaga_zadan.PP_Vol & 0x08) == 8) //Slot 4
				{
			flaga_zadan.PP_Vol &= 0x77; //Clear flag
			PP_Read_Voltage(_GPIOA4S);
		}
		if ((flaga_zadan.PP_Vol & 0x10) == 16) //Slot 5
				{
			flaga_zadan.PP_Vol &= 0x6F; //Clear flag
			PP_Read_Voltage(_GPIOA5S);
		}
		if ((flaga_zadan.PP_Vol & 0x20) == 32) //Slot 6
				{
			flaga_zadan.PP_Vol &= 0x5F; //Clear flag
			PP_Read_Voltage(_GPIOA6S);
		}
		if ((flaga_zadan.PP_Vol & 0x40) == 64) //Slot 6
				{
			flaga_zadan.PP_Vol &= 0x3F; //Clear flag
			PP_Read_Voltage(_GPIOA7S);
		}
	}


	while (flaga_zadan.BB_B != 0) {
		uint8_t result_out[7];
		if ((flaga_zadan.BB_B & 0x01) == 1) { //Napiecie
			flaga_zadan.BB_B &= 0x7E; //Clear flag
			if (Tester_Acces_to_MZB()) {
				uint16_t result = STC3105_GET_VOLTAGE();
				result_out[0] = 'B';
				result_out[1] = 'B';
				result_out[2] = 'V';
				result_out[6] = (result % 10) + '0';
				result /= 10;
				result_out[5] = (result % 10) + '0';
				result /= 10;
				result_out[4] = (result % 10) + '0';
				result /= 10;
				result_out[3] = (result % 10) + '0';
				USBD_Write(0x81, result_out, sizeof(result_out),
						dataSentCallback);
			} else {

				val_zadanie[0] = 'B';
				val_zadanie[1] = 'B';
				val_zadanie[2] = 'V';
				val_zadanie[3] = 'B';
				val_zadanie[4] = 'R';
				val_zadanie[5] = 'A';
				val_zadanie[6] = 'K';
				val_zadanie[7] = 'B';
				USBD_Write(0x81, val_zadanie, sizeof(val_zadanie),
						dataSentCallback);
			}
		}

		if ((flaga_zadan.BB_B & 0x02) == 2) { //Prad
			flaga_zadan.BB_B &= 0x7D; //Clear flag
			if (Tester_Acces_to_MZB()) {
				uint16_t result = STC3105_GET_CURRENT();
				result_out[0] = 'B';
				result_out[1] = 'B';
				result_out[2] = 'C';
				result_out[6] = (result % 10) + '0';
				result /= 10;
				result_out[5] = (result % 10) + '0';
				result /= 10;
				result_out[4] = (result % 10) + '0';
				result /= 10;
				result_out[3] = (result % 10) + '0';
				USBD_Write(0x81, result_out, sizeof(result_out),
						dataSentCallback);

			} else {
				val_zadanie[0] = 'B';
				val_zadanie[1] = 'B';
				val_zadanie[2] = 'C';
				val_zadanie[3] = 'B';
				val_zadanie[4] = 'R';
				val_zadanie[5] = 'A';
				val_zadanie[6] = 'K';
				val_zadanie[7] = 'B';
				USBD_Write(0x81, val_zadanie, sizeof(val_zadanie),
						dataSentCallback);
			}
		}

		if ((flaga_zadan.BB_B & 0x04) == 4) { //Counter
			flaga_zadan.BB_B &= 0x7B; //Clear flag
			if (Tester_Acces_to_MZB()) {
				uint16_t result = STC3105_GET_COUNTER();
				result_out[0] = 'B';
				result_out[1] = 'B';
				result_out[2] = 'O';
				result_out[7] = (result % 10) + '0';
				result /= 10;
				result_out[6] = (result % 10) + '0';
				result /= 10;
				result_out[5] = (result % 10) + '0';
				result /= 10;
				result_out[4] = (result % 10) + '0';
				result /= 10;
				result_out[3] = (result % 10) + '0';
				USBD_Write(0x81, result_out, sizeof(result_out),
						dataSentCallback);
			} else {
				val_zadanie[0] = 'B';
				val_zadanie[1] = 'B';
				val_zadanie[2] = 'O';
				val_zadanie[3] = 'B';
				val_zadanie[4] = 'R';
				val_zadanie[5] = 'A';
				val_zadanie[6] = 'K';
				val_zadanie[7] = 'B';
				USBD_Write(0x81, val_zadanie, sizeof(val_zadanie),
						dataSentCallback);
			}
		}

		if ((flaga_zadan.BB_B & 0x08) == 8) { //Culomb
			flaga_zadan.BB_B &= 0x77; //Clear flag
			if (Tester_Acces_to_MZB()) {
				uint16_t result = STC3105_GET_CHARGE();
				result_out[0] = 'B';
				result_out[1] = 'B';
				result_out[2] = 'U';
				result_out[6] = (result % 10) + '0';
				result /= 10;
				result_out[5] = (result % 10) + '0';
				result /= 10;
				result_out[4] = (result % 10) + '0';
				result /= 10;
				result_out[3] = (result % 10) + '0';
				USBD_Write(0x81, result_out, sizeof(result_out),
						dataSentCallback);
			} else {
				val_zadanie[0] = 'B';
				val_zadanie[1] = 'B';
				val_zadanie[2] = 'U';
				val_zadanie[3] = 'B';
				val_zadanie[4] = 'R';
				val_zadanie[5] = 'A';
				val_zadanie[6] = 'K';
				val_zadanie[7] = 'B';
				USBD_Write(0x81, val_zadanie, sizeof(val_zadanie),
						dataSentCallback);
			}
		}

		if ((flaga_zadan.BB_B & 0x10) == 16) { //Konfiguracja
			flaga_zadan.BB_B &= 0x6F; //Clear flag
			if (Tester_Acces_to_MZB()) {
				STC3105_Init_ConfigMode(REG_MODE, GG_RUN_OPERATING_MODE);
			}
		}

		if ((flaga_zadan.BB_B & 0x20) == 32) { //Restart
			flaga_zadan.BB_B &= 0x5F; //Clear flag
			if (Tester_Acces_to_MZB()) {
				STC3105_Init_ConfigMode(REG_CTRL,
						PORDET_RELEASE_ASSERT_RESTART | GG_RST_ON
								| IO_DATA_ALM_ON);
			}
		}

		if ((flaga_zadan.BB_B & 0x40) == 64) { //Slot 6
			flaga_zadan.BB_B &= 0x3F; //Clear flag
		}
	}

	while (flaga_zadan.MM_R != 0) {
		if ((flaga_zadan.MM_R & 0x01) == 1) //Producent
				{
			flaga_zadan.MM_R &= 0xFE; //Clear flag
			if (Tester_Acces_to_STER()) { //Plytka wlozona
				RS485_MM_R11();
				USBD_Write(0x81, RxBuffor.Buffor, sizeof(RxBuffor.Buffor),
						dataSentCallback);
			} else {
				//RxBuffor.Buffor='B';RxBuffor.Buffor='R';RxBuffor.Buffor='A';RxBuffor.Buffor='K';RxBuffor.Buffor='S';
				USBD_Write(0x81, RxBuffor.Buffor, sizeof(RxBuffor.Buffor),
						dataSentCallback);
			}
		}
		if ((flaga_zadan.MM_R & 0x02) == 2) //Urzadzenie
				{
			flaga_zadan.MM_R &= 0xFD; //Clear flag
			if (Tester_Acces_to_STER()) { //Plytka wlozona
				RS485_MM_R12();
				USBD_Write(0x81, RxBuffor.Buffor, sizeof(RxBuffor.Buffor),
						dataSentCallback);
			} else { //Brak plytki
					 //RxBuffor.Buffor='B';RxBuffor.Buffor='R';RxBuffor.Buffor='A';RxBuffor.Buffor='K';RxBuffor.Buffor='S';
				USBD_Write(0x81, RxBuffor.Buffor, sizeof(RxBuffor.Buffor),
						dataSentCallback);
			}
		}
		if ((flaga_zadan.MM_R & 0x04) == 4) {					//Sprawdzanie CS
			flaga_zadan.MM_R &= 0xFB; //Clear flag
			RS485_MM_K03();

		}
	}
	while (flaga_zadan.MM_W != 0) {
		if ((flaga_zadan.MM_W & 0x01) == 1) {
			flaga_zadan.MM_W &= 0xFE; //Clear flag   SPRAWDZANIE CS
			uint8_t stan_cs = 10;
			switch (stan_cs) {
			case 10: //CS_1n

				if (Tester_Acces_to_STER()) { //Plytka wlozona
					ZAL_TC_PKSPP_ON();
					//if( 10000==RS485_MM_R00() ){
					//Tester_Check_CS1n();
					//}
					USBD_Write(0x81, MODBUS_CS1_ok, sizeof(MODBUS_CS1_ok),
							dataSentCallback);
				} else { //Brak plytki
						 //komunikat brak plytki
					USBD_Write(0x81, MODBUS_Blad, sizeof(MODBUS_Blad),
							dataSentCallback);
				}
				//ZAL_TC_PKSPP_OFF();
				break;
			}
		}
	}
	while (flaga_zadan.TT_G != 0) {
		if ((flaga_zadan.TT_G & 0x01) == 0x01) {
			flaga_zadan.TT_G &= 0xFE; //Zadanie testu sprzetu
			Hardware_ALL();
		}
		if ((flaga_zadan.TT_G & 0x02) == 0x02) {
			flaga_zadan.TT_G &= 0xFD; //Test PORTA
			Hardware_TEST_PORTA();
		}
		if ((flaga_zadan.TT_G & 0x04) == 0x04) {
			flaga_zadan.TT_G &= 0xFB; //TEST PORTB
			Hardware_TEST_PORTB();
		}
		if ((flaga_zadan.TT_G & 0x08) == 0x08) {
			flaga_zadan.TT_G &= 0xF7; //TEST RDY
			Hardware_TEST_RDY();
		}
		if ((flaga_zadan.TT_G & 0x10) == 0x10) {
			flaga_zadan.TT_G &= 0xEF; //TEST POMIAR
			Hardware_TEST_POMIAR();
		}
		if ((flaga_zadan.TT_G & 0x20) == 0x20) {
			flaga_zadan.TT_G &= 0xDF; //TEST PWR
			Hardware_TEST_PWR();
		}
		if ((flaga_zadan.TT_G & 0x40) == 0x40) {
			flaga_zadan.TT_G &= 0xBF; //TEST ZER
			Hardware_TEST_ZER();
		}
		if ((flaga_zadan.TT_G & 0x80) == 0x80) {
			flaga_zadan.TT_G &= 0x7F; //TEST RESTART
			Hardware_TEST_RESTART();
		}
	}
}
