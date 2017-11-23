#include "tester.h"
#include "MCP23017.h"
#include "ADS1247.h"
#include "em_timer.h"
#include "RS485.h"


//volatile uint8_t clock;
//volatile uint8_t flagai2c_timeout=0;

ADC_profile tmp_profile;
ADC_profile profile1;
//Tester oTester;

//flagai2c_timeout=0;
//I2C_TransferReturn_TypeDef ret1;

void Init_Tester()
{
	  //PC0 HIGH Linia Reset do MCP23017
	  GPIO_PinModeSet(gpioPortC,0,gpioModePushPull,0);
	  GPIO_PinOutSet(gpioPortC,0);
	  Timer1_Configure(10);while(clock == 0); clock=0;
	  GPIO_PinOutClear(gpioPortC,0);
	  Timer1_Configure(100);while(clock == 0);clock=0;
	  GPIO_PinOutSet(gpioPortC,0);


	  //PC3 HIGH Linia Klucz
	  GPIO_PinModeSet(gpioPortC,3,gpioModePushPull,0);
	  //GPIO_PinOutClear(gpioPortC,3);
	  //GPIO_PinOutToggle(gpioPortC,3);
	  GPIO_PinOutSet(gpioPortC,3);

	  /*
	   * Konfiguracja ukladow
	   */
	  MCP23017_Configure_Default(Address_MCP23017_U18);
	  MCP23017_Configure_Default(Address_MCP23017_U10);
	  MCP23017_Configure_Default(Address_MCP23017_U17);
	  MCP23017_Configure_Default(Address_MCP23017_U22);
	  MCP23017_Configure_Default(Address_MCP23017_U30);

	  //zwracam konfiguracje
	  U10.IOCON = MCP23017_Read_Register(MCP23017_IOCONA,Address_MCP23017_U10);
	  U17.IOCON = MCP23017_Read_Register(MCP23017_IOCONA,Address_MCP23017_U17);
	  U18.IOCON = MCP23017_Read_Register(MCP23017_IOCONA,Address_MCP23017_U18);
	  U22.IOCON = MCP23017_Read_Register(MCP23017_IOCONA,Address_MCP23017_U22);
	  U30.IOCON = MCP23017_Read_Register(MCP23017_IOCONA,Address_MCP23017_U30);

	   //U10
	  MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U10,0x00);
	  MCP23017_Write_Register(MCP23017_GPIOA,Address_MCP23017_U10,0xFF);
	  MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U10,0x00);
	  MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U10,0xFF);

	  //U17
	  MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U17,0xFE);//U17.GPIOA od P1-P7 to sygnaly ready
	  MCP23017_Write_Register(MCP23017_GPINTENA,Address_MCP23017_U17,0x00); //Enable GPIO INTERRUPT
	  MCP23017_Write_Register(MCP23017_DEFVALA,Address_MCP23017_U17,0xFE);
	  MCP23017_Write_Register(MCP23017_INTCONA,Address_MCP23017_U17,0xFE); //COMPARED AGAINST THE PREVIOUS VALUE
	  //MCP23017_Write_Register(MCP23017_INTFA,Address_MCP23017_U17,0xFE);

	  MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U17,0x00);
	  MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U17,0x00);


	  /* U18
	   * CSxn - sygna³ cs do p³ytek
	   * CSx  - do kluczy
	   */

	  MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U18,0x00);
	  MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00);
	  MCP23017_Write_Register(MCP23017_GPIOA,Address_MCP23017_U18,0x00);
	  MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U18,0xFF);


	  // U22
	  MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U22,0x00);
	  MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U22,0x00);
	  U22.IODIRA = MCP23017_Read_Register(MCP23017_IODIRA,Address_MCP23017_U22);

	   /* ++U30++
	   *  SIGNAL JEST_MZB / JEST_STER / JEST_PKSPP AS INPUT
	   */
	  MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U30,0x00);//(_GPIOA7I|_GPIOA6I|_GPIOA5I));
	  MCP23017_Write_Register(MCP23017_GPPUA,Address_MCP23017_U30,0x00);//((_GPIOA7S)|(_GPIOA6S)|(_GPIOA5S)));
	  MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U30,(_GPIOA7I|_GPIOA6I|_GPIOA5I));
	  MCP23017_Write_Register(MCP23017_GPPUA,Address_MCP23017_U30,((_GPIOA7S)|(_GPIOA6S)|(_GPIOA5S)));

	  MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U30,0x00);
	  U30.IODIRA = MCP23017_Read_Register(MCP23017_IODIRA,Address_MCP23017_U30);
	  U30.GPPUA = MCP23017_Read_Register(MCP23017_GPPUA,Address_MCP23017_U30);
	  U30.IODIRB = MCP23017_Read_Register(MCP23017_IODIRB,Address_MCP23017_U30);
	  //Odczytuje rejestry IODIRB 1(INPUT) 0(OUTPUT)

}

void SEL_SPI1_TC_ON()
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)|(_GPIOA0S)));

}
void SEL_SPI1_TC_OFF()
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)&(0xFE)));
}
void SEL_SPI1_PKSPP_ON()
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)|(_GPIOA1S)));
}
void SEL_SPI1_PKSPP_OFF()
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)&(0xFD)));
}
void SEL_SPI_SL_ON()
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)|(_GPIOA4S)));
}
void SEL_SPI_SL_OFF()
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)&(0xEF)));
}

void ZAL_TC_PKSPP_ON(void)
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)|(_GPIOA3S)));
	U30.GPPUA=MCP23017_Read_Register(MCP23017_GPPUA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_GPPUA,Address_MCP23017_U30,((U30.GPPUA)|(_GPIOA3S)));
}

void ZAL_TC_PKSPP_OFF(void)
{
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)&(0xF7)));
	U30.GPPUA=MCP23017_Read_Register(MCP23017_GPPUA,Address_MCP23017_U30);
	MCP23017_Write_Register(MCP23017_GPPUA,Address_MCP23017_U30,((U30.GPPUA)&(0xF7)));
}

void PP_Select_EEPROM()
{
	/*
	 * zmieniam parametry transmisji i ustawiam linie CS
	 */

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockSelectSet(cmuClock_HF,cmuSelect_HFXO);
    CMU_ClockEnable(cmuClock_USART1, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    SystemCoreClockUpdate();
    // Select HF XTAL osc as system clock source. 48MHz XTAL, but we divided the system clock by 2, therefore our HF clock should be 24MHz
    SystemCoreClockUpdate();

    USART_InitSync_TypeDef spiInit =
    {
        .enable       = usartEnable,
        .refFreq      = 0,
        .baudrate     = 1000000, //**
        .databits     = usartDatabits8,//**
        .master       = true,
        .msbf         = true,
        .clockMode    = usartClockMode0,  //CPOL=0 CPHA =0
        .prsRxEnable  = false,          /* Enable USART Rx via Peripheral Reflex System */
        .prsRxCh      = usartPrsRxCh0,  /* Select PRS channel if enabled */
        .autoTx       = false,
    };

	  USART_Reset(USART1);
	  USART_InitSync(USART1, &spiInit);
	  USART1->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN |USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC1;

	  USART0->CTRL|=_USART_CTRL_TXDELAY_SINGLE;
	  USART_Enable(USART1,usartEnable);
	  Timer1_Configure(100);while(clock == 0);clock=0;

	  MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U10,0xFF);// Wylacz sygnal RESET na adc
	  Timer1_Configure(100);while(clock == 0);clock=0;

}
void PP_Select_ADC()
{

    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockSelectSet(cmuClock_HF,cmuSelect_HFXO);
    CMU_ClockEnable(cmuClock_USART1, true);
    CMU_ClockEnable(cmuClock_GPIO, true);
    SystemCoreClockUpdate();


	/// -------------------prepare SPI ---------------------------

    USART_InitSync_TypeDef spiInit =
    {
        .enable       = usartEnable,
        .refFreq      = 0,
        .baudrate     = 1000000,
        .databits     = usartDatabits8,
        .master       = true,
        .msbf         = true,
        .clockMode    = usartClockMode1,  //CPOL=0 CPHA =1
        .prsRxEnable  = false,          /* Enable USART Rx via Peripheral Reflex System */
        .prsRxCh      = usartPrsRxCh0,  /* Select PRS channel if enabled */
        .autoTx       = false,
    };

    USART_InitSync(USART1, &spiInit);

    /* Enabling pins and setting location, SPI CS not enable */
    USART1->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN
    		| USART_ROUTE_LOCATION_LOC1;

    Timer1_Configure(100);while(clock == 0);clock=0;
    MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U10,0xFF);// Wylacz sygnal RESET na adc
    Timer1_Configure(100);while(clock == 0);clock=0;
}

uint8_t PP_Select_CS_EEPROM(uint8_t PIN)
{
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00);   //U18.GPIOB na wyjscia
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATB ,Address_MCP23017_U18,~(PIN)); //U18.GPIOB(PIN) LOW --linia CS(x)n
	Timer1_Configure(100);while(clock == 0);clock=0;

	//blok do kluczy z DOUT
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U18,0x00);	  //U18.GPIOA as OUTPUT
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATA ,Address_MCP23017_U18,PIN|1); //U18.GPIOA(PIN)
	Timer1_Configure(100);while(clock == 0);clock=0;
	return 0;
}

uint8_t PP_Select_CS_ADC(uint8_t PIN)
{
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00);   //U18.GPIOB na wyjscia
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATB ,Address_MCP23017_U18,~(PIN)); //U18.GPIOB(PIN) LOW --linia CS(x)n
	Timer1_Configure(100);while(clock == 0);clock=0;

	//blok do kluczy z DOUT
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U18,0x00);	  //U18.GPIOA as OUTPUT
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATA ,Address_MCP23017_U18,((PIN)&0xFE)); //U18.GPIOA(PIN) LOW  --linia CS(x)
	Timer1_Configure(100);while(clock == 0);clock=0;

	return 0;
}

uint8_t PP_UnSelect_CS_EEPROM()
{
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U18,0xFF);
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U18,0x01);
	Timer1_Configure(100);while(clock == 0);clock=0;
	return 0;
}

uint8_t PP_UnSelect_CS_ADC()
{
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U18,0xFF);
	Timer1_Configure(100);while(clock == 0);clock=0;
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U18,0x00);
	Timer1_Configure(100);while(clock == 0);clock=0;
	return 0;
}

void PP_PWR_ON(uint8_t PIN, uint8_t zalacz)
{
	/*
	 * slot 1- 7
	 * zalacz 1/0
	 */
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U10,0x00); //U10.GAx as OUTPUT
	switch(PIN)
	{
	case _GPIOA1S:
		  if(zalacz)
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,~(_GPIOA1S)); //U10.GA1 na LOW
		  else
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,0x00);
		break;
	case _GPIOA2S:
		  if(zalacz)
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,~(_GPIOA2S)); //U10.GA2 na LOW
		  else
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,0x00);
		break;
	case _GPIOA3S:
		  if(zalacz)
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,~(_GPIOA3S));	//U10.GA3 na LOW
		  else
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,0x00);
		break;
	case _GPIOA4S:
		  if(zalacz)
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,~(_GPIOA4S));	//U10.GA4 na LOW
		  else
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,0x00);
		break;
	case _GPIOA5S:
		  if(zalacz)
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,~(_GPIOA5S));	//U10.GA5 na LOW
		  else
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,0x00);
		break;
	case _GPIOA6S:
		  if(zalacz)
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,~(_GPIOA6S));	//U10.GA6 na LOW
		  else
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,0x00);
		break;
	case _GPIOA7S:
		  if(zalacz)
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,~(_GPIOA7S));	//U10.GA7 na LOW
		  else
			  MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U10,0x00);
		break;
	}
}

uint8_t PP_CheckEEPROM(uint8_t PIN) {

    uint8_t res[3]; uint8_t out=1; uint8_t rdy=1;
    PP_PWR_ON(PIN,1);

    PP_Select_EEPROM();

    PP_Select_CS_EEPROM(PIN);    // WEN

   	USART_SpiTransfer(USART1,0x06);
    PP_UnSelect_CS_EEPROM();

    PP_Select_CS_EEPROM(PIN);
    USART_SpiTransfer(USART1,0x06);
    PP_UnSelect_CS_EEPROM();

    PP_Select_CS_EEPROM(PIN);
	Timer1_Configure(100);
	while(clock == 0);
	clock=0;
    USART_SpiTransfer(USART1,0x02); //W Data to Memory
    USART_SpiTransfer(USART1,0x00);
    USART_SpiTransfer(USART1,0x80);
    USART_SpiTransfer(USART1,'m');
    USART_SpiTransfer(USART1,'i');
    USART_SpiTransfer(USART1,'s');
    PP_UnSelect_CS_EEPROM();

    while(rdy) {
        PP_Select_CS_EEPROM(PIN);
        USART_SpiTransfer(USART1,0x05); //Read status Register

        rdy=USART_SpiTransfer(USART1,0xFF)&1;
        PP_UnSelect_CS_EEPROM();
    }

    PP_Select_CS_EEPROM(PIN);

    USART_SpiTransfer(USART1,0x03); //Read status Register
    USART_SpiTransfer(USART1,0x00);
    USART_SpiTransfer(USART1,0x80);
    res[0]=USART_SpiTransfer(USART1,0xFF);

//    dprintf("\r\nROUTE:%X\r\n",USART0->ROUTE);
    //dprintf("%x",res);
    if(res[0]!='m') out=0;

    res[1]=USART_SpiTransfer(USART1,0xFF);
    //dprintf("%x",res);
    if(res[1]!='i') out=0;

    res[2]=USART_SpiTransfer(USART1,0xFF);
    //dprintf("%x",res);
    if(res[2]!='s') out=0;

    PP_UnSelect_CS_EEPROM();
	Timer1_Configure(100);
	while(clock == 0);
	clock=0;
    if(out) {
        return 49;

    } else
    {
        return 48;
    }
}

/*void PP_StatusEEPROM(uint8_t PIN)
{
    uint8_t res;
    PP_Select_EEPROM();
    PP_UnSelect_CS();

    PP_Select_CS(PIN);
    USART_SpiTransfer(USART1,0x05);
    PP_UnSelect_CS();

    PP_Select_CS(PIN);
    res=USART_SpiTransfer(USART1,0xFF);
    //dprintf("%X",res);
    PP_UnSelect_CS();

}*/

/*void PP_ReadEEPROM(uint8_t PP_No,ADC_profile *profil,uint8_t typ,uint8_t PIN)
{
    uint8_t profile_no;
    uint8_t rdy=1;

    switch(typ) {
    case HZ:
        profile_no=0;
        break;
    case DC:
        profile_no=1;
        break;
    case EEPROM_TEST:
        profile_no=3;

    }

    uint16_t EEPROM_adr = (profile_no * 0x40)+0x20;

    PP_Select_EEPROM();

    PP_Select_CS(PIN);

    PP_UnSelect_CS();

    while(rdy) {
        PP_Select_CS(PIN);
        USART_SpiTransfer(USART1,0x05);
        rdy=USART_SpiTransfer(USART1,0xFF)&1;
        PP_UnSelect_CS();
        //Delay(5);
    }

    PP_Select_CS(PIN);


    //dprintf("\n\nOdczyt EEPROM \n\n");
    USART_SpiTransfer(USART1,0x03);
    USART_SpiTransfer(USART1,(EEPROM_adr>>8)&0x00FF);
    USART_SpiTransfer(USART1,(EEPROM_adr)&0x00FF);
    //USART_SpiTransfer(USART0,0x00);
    //USART_SpiTransfer(USART0,0x00);
    profil->MUX0 = USART_SpiTransfer(USART1,0xFF);
    profil->VBIAS = USART_SpiTransfer(USART1,0xFF);
    profil->MUX1 = USART_SpiTransfer(USART1,0xFF);
    profil->SYS0 = USART_SpiTransfer(USART1,0xFF);
    profil->OFC0 = USART_SpiTransfer(USART1,0xFF);
    profil->OFC1 = USART_SpiTransfer(USART1,0xFF);
    profil->OFC2 = USART_SpiTransfer(USART1,0xFF);
    profil->FSC0 = USART_SpiTransfer(USART1,0xFF);
    profil->FSC1 = USART_SpiTransfer(USART1,0xFF);
    profil->FSC2 = USART_SpiTransfer(USART1,0xFF);
    profil->IDAC0 = USART_SpiTransfer(USART1,0xFF);
    profil->IDAC1 = USART_SpiTransfer(USART1,0xFF);
    profil->GPIOCFG = USART_SpiTransfer(USART1,0xFF);
    profil->GPIODIR = USART_SpiTransfer(USART1,0xFF);
    profil->GPIODAT = USART_SpiTransfer(USART1,0xFF);

    PP_UnSelect_CS();

}*/

/*void PP_WriteEEPROM(uint8_t PP_No,ADC_profile *profil,uint8_t typ,uint8_t PIN)
{
    uint8_t profile_no;
    switch(typ) {
    case HZ:
        profile_no=0;
        break;
    case DC:
        profile_no=1;
        break;
    case EEPROM_TEST:
        profile_no=3;
    }

    uint16_t EEPROM_adr = (profile_no * 0x40)+0x20;

    //dprintf("\n\nZapis EEPROM \n\n");

    PP_Select_EEPROM();
    PP_UnSelect_CS();

    PP_Select_CS(PIN);

    // WEN
    USART_SpiTransfer(USART1,0x06);

    PP_UnSelect_CS();
    PP_Select_CS(PIN);

    USART_SpiTransfer(USART1,0x06);// WEN - 2 raz

    PP_UnSelect_CS();

    PP_Select_CS(PIN);

    // Write page
    USART_SpiTransfer(USART1,0x02);
    USART_SpiTransfer(USART1,(EEPROM_adr>>8)&0x00FF);
    USART_SpiTransfer(USART1,(EEPROM_adr)&0x00FF);

//    nr profilu
    USART_SpiTransfer(USART1,profile_no);
    USART_SpiTransfer(USART1,profil->MUX0);
    USART_SpiTransfer(USART1,profil->VBIAS);
    USART_SpiTransfer(USART1,profil->MUX1);
    USART_SpiTransfer(USART1,profil->SYS0);
    USART_SpiTransfer(USART1,profil->OFC0);
    USART_SpiTransfer(USART1,profil->OFC1);
    USART_SpiTransfer(USART1,profil->OFC2);
    USART_SpiTransfer(USART1,profil->FSC0);
    USART_SpiTransfer(USART1,profil->FSC1);
    USART_SpiTransfer(USART1,profil->FSC2);
    USART_SpiTransfer(USART1,profil->IDAC0);
    USART_SpiTransfer(USART1,profil->IDAC1);
    USART_SpiTransfer(USART1,profil->GPIOCFG);
    USART_SpiTransfer(USART1,profil->GPIODIR);
    USART_SpiTransfer(USART1,profil->GPIODAT);

    //Delay(100);
    PP_UnSelect_CS();
}*/


void Timer2_Configure(uint16_t scaler)
{
	CMU_ClockEnable(cmuClock_TIMER2, true);   // Enable TIMER0 peripheral clock
	TIMER_Init_TypeDef timer2Init =  //Select TIMER0 parameters
		  {
		    .enable     = true,
		    .debugRun   = false,
		    .prescale   = timerPrescale512,
		    .clkSel     = timerClkSelHFPerClk,
		    .fallAction = timerInputActionNone,
		    .riseAction = timerInputActionNone,
		    .mode       = timerModeUp,
		    .dmaClrAct  = false,
		    .quadModeX4 = false,
		    .oneShot    = false,
		    .sync       = false,
		  };

		  // Set TIMER Top value - Top Value / (8MHz / 1024 (prescaler))
		  // For 2 Seconds Top Value= 15625
		  TIMER_TopSet(TIMER2, scaler * 16);
		  TIMER_Init(TIMER2, &timer2Init);    /* Configure TIMER */
		  TIMER_IntEnable(TIMER2, TIMER_IF_OF); 	  // Enable overflow interrupt
		  NVIC_EnableIRQ(TIMER2_IRQn);     // Enable TIMER0 interrupt vector in NVIC
		  clock=0;
}

void TIMER1_IRQHandler(void)
{
	TIMER_IntDisable(TIMER1, TIMER_IF_OF);
	TIMER_IntClear(TIMER1, TIMER_IF_OF);
	clock=1;

}
void TIMER2_IRQHandler(void)
{
	TIMER_IntDisable(TIMER2, TIMER_IF_OF);
	TIMER_IntClear(TIMER2, TIMER_IF_OF);
	flagai2c_timeout = 1;
}

void DebugMon_Handler(void)
{

}
void PP_FormatEEPROM(uint8_t PP_No,uint8_t PP_Type,uint8_t PP_Version,uint8_t PP_Device,uint8_t PIN)
{
    uint16_t EEPROM_adr = 0x10;
    //dprintf("\n\nFormatowanie EEPROM \n\n");
    PP_Select_EEPROM();
    PP_UnSelect_CS();
    PP_Select_CS(PIN);

    // WEN
    USART_SpiTransfer(USART1,0x06);

    PP_UnSelect_CS();
    PP_Select_CS(PIN);
    // WEN - 2 raz
    USART_SpiTransfer(USART1,0x06);

    PP_UnSelect_CS();

    PP_Select_CS(PIN);


    // Write page
    USART_SpiTransfer(USART1,0x02);
    USART_SpiTransfer(USART1,(EEPROM_adr>>8)&0x00FF);
    USART_SpiTransfer(USART1,(EEPROM_adr)&0x00FF);

//    nr profilu
    USART_SpiTransfer(USART1,PP_Type);
    USART_SpiTransfer(USART1,PP_Version);
    USART_SpiTransfer(USART1,PP_Device);
    for(uint8_t i = 3; i<16; i++) USART_SpiTransfer(USART1,'r');
//    for(int i = 0; i<16; i++) USART_SpiTransfer(USART0,'r');
// for(int i = 0; i<16; i++) USART_SpiTransfer(USART0,'r');
// for(int i = 0; i<16; i++) USART_SpiTransfer(USART0,'r');
// for(int i = 0; i<16; i++) USART_SpiTransfer(USART0,'r');

    PP_UnSelect_CS();

}

/*void  PP_LoadType(uint8_t PP_No,UDRegister *typ, uint8_t PIN) {

    uint8_t i;
	uint16_t EEPROM_adr =0x10;

    PP_Select_EEPROM();
    PP_Select_CS(PIN);

    //dprintf("\n\nOdczyt typu \n\n");

    USART_SpiTransfer(USART1,0x03);
    USART_SpiTransfer(USART1,(EEPROM_adr>>8)&0x00FF);
    USART_SpiTransfer(USART1,(EEPROM_adr)&0x00FF);

    typ = USART_SpiTransfer(USART1,0xFF);
    for(i=3; i<16; i++)USART_SpiTransfer(USART1,0xFF);

    PP_UnSelect();

    if(typ      == PP_100mA) typ = 0.1;
    else if(typ == PP_100mV) typ = 0.1;
    else if(typ == PP_10V)   typ = 10.0;
    else if(typ == PP_500V)  typ = 500.0;
    else if(typ == PP_5mA)   typ = 5.0;

    else typ->Value32b=0L;
}*/


/*
 *   ADC
 */
void ResetADC(uint8_t PIN)
{
	/*
	 * Restart ADC - Aktywny gdy stan LOW
	 */
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U10,~(PIN));
}

void PP_INIT_ADC(uint8_t PIN)
{
	PP_Select_ADC();
	PP_Select_CS(PIN);
	PP_StartMeasure(PIN);
    Timer1_Configure(2);//
    while(clock == 0);//
    clock=0;//

    MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U10,0x00);
    Timer1_Configure(10);
    while(clock == 0);
    clock=0;
    MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U10,0xFF);
    Timer1_Configure(200);
    while(clock == 0);
    clock=0;

    USART_SpiTransfer(USART1,0x06); //Restart
    Timer1_Configure(200);
    while(clock == 0);
    clock=0;

	 USART_SpiTransfer(USART1,ADS1247_SDATAC);

	USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_MUX1);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,0x23<<1); //MUX1

	USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_VBIAS);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,0x00); //VBIAS

	USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_SYS0);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,0x08<<1); //SYS0

	USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_IDAC1);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,0xCC<<1); //IDAC1

	 /*USART_SpiTransfer(USART1,0x00); //MUX0
	 USART_SpiTransfer(USART1,0x00); //VBIAS
	 USART_SpiTransfer(USART1,0x23<<1); //MUX1
	 USART_SpiTransfer(USART1,0x00); //SYS0
	 USART_SpiTransfer(USART1,0x00); //OFC0
	 USART_SpiTransfer(USART1,0x00); //OFC1
	 USART_SpiTransfer(USART1,0x00); //OFC2
	 USART_SpiTransfer(USART1,0x00); //FSC0
	 USART_SpiTransfer(USART1,0x00); //FSC1
	 USART_SpiTransfer(USART1,0x00); //FSC2
	 USART_SpiTransfer(USART1,0x00); //IDAC0
	 USART_SpiTransfer(USART1,0xCC<<1); //IDAC1
	 USART_SpiTransfer(USART1,0x00); //GPIOCFG
	 USART_SpiTransfer(USART1,0x00); //GPIODIR
	 USART_SpiTransfer(USART1,0x00); //GPIODAT*/


	 PP_UnSelect_CS();
	 PP_StopMeasure(PIN);
	 Timer1_Configure(300);
	 while(clock == 0);
	 clock=0;

}

ADC_profile PP_ReadADCConfig(uint8_t PIN, ADC_profile profil)
{
    PP_Select_ADC();
    PP_Select_CS_ADC(PIN);
    PP_StartMeasure(PIN);

    Timer1_Configure(1);
    while(clock == 0);
    clock=0;

    USART_SpiTransfer(USART1,ADS1247_RREG|ADS1247_R_MUX0);
    USART_SpiTransfer(USART1,0x0E);

    profile1.MUX0  = USART_SpiTransfer(USART1,0xFF);
    profile1.VBIAS  = USART_SpiTransfer(USART1,0xFF);
    profile1.MUX1   = USART_SpiTransfer(USART1,0xFF);
    profile1.SYS0  = USART_SpiTransfer(USART1,0xFF);
    profile1.OFC0  = USART_SpiTransfer(USART1,0xFF);
    profile1.OFC1  = USART_SpiTransfer(USART1,0xFF);
    profile1.OFC2   = USART_SpiTransfer(USART1,0xFF);
    profile1.FSC0   = USART_SpiTransfer(USART1,0xFF);
    profile1.FSC1   = USART_SpiTransfer(USART1,0xFF);
    profile1.FSC2   = USART_SpiTransfer(USART1,0xFF);
    profile1.IDAC0  = USART_SpiTransfer(USART1,0xFF);
    profile1.IDAC1  = USART_SpiTransfer(USART1,0xFF);
    profile1.GPIOCFG = USART_SpiTransfer(USART1,0xFF);
    profile1.GPIODIR = USART_SpiTransfer(USART1,0xFF);
    profile1.GPIODAT = USART_SpiTransfer(USART1,0xFF);

    PP_UnSelect_CS_ADC();
    PP_StopMeasure(PIN);
    Timer1_Configure(50);
    while(clock == 0);
    clock=0;
    return profile1;
}
void PP_WriteADCConfig(uint8_t PIN, ADC_profile profil)
{
    PP_Select_ADC();
    PP_Select_CS_ADC(PIN);
    PP_StartMeasure(PIN);

    USART_SpiTransfer(USART1,0x06); //Restart
    Timer1_Configure(200);
    while(clock == 0);
    clock=0;

    USART_SpiTransfer(USART1,ADS1247_SDATAC);

	USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_MUX1);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,profil.MUX1); //MUX1

	/*USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_VBIAS);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,profil.VBIAS<<1); //VBIAS*/

	USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_SYS0);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,profil.SYS0); //SYS0

	USART_SpiTransfer(USART1,ADS1247_WREG|ADS1247_R_IDAC1);
	USART_SpiTransfer(USART1,0x00);
	USART_SpiTransfer(USART1,profil.IDAC1); //IDAC1


	 PP_UnSelect_CS_ADC();
	 PP_StopMeasure(PIN);

	 Timer1_Configure(50);
	 while(clock == 0);
	 clock=0;
}

uint8_t PP_StartMeasure(uint8_t PIN)
{
    MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U17,PIN); // START MEASURE
    return 0;
}
uint8_t PP_StopMeasure(uint8_t PIN)
{
    MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U17,0x00); // STOP MEASURE
    return 0;
}



float PP_SingleM(uint8_t PIN, ADC_profile *profil) {
    uint8_t msr[4];
    int32_t m=0;
    float measure = 0;

   // Timer0_Configure(16);
   // MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U10,0xFF); //U10.Gx as INPUT do funkcji PP_CheckReady

   /* PP_Select_ADC();
    PP_Select_CS(PIN);

    MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U17,PIN); // START MEASURE
    wstrzymaj(5000);*/

    //USART_SpiTransfer(USART1,ADS1247_RESET);
    //wstrzymaj(1000);
    //USART_SpiTransfer(USART1,ADS1247_SDATAC); // STOP READ DATA
   // USART_SpiTransfer(USART1,ADS1247_WREG);

    //USART_SpiTransfer(USART1,ADS1247_SYNC);
    //wstrzymaj(1000);
    //PP_UnSelect_CS();


    /*PP_Select_ADC();
    Timer0_Configure(10);
    while(clock == 0);
    clock=0;

    PP_Select_CS(2);
    Timer0_Configure(100);
    while(clock == 0);
    clock=0;*/

    /*MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U10,~(2));
    Timer0_Configure(10);
    while(clock == 0);
    clock=0;
    MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U10,2);
    Timer0_Configure(500);
    while(clock == 0);
    clock=0;*/

    PP_Select_ADC();
    Timer1_Configure(10);
    while(clock == 0);
    clock=0;
    PP_Select_CS(PIN);
    Timer1_Configure(10);
     while(clock == 0);
     clock=0;


    MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U17,PIN); // START MEASURE
    Timer1_Configure(10);
    while(clock == 0);
    clock=0;
    MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U17,0x00); // STOP MEASURE

    //PP_CheckReady(2);
    Timer1_Configure(10);
    while(clock == 0);
    clock=0;

    USART_SpiTransfer(USART1,ADS1247_RDATA);

    msr[2]= USART_SpiTransfer(USART1,0xFF)&0x000000FF;
    msr[1]= USART_SpiTransfer(USART1,0xFF)&0x000000FF;
    msr[0]= USART_SpiTransfer(USART1,0xFF)&0x000000FF;

    //PP_UnSelect_CS();
    Timer1_Configure(100);
    while(clock == 0);
    clock=0;

    //dprintf("MSR: %d\n",msr[0]);
    //dprintf("MSR: %d\n",msr[1]);
    //dprintf("MSR: %d\n",msr[2]);
    m = ( ( msr[ 2 ] << 24 ) | ( msr[ 1 ] << 16 ) | ( msr[ 0 ] <<8 ) )>>8 ;

    //    if(m&(1<<24)) mp=(~(m&0xFFFFFF)+1)&0xffffff;
    //    else mp=m&0x7FFFFF;
    //m=m&0xffffff;

    //PP_ReadLSB(PP_No,LSB,1);
    //measure=( ( float ) m ) * LSB->fValue;

    //dprintf("\r\nZ = %u  f=%f\r\n",m,measure);
    return measure;
}

float PP_SingleMinfo(uint8_t PIN, ADC_profile profil) {
    uint8_t msr[4];
    int32_t m=0;
    uint8_t gain;
    float measure;


    if(profil.MUX1!=0) {
        //PP_WriteADCConfig(PIN,profil);
        //Timer0_Configure(10);
        //while(clock == 0);
        //clock=0;
    }


    PP_Select_ADC();
    Timer1_Configure(10);
    while(clock == 0);
    clock=0;
    PP_Select_CS_ADC(PIN);
    Timer1_Configure(5);
    while(clock == 0);
    clock=0;

    MCP23017_Write_Register(MCP23017_GPINTENA,Address_MCP23017_U17,0xFE); //Enable Inter
    MCP23017_Write_Register(MCP23017_GPIOB,Address_MCP23017_U17,PIN); // START MEASURE

    while(PP_ADC_RDY);

    USART_SpiTransfer(USART1,ADS1247_RDATA);
    msr[2]= USART_SpiTransfer(USART1,0xFF)&0x000000FF;
    msr[1]= USART_SpiTransfer(USART1,0xFF)&0x000000FF;
    msr[0]= USART_SpiTransfer(USART1,0xFF)&0x000000FF;

    Timer1_Configure(10);
    while(clock == 0);
    clock=0;
    PP_UnSelect_CS_ADC();
    MCP23017_Write_Register(MCP23017_GPINTENA,Address_MCP23017_U17,0x00); //Disable inter


    //dprintf("MSR: %d\n",msr[0]);
    //dprintf("MSR: %d\n",msr[1]);
    //dprintf("MSR: %d\n",msr[2]);
    m = ( ( msr[ 2 ] << 16 ) | ( msr[ 1 ] << 8) | ( msr[ 0 ]  ) );

    gain=power2(((profil.SYS0)>>4)&7); //
    measure=(2*3.6/(float)gain)/16777216;

    measure=( ( float ) m )* measure;

    //dprintf("\r\nZ = %u  f=%f\r\n",m,measure);

    return measure;
}

float PP_ReadVdd(int PIN) {

	float war,temp;
	Timer1_Configure(100);
	while(clock == 0);
	clock=0;
    PP_ReadADCConfig(PIN,tmp_profile);
    tmp_profile=profile1;
    tmp_profile.SYS0=8;
    tmp_profile.MUX1=39;
    PP_WriteADCConfig(PIN,tmp_profile);
    tmp_profile=PP_ReadADCConfig(PIN,tmp_profile);

    war=PP_SingleMinfo(PIN,tmp_profile);

    temp=4*war;
    //return temp;
    if(temp >= 2.97 && temp <= 3.9)
    {
    	//printf RS485 - ok.
    	return temp;
    }
    else
    {
    	//printf RS485 - nok.
    	return 0;
    }
}

int power2( unsigned int exp) {
    int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= 2;
    return result;
}
float PP_ReadTemp(int PIN) {
    float war,temp;
    tmp_profile = PP_ReadADCConfig(PIN,tmp_profile);
    tmp_profile.SYS0=8;
    tmp_profile.MUX1=35;
    PP_WriteADCConfig(PIN,tmp_profile);
    war=PP_SingleMinfo(PIN,tmp_profile);

    temp=25+(war-0.118)/0.405;
    //printf RS485 - temp.
    return temp;

}
float PP_ReadVav(int PIN) {

    float war,temp;

    tmp_profile=PP_ReadADCConfig(PIN,tmp_profile);
    tmp_profile.SYS0=8;
    tmp_profile.MUX1=38;
    PP_WriteADCConfig(PIN,tmp_profile);
    war=PP_SingleMinfo(PIN,tmp_profile);

    temp=4*war;
    if(temp >= 4.75 && temp <= 5.35)
    {
    	//printf RS485 - ok.
    	return temp;
    }
    else
    {
    	//printf RS485 - nok.
    	return 0;
    }
}
float PP_ReadVref(int PIN) {

    float war,temp;

    tmp_profile=PP_ReadADCConfig(PIN,tmp_profile);
    tmp_profile.SYS0=8;
    tmp_profile.MUX1=37;
    PP_WriteADCConfig(PIN,tmp_profile);
    war=PP_SingleMinfo(PIN,tmp_profile);
    temp=4*war;
    if(temp >= 3.564 && temp <= 3.636)
    {
    	//printf RS485 - ok.
    	return temp;
    }
    else
    {
    	//printf RS485 - nok.
    	return 0;
    }

}

/*
 * LED
 */

void Tester_LED_RED(void)
{
	//U22.OLATB = MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U22);
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U22,(_GPIOB6S|_GPIOB5S));
	//U22.OLATB = MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U22);
}
void Tester_LED_GREEN(void)
{
	//U22.OLATB = MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U22);
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U22,(_GPIOB7S|_GPIOB5S));
	//U22.OLATB = MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U22);
}
void Tester_LED_BLUE(void)
{
	// Sygnal sterujacy ZAL_ZAS_BAT -> U30.GPA2
	//U22.OLATB = MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U22);
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U22,(_GPIOB7S|_GPIOB6S));
	//U22.OLATB = MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U22);
}


/*
 * PrzekaŸnik
 */
void Tester_Relay_PK1_ON(void)
{
	// Sygnal sterujacy ZAL_ZAS_BAT -> U30.GPA2
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.GPPUA=MCP23017_Read_Register(MCP23017_GPPUA,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUA,Address_MCP23017_U30,(U30.GPPUA|(_GPIOA2S)));
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,(U30.OLATA|(_GPIOA2S)));
	//Timer1_Configure(100); while(clock == 0); clock=0;

}
void Tester_Relay_PK1_OFF(void)
{
	// Sygnal sterujacy ZAL_ZAS_BAT -> U30.GPA2
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.GPPUA=MCP23017_Read_Register(MCP23017_GPPUA,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUA,Address_MCP23017_U30,((U30.GPPUA)&(0xFB)));
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.OLATA=MCP23017_Read_Register(MCP23017_OLATA,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_OLATA,Address_MCP23017_U30,((U30.OLATA)&(0xFB)));
	//Timer1_Configure(100); while(clock == 0); clock=0;

}

void Tester_Relay_PK2_ON(void)
{
	// Sygnal sterujacy ZAL_LD_RS485 -> U30.GPB7
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.GPPUB=MCP23017_Read_Register(MCP23017_GPPUB,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U30,(U30.GPPUB|(_GPIOB7S)));
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.OLATB=MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U30,(U30.OLATB|(_GPIOB7S)));
	//Timer1_Configure(100); while(clock == 0); clock=0;
}
void Tester_Relay_PK2_OFF(void)
{
	// Sygnal sterujacy ZAL_LD_RS485 -> U30.GPB7
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.GPPUB=MCP23017_Read_Register(MCP23017_GPPUB,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U30,(U30.GPPUB&0x7F));
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.OLATB=MCP23017_Read_Register(MCP23017_OLATB,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U30,(U30.OLATB&0x7F));
	//Timer1_Configure(100); while(clock == 0); clock=0;
}

/*
 * Dostep do peryferiow
 */

void Tester_Acces_to_ALL(void){
	//Timer1_Configure(100); while(clock == 0); clock=0;
	//Delay(100);
	U30.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U30);
	//Timer1_Configure(100); while(clock == 0); clock=0;
	Tester.Plytki.JEST_MZB=((U30.GPIOA)&0x80)>>7;
	Tester.Plytki.JEST_PKSPP=(((U30.GPIOA)&0x40)>>6);
	Tester.Plytki.JEST_STER=(((U30.GPIOA)&0x20)>>5);
}
uint8_t Tester_Acces_to_MZB(void)
{
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return (((~(U30.GPIOA))&(0x80))>>7);
}
uint8_t Tester_Acces_to_STER(void)
{
	/*
	 * Funkcja zwraca 1 gdy plytka sterownika wlozona do testera
	 */
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return (((~(U30.GPIOA))&(0x40))>>6);
}
uint8_t Tester_Acces_to_PKSPP(void)
{
	/*
	 * Funkcja zwraca 1 gdy plytka pomiarowa wlozona do testera
	 */
	Timer1_Configure(100); while(clock == 0); clock=0;
	U30.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U30);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return (((~(U30.GPIOA))&(0x20))>>5);
}

/*
 * Sprawdzanie sygnalu CS od TelCor
 */
/*uint8_t Tester_Check_CS1n(void)
{
	RxBuffor.i=0;
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0xFF); //U18.PINx as INPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0xFF); //U18.PINx as PULL-UP
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	Timer1_Configure(100); while(clock == 0); clock=0;
	RS485_MM_K01();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	RS485_MM_WykonajPomiar();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	while(U18.GPIOB == MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18)){
	//while(RS485_MM_R00()!=10001){
		Timer1_Configure(100); while(clock == 0); clock=0;
	}
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	ZAL_TC_PKSPP_OFF();
	RS485_MM_W_Restart();
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00); //U18.PINx as OUTPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0x00);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return 0;
}
uint8_t Tester_Check_CS2n(void)
{
	RxBuffor.i=0;
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0xFF); //U18.PINx as INPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0xFF); //U18.PINx as PULL-UP
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	Timer1_Configure(100); while(clock == 0); clock=0;
	RS485_MM_K02();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	RS485_MM_WykonajPomiar();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	while(U18.GPIOB == MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18)){
	//while(RS485_MM_R00()!=10001){
		Timer1_Configure(100); while(clock == 0); clock=0;
	}
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	ZAL_TC_PKSPP_OFF();
	RS485_MM_W_Restart();
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00); //U18.PINx as OUTPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0x00);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return 0;
}
uint8_t Tester_Check_CS3n(void)
{
	RxBuffor.i=0;
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0xFF); //U18.PINx as INPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0xFF); //U18.PINx as PULL-UP
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	Timer1_Configure(100); while(clock == 0); clock=0;
	RS485_MM_K03();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	RS485_MM_WykonajPomiar();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	while(U18.GPIOB == MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18)){
	//while(RS485_MM_R00()!=10001){
		Timer1_Configure(100); while(clock == 0); clock=0;
	}
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	ZAL_TC_PKSPP_OFF();
	RS485_MM_W_Restart();
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00); //U18.PINx as OUTPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0x00);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return 0;
}
uint8_t Tester_Check_CS4n(void)
{
	RxBuffor.i=0;
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0xFF); //U18.PINx as INPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0xFF); //U18.PINx as PULL-UP
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	Timer1_Configure(100); while(clock == 0); clock=0;
	RS485_MM_K04();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	RS485_MM_WykonajPomiar();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	while(U18.GPIOB == MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18)){
	//while(RS485_MM_R00()!=10001){
		Timer1_Configure(100); while(clock == 0); clock=0;
	}
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	ZAL_TC_PKSPP_OFF();
	RS485_MM_W_Restart();
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00); //U18.PINx as OUTPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0x00);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return 0;
}
uint8_t Tester_Check_CS5n(void)
{
	RxBuffor.i=0;
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0xFF); //U18.PINx as INPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0xFF); //U18.PINx as PULL-UP
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	Timer1_Configure(100); while(clock == 0); clock=0;
	RS485_MM_K05();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	RS485_MM_WykonajPomiar();
	Timer1_Configure(1000); while(clock == 0); clock=0;
	while(U18.GPIOB == MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18)){
	//while(RS485_MM_R00()!=10001){
		Timer1_Configure(100); while(clock == 0); clock=0;
	}
	Timer1_Configure(100); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);
	ZAL_TC_PKSPP_OFF();
	RS485_MM_W_Restart();
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x00); //U18.PINx as OUTPUT
	Timer1_Configure(100); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_GPPUB,Address_MCP23017_U18,0x00);
	Timer1_Configure(100); while(clock == 0); clock=0;
	return 0;
}*/

/*
 * Test Hardware
 */
uint8_t Hardware_ALL()
{
	uint8_t UxxBuffor[19]="TG00_U0GA_00000000";

	//Ustaw GPIO jako wejscia
	/*MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U10,0xFF);
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U10,0xFF);
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U17,0xFF);
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U17,0xFF);
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U18,0xFF);
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0xFF);*/

	//Wyslij komende diagnostyczna po modbus, urzadzenie ma wystawic stany niskie na wyjsciach
	//Odbierz ack
	Timer1_Configure(500); while(clock == 0); clock=0;
	U10.GPIOA = MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U10);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U10.GPIOB = MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U10);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U17.GPIOA = MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U17);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U17.GPIOB = MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U17);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U18.GPIOA = MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U18);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U18.GPIOB = MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);

	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U10.GPIOA == 0x00){
		UxxBuffor[6]='0';UxxBuffor[8]='A';
		UxxBuffor[10]=(uint8_t)((U10.GPIOA&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U10.GPIOA&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U10.GPIOA&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U10.GPIOA&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U10.GPIOA&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U10.GPIOA&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U10.GPIOA&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U10.GPIOA&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='0';UxxBuffor[8]='A';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(800); while(clock == 0); clock=0;
	//if(U10.GPIOB == 0x00){
		UxxBuffor[6]='0';UxxBuffor[8]='B';
		UxxBuffor[10]=(uint8_t)((U10.GPIOB&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U10.GPIOB&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U10.GPIOB&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U10.GPIOB&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U10.GPIOB&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U10.GPIOB&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U10.GPIOB&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U10.GPIOB&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='0';UxxBuffor[8]='B';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U17.GPIOA == 0x00){
		UxxBuffor[6]='7';UxxBuffor[8]='A';
		UxxBuffor[10]=(uint8_t)((U17.GPIOA&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U17.GPIOA&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U17.GPIOA&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U17.GPIOA&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U17.GPIOA&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U17.GPIOA&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U17.GPIOA&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U17.GPIOA&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='7';UxxBuffor[8]='A';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U17.GPIOB == 0x00){
		UxxBuffor[6]='7';UxxBuffor[8]='B';
		UxxBuffor[10]=(uint8_t)((U17.GPIOB&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U17.GPIOB&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U17.GPIOB&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U17.GPIOB&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U17.GPIOB&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U17.GPIOB&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U17.GPIOB&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U17.GPIOB&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='7';UxxBuffor[8]='B';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U18.GPIOA == 0x00){
		UxxBuffor[6]='8';UxxBuffor[8]='A';
		UxxBuffor[10]=(uint8_t)((U18.GPIOA&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U18.GPIOA&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U18.GPIOA&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U18.GPIOA&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U18.GPIOA&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U18.GPIOA&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U18.GPIOA&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U18.GPIOA&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='8';UxxBuffor[8]='A';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U18.GPIOB == 0x00){
		UxxBuffor[6]='8';UxxBuffor[8]='B';
		UxxBuffor[10]=(uint8_t)((U18.GPIOB&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U18.GPIOB&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U18.GPIOB&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U18.GPIOB&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U18.GPIOB&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U18.GPIOB&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U18.GPIOB&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U18.GPIOB&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='8';UxxBuffor[8]='B';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;

	//Wyslij komende diagnostyczna po modbus, urzadzenie ma wystawic stany niskie na wyjsciach
	//Odbierz ack

	U10.GPIOA = MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U10);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U10.GPIOB = MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U10);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U17.GPIOA = MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U17);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U17.GPIOB = MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U17);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U18.GPIOA = MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U18);
	Timer1_Configure(500); while(clock == 0); clock=0;
	U18.GPIOB = MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);

	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U10.GPIOA == 0xFF){
		UxxBuffor[6]='0';UxxBuffor[8]='A';
		UxxBuffor[10]=(uint8_t)((U10.GPIOA&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U10.GPIOA&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U10.GPIOA&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U10.GPIOA&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U10.GPIOA&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U10.GPIOA&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U10.GPIOA&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U10.GPIOA&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='0';UxxBuffor[8]='A';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(800); while(clock == 0); clock=0;
	//if(U10.GPIOB == 0xFF){
		UxxBuffor[6]='0';UxxBuffor[8]='B';
		UxxBuffor[10]=(uint8_t)((U10.GPIOB&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U10.GPIOB&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U10.GPIOB&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U10.GPIOB&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U10.GPIOB&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U10.GPIOB&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U10.GPIOB&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U10.GPIOB&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='0';UxxBuffor[8]='B';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U17.GPIOA == 0xFF){
		UxxBuffor[6]='7';UxxBuffor[8]='A';
		UxxBuffor[10]=(uint8_t)((U17.GPIOA&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U17.GPIOA&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U17.GPIOA&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U17.GPIOA&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U17.GPIOA&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U17.GPIOA&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U17.GPIOA&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U17.GPIOA&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='7';UxxBuffor[8]='A';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U17.GPIOB == 0xFF){
		UxxBuffor[6]='7';UxxBuffor[8]='B';
		UxxBuffor[10]=(uint8_t)((U17.GPIOB&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U17.GPIOB&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U17.GPIOB&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U17.GPIOB&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U17.GPIOB&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U17.GPIOB&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U17.GPIOB&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U17.GPIOB&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='7';UxxBuffor[8]='B';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U18.GPIOA == 0xFF){
		UxxBuffor[6]='8';UxxBuffor[8]='A';
		UxxBuffor[10]=(uint8_t)((U18.GPIOA&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U18.GPIOA&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U18.GPIOA&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U18.GPIOA&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U18.GPIOA&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U18.GPIOA&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U18.GPIOA&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U18.GPIOA&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='8';UxxBuffor[8]='A';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;
	//if(U18.GPIOB == 0xFF){
		UxxBuffor[6]='8';UxxBuffor[8]='B';
		UxxBuffor[10]=(uint8_t)((U18.GPIOB&0x80)>>7)+48;UxxBuffor[11]=(uint8_t)((U18.GPIOB&0x40)>>6)+48;
		UxxBuffor[12]=(uint8_t)((U18.GPIOB&0x20)>>5)+48;UxxBuffor[13]=(uint8_t)((U18.GPIOB&0x10)>>4)+48;
		UxxBuffor[14]=(uint8_t)((U18.GPIOB&0x08)>>3)+48;UxxBuffor[15]=(uint8_t)((U18.GPIOB&0x04)>>2)+48;
		UxxBuffor[16]=(uint8_t)((U18.GPIOB&0x02)>>1)+48;UxxBuffor[17]=(uint8_t)((U18.GPIOB&0x01))+48;
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	/*}else{
		UxxBuffor[6]='8';UxxBuffor[8]='B';UxxBuffor[10]='E';UxxBuffor[11]='R';UxxBuffor[12]='R';UxxBuffor[13]='O';UxxBuffor[14]='R';
		USBD_Write(0x81, UxxBuffor, sizeof(UxxBuffor), dataSentCallback);
	}*/
	Timer1_Configure(500); while(clock == 0); clock=0;

	//Send Ack

	return 0;
}

uint8_t Hardware_TEST_PORTA()
{
	uint8_t ui8_USB_BUFFOR[15] = "TT_G0100000000";

	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0xFF);Timer1_Configure(500); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);Timer1_Configure(500); while(clock == 0); clock=0;//Czytaj gpio

	ui8_USB_BUFFOR[6] =((U18.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U18.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U18.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U18.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U18.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U18.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U18.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= 'p';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_10_CLEAR();
	Timer1_Configure(2500); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);Timer1_Configure(500); while(clock == 0); clock=0;
	ui8_USB_BUFFOR[6] =((U18.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U18.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U18.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U18.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U18.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U18.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U18.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]='n';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_10_SET_CS();
	Timer1_Configure(2500); while(clock == 0); clock=0;
	U18.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U18);Timer1_Configure(500); while(clock == 0); clock=0;

	ui8_USB_BUFFOR[6] =((U18.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U18.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U18.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U18.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U18.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U18.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U18.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= 'w';

	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);
	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U18,0x01);Timer1_Configure(500); while(clock == 0); clock=0;//Domyslne ustawienia

	return 0;
}
uint8_t Hardware_TEST_PORTB()
{
	return 0;
}
uint8_t Hardware_TEST_RDY()
{
	uint8_t ui8_USB_BUFFOR[15] = "TT_G0300000000";

	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U17,0xFF);Timer1_Configure(500); while(clock == 0); clock=0;
	U17.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U17);Timer1_Configure(500); while(clock == 0); clock=0;//Czytaj gpio

	ui8_USB_BUFFOR[6] =((U17.GPIOA & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U17.GPIOA & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U17.GPIOA & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U17.GPIOA & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U17.GPIOA & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U17.GPIOA & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U17.GPIOA & 0x02)>>1)+48;ui8_USB_BUFFOR[13]='p';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_1_SET_RDY();
	Timer1_Configure(2500);while(clock == 0); clock=0;
	U17.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U17);Timer1_Configure(500); while(clock == 0); clock=0;//Czytaj gpio

	ui8_USB_BUFFOR[6] =((U17.GPIOA & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U17.GPIOA & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U17.GPIOA & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U17.GPIOA & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U17.GPIOA & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U17.GPIOA & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U17.GPIOA & 0x02)>>1)+48;ui8_USB_BUFFOR[13]='n';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_1_CLEAR();
	Timer1_Configure(2500);while(clock == 0); clock=0;
	U17.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U17);Timer1_Configure(500); while(clock == 0); clock=0;//Czytaj gpio

	ui8_USB_BUFFOR[6] =((U17.GPIOA & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U17.GPIOA & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U17.GPIOA & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U17.GPIOA & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U17.GPIOA & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U17.GPIOA & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U17.GPIOA & 0x02)>>1)+48;ui8_USB_BUFFOR[13]='w';

	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);
	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U17,0x01);Timer1_Configure(500); while(clock == 0); clock=0;//Domyslne ustawienia
	return 0;
}
uint8_t Hardware_TEST_POMIAR()
{
	uint8_t ui8_USB_BUFFOR[15] = "TT_G0400000000";

	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U17,0xFF);	Timer1_Configure(500); while(clock == 0); clock=0;

	U17.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U17);Timer1_Configure(500); while(clock == 0); clock=0; //Czytaj gpio

	ui8_USB_BUFFOR[6] =((U17.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U17.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U17.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U17.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U17.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U17.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U17.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]='p';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_1_CLEAR();

//	Timer1_Configure(2500); while(clock == 0); clock=0;
	U17.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U17);	Timer1_Configure(500); while(clock == 0); clock=0;//Czytaj gpio
	ui8_USB_BUFFOR[6] =((U17.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U17.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U17.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U17.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U17.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U17.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U17.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= 'w';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_1_SET_POMIAR();
	Timer1_Configure(2500); while(clock == 0); clock=0;
	U17.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U17);Timer1_Configure(500); while(clock == 0); clock=0; //Czytaj gpio

	ui8_USB_BUFFOR[6] =((U17.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U17.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U17.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U17.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U17.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U17.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U17.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= 'n';

	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);
	Timer1_Configure(500); while(clock == 0); clock=0;

	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U17,0x01);Timer1_Configure(500); while(clock == 0); clock=0;//Domyslne ustawienia
	return 0;
}
uint8_t Hardware_TEST_PWR()
{
	uint8_t ui8_USB_BUFFOR[15] = "TT_G0500000000";

	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U10,0xFF);Timer1_Configure(500); while(clock == 0); clock=0;
	U10.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U10);Timer1_Configure(500); while(clock == 0); clock=0; //Czytaj gpio
	ui8_USB_BUFFOR[6] =((U10.GPIOA & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U10.GPIOA & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U10.GPIOA & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U10.GPIOA & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U10.GPIOA & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U10.GPIOA & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U10.GPIOA & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= 'p';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_2_SET_PWR();
	//Timer1_Configure(2500); while(clock == 0); clock=0;
	U10.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U10);Timer1_Configure(500); while(clock == 0); clock=0; //Czytaj gpio
	ui8_USB_BUFFOR[6] =((U10.GPIOA & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U10.GPIOA & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U10.GPIOA & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U10.GPIOA & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U10.GPIOA & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U10.GPIOA & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U10.GPIOA & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= (U10.GPIOA & 0x01);
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_2_CLEAR();

	//Timer1_Configure(2500); while(clock == 0); clock=0;
	U10.GPIOA=MCP23017_Read_Register(MCP23017_GPIOA,Address_MCP23017_U10);Timer1_Configure(500); while(clock == 0); clock=0; //Czytaj gpio

	ui8_USB_BUFFOR[6] =((U10.GPIOA & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U10.GPIOA & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U10.GPIOA & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U10.GPIOA & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U10.GPIOA & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U10.GPIOA & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U10.GPIOA & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= (U10.GPIOA & 0x01);
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U17,0x01);Timer1_Configure(500); while(clock == 0); clock=0;	//Domyslne ustawienia
	return 0;
}
uint8_t Hardware_TEST_ZER()
{
	uint8_t ui8_USB_BUFFOR[15] = "TT_G0600000000";

	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U10,0xFF);Timer1_Configure(500); while(clock == 0); clock=0;
	U10.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U10);Timer1_Configure(500); while(clock == 0); clock=0; //Czytaj gpio
	ui8_USB_BUFFOR[6] =((U10.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U10.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U10.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U10.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U10.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U10.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U10.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= 'p';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_2_CLEAR();
	Timer1_Configure(2500); while(clock == 0); clock=0;
	U10.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U10);Timer1_Configure(500); while(clock == 0); clock=0;//Czytaj gpio
	ui8_USB_BUFFOR[6] =((U10.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U10.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U10.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U10.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U10.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U10.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U10.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]= 'w';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Hardware_Modbus_Send_GPIO_2_SET_ZER();

	Timer1_Configure(2500); while(clock == 0); clock=0;
	U10.GPIOB=MCP23017_Read_Register(MCP23017_GPIOB,Address_MCP23017_U10);Timer1_Configure(500); while(clock == 0); clock=0; //Czytaj gpio
	ui8_USB_BUFFOR[6] =((U10.GPIOB & 0x80)>>7)+48;ui8_USB_BUFFOR[7] =((U10.GPIOB & 0x40)>>6)+48;ui8_USB_BUFFOR[8] =((U10.GPIOB & 0x20)>>5)+48;
	ui8_USB_BUFFOR[9] =((U10.GPIOB & 0x10)>>4)+48;ui8_USB_BUFFOR[10]=((U10.GPIOB & 0x08)>>3)+48;ui8_USB_BUFFOR[11]=((U10.GPIOB & 0x04)>>2)+48;
	ui8_USB_BUFFOR[12]=((U10.GPIOB & 0x02)>>1)+48;ui8_USB_BUFFOR[13]='n';
	USBD_Write(0x81, ui8_USB_BUFFOR, sizeof(ui8_USB_BUFFOR), dataSentCallback);

	Timer1_Configure(500); while(clock == 0); clock=0;
	MCP23017_Write_Register(MCP23017_IODIRA,Address_MCP23017_U17,0x01);Timer1_Configure(500); while(clock == 0); clock=0; 	//Domyslne ustawienia
	return 0;
}
uint8_t Hardware_TEST_RESTART()
{
	RS485_MM_W_Restart();
	return 0;
}
void Hardware_Modbus_Send_GPIO_1_SET_RDY()
{
	//98 10 EA 7C 00 03 06 00 01 00 FE 00 FE 53 A7
	//98 10 EA 7C 00 03 CS CS
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
 	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x53;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0xA7;while(!(((UART1->STATUS)&0x20)>>5));//CS
	for(uint16_t i=0;i<2000;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<5000;i++);
	//Timer1_Configure(5000);
	while((RxBuffor.i < RxBuffor.idane));	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;//clock=0;
}
void Hardware_Modbus_Send_GPIO_1_CLEAR()
{
	//98 10 EA 7C 00 03 06 00 01 FF FF 00 00 B3 F3
	//98 10 EA 7C 00 03 68 01
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
 	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xFF;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFF;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xB3;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0xF3;while(!(((UART1->STATUS)&0x20)>>5));//CS
	//for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<500;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
void Hardware_Modbus_Send_GPIO_1_SET_POMIAR()
{
	//98 10 EA 7C 00 03 06 00 01 FE 00 FE 00 C2 5F
	//98 10 EA 7C 00 03 68 01
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
 	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xC2;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0x5F;while(!(((UART1->STATUS)&0x20)>>5));//CS
	//for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<500;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
void Hardware_Modbus_Send_GPIO_2_SET_PWR()
{
	//98 10 EA 7C 00 03 06 00 02 00 FE 00 FE 17 A7
	//98 10 EA 7C 00 03 68 01
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x17;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0xA7;while(!(((UART1->STATUS)&0x20)>>5));//CS
	for(uint16_t i=0;i<2000;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
void Hardware_Modbus_Send_GPIO_2_CLEAR()
{
	//98 10 EA 7C 00 03 06 00 02 FF FF 00 00 F7 F3
	//98 10 EA 7C 00 03 68 01
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xFF;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFF;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xF7;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0xF3;while(!(((UART1->STATUS)&0x20)>>5));//CS
	for(uint16_t i=0;i<2000;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
void Hardware_Modbus_Send_GPIO_2_SET_ZER()
{
	//98 10 EA 7C 00 03 06 00 02 FE 00 FE 00 86 5F
	//98 10 EA 7C 00 03 68 01
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x86;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0x5F;while(!(((UART1->STATUS)&0x20)>>5));//CS
	for(uint16_t i=0;i<2000;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
void Hardware_Modbus_Send_GPIO_10_SET_CS()
{
	//98 10 EA 7C 00 03 06 00 0A 00 FE 00 FE F6 66
	//98 10 EA 7C 00 03 68 01
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x0A;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0xF6;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0x66;while(!(((UART1->STATUS)&0x20)>>5));//CS
	for(uint16_t i=0;i<2000;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
void Hardware_Modbus_Send_GPIO_10_CLEAR()
{
	//98 10 EA 7C 00 03 06 00 0A 00 FE 00 00 77 E6
	//98 10 EA 7C 00 03 68 01
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//Addres 152
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji 16
	UART1->TXDATA =0xEA;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0x7C;while(!(((UART1->STATUS)&0x20)>>5));//reestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x06;while(!(((UART1->STATUS)&0x20)>>5));//licznik bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x0A;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xFE;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//wartosc
	UART1->TXDATA =0x77;while(!(((UART1->STATUS)&0x20)>>5));//CS
	UART1->TXDATA =0xE6;while(!(((UART1->STATUS)&0x20)>>5));//CS
	for(uint16_t i=0;i<2000;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
