/*
 * RS485.c
 *
 *  Created on: 1 mar 2017
 *      Author: mikolajp
 */
#include "RS485.h"
#include "my_timer.h"

//loc3

void RS485_Init(void)
{
	CMU_ClockEnable(cmuClock_GPIO, true);            // Enable GPIO peripheral clock
	CMU_ClockEnable(cmuClock_UART1, true);

	GPIO_PinModeSet(COM_PORT, UART_TX_pin, gpioModePushPull, 0); // Configure UART TX pin as digital output, initialize high since UART TX idles high (otherwise glitches can occur)
	GPIO_PinModeSet(COM_PORT, UART_RX_pin, gpioModeInput, 0);    // Configure UART RX pin as input (no filter)

	//UART1->CTRL =(1<<29);
	UART1->CLKDIV = (22 << 6);//dla 152 19200 //dla 625 4800  //dla 22 115200
	//UART1->FRAME = (1<<12)|(5<<3);
	UART1->CMD = (1 << 11) | (1 << 10) | (1 << 2) | (1 << 0);

	UART1->IFC = 0x1FF9;                                      // clear all USART interrupt flags
	UART1->ROUTE = (3<<8)|(1<<1)|(1<<0);
	UART1->IEN = 0x04; 										 // Enable RXDATAV


	NVIC_EnableIRQ(UART1_RX_IRQn);
	NVIC_ClearPendingIRQ(UART1_RX_IRQn);   // Clear pending RX interrupt flag in NVIC
	NVIC_ClearPendingIRQ(UART1_TX_IRQn);   // Clear pending TX interrupt flag in NVIC
	NVIC_EnableIRQ(UART1_RX_IRQn);


	GPIO_PinModeSet(gpioPortC,7,gpioModePushPull,1); //DE
	GPIO_PinModeSet(gpioPortD,8,gpioModePushPull,1); //REn
	//GPIO_PinOutSet(gpioPortD,8);
	//GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.i = 0;
	RxBuffor.idane=0;
	TxBuffor.i = 0;


}

void UART1_RX_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(UART1_TX_IRQn);   // Clear pending TX interrupt flag in NVIC

	RxBuffor.znak=UART1->RXDATA;
	if(RxBuffor.ansi)
	{
		if((RxBuffor.znak>='0'&&RxBuffor.znak<='9') ||
					(RxBuffor.znak>='A'&&RxBuffor.znak<='Z')||
					(RxBuffor.znak>='a'&&RxBuffor.znak<='z'))
		{
				RxBuffor.Buffor[RxBuffor.i] = RxBuffor.znak;RxBuffor.i+=1;
		}
	}
	else
	{
		RxBuffor.Buffor[RxBuffor.i] = RxBuffor.znak;RxBuffor.i+=1;
	}

}

void UART1_TX_IRQHandler(void)
{
	    /* Check TX buffer level status */
	if (USART1->STATUS & UART_STATUS_TXBL)
	{
		/*if (txBuf.pendingBytes > 0)
	    {
	        /* Transmit pending character */
	       /* USART_Tx(USART1, txBuf.data[txBuf.rdI]);
	        txBuf.rdI = (txBuf.rdI + 1) % BUFFERSIZE;
	        txBuf.pendingBytes--;
	    }

	      /* Disable Tx interrupt if no more bytes in queue */
	    /*if (txBuf.pendingBytes == 0)
	    {
	        USART_IntDisable(USART1, UART_IF_TXBL);
	    }*/
	}
}

uint16_t ModRTU_CRC(uint8_t buf[])
{
	uint16_t crc = 0xFFFF;
	uint8_t len = sizeof(buf);

  for (uint8_t pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc

    for (uint8_t i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

uint8_t RS485_MM_R00(void) //Zwroc statusu
{
	uint16_t Rejestr=0;
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.idane=7; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){
	RxBuffor.Buffor[i]='\0';}

	for(uint16_t i=0;i<4000;i++);

	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xe9;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x48;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x73;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	Rejestr=(RxBuffor.Buffor[3]<<8)|(RxBuffor.Buffor[4]);
	return Rejestr;
}

void RS485_MM_R01(void)  //Ustawienia domysle
{
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.idane=7; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){
	RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xe9;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x05;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xfe;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	RxBuffor.i=0;
}

void RS485_MM_R11(void) //Producent
{
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=11;  RxBuffor.ansi=0;//liczba danych powrotnych//51
	for(uint8_t i=0;i<11;i++){
		RxBuffor.Buffor[i]='\0';}

	for(uint16_t i=0;i<4000;i++);
	RxBuffor.i=0;
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x18;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<2000;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<5000;i++);

	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.Buffor[0]='M';RxBuffor.Buffor[1]='R';RxBuffor.Buffor[2]='1';RxBuffor.Buffor[3]='1';

}

void RS485_MM_R12(void)
{
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=6; RxBuffor.ansi=1; //liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){
		RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	RxBuffor.i=0;
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x04;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x59;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xc3;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<5000;i++);

	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)
	RxBuffor.i=0;
}

void RS485_MM_W_Restart(void) //Restart
{
	//98  10  03  e9  00  01  02  00  01  84  3f
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=0; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}

	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));//addres
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));//kod funkcji
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));//rejestr HI 5
	UART1->TXDATA =0xe9;while(!(((UART1->STATUS)&0x20)>>5));//rejestr LO
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));//ilosc bit trans
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));//2*ilosc bit
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));//dane
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));//dane
	UART1->TXDATA =0x84;while(!(((UART1->STATUS)&0x20)>>5));//cs
	UART1->TXDATA =0x3f;while(!(((UART1->STATUS)&0x20)>>5));//cs

	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	//while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}

void RS485_MM_WykonajPomiar(void)
{
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);
	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	//for(uint8_t i=0;i<11;i++){
	//	RxBuffor.Buffor[i]='\0';}

	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x03;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xe9;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x6e;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xc4;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x13;while(!(((UART1->STATUS)&0x20)>>5));

	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	for(uint16_t i=0;i<2000;i++);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}

void RS485_MM_K01(void) //Wykonaj pomiar kanal 1
{
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x05;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x2b;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xf2;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xdd;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;

}

void RS485_MM_K02(void) //Wykonaj pomiar kanal 2
{
	//"98  10  05  2b  00  01  02  00  02  b2  dc  "
	//"98  10  05  2b  00  01  6c  c4 "
	//"98  03  05  2b  00  01  e9  07  "
	//"98  03  02  00  02  25  99  "
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x05;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x2b;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xb2;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xdc;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}

void RS485_MM_K03(void) //Wykonaj pomiar kanal 3
{
	//"98  10  05  2b  00  01  02  00  04  32  de  "
	//"98  10  05  2b  00  01  6c  c4  "
	//"98  03  05  2b  00  01  e9  07  "
	//"98  03  02  00  04  a5  9b  "
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x05;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x2b;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x04;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x32;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xde;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}

void RS485_MM_K04(void) //Wykonaj pomiar kanal 4
{
	 //"98  10  05  2b  00  01  02  00  08  32  db  "
	 //"98  10  05  2b  00  01  6c  c4  "
	 //"98  03  05  2b  00  01  e9  07  "
	 //"98  03  02  00  08  a5  9e  "
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x05;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x2b;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x08;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x32;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xdb;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}

void RS485_MM_K05(void) //Wykonaj pomiar kanal 5
{
	//"98  10  05  2b  00  01  02  00  10  32  d1  "
	//"98  10  05  2b  00  01  6c  c4  "
	//"98  03  05  2b  00  01  e9  07  "
	//"98  03  02  00  10  a5  94  "
	GPIO_PinOutSet(gpioPortD,8);	GPIO_PinOutSet(gpioPortC,7);

	RxBuffor.idane=8; RxBuffor.ansi=0;//liczba danych powrotnych
	for(uint8_t i=0;i<11;i++){RxBuffor.Buffor[i]='\0';}
	for(uint16_t i=0;i<4000;i++);
	UART1->TXDATA =0x98;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x05;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x2b;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x01;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x02;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x00;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x10;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0x32;while(!(((UART1->STATUS)&0x20)>>5));
	UART1->TXDATA =0xd1;while(!(((UART1->STATUS)&0x20)>>5));
	for(uint16_t i=0;i<500;i++);
	GPIO_PinOutClear(gpioPortC,7);	GPIO_PinOutClear(gpioPortD,8);
	while(RxBuffor.i < RxBuffor.idane);	if(RxBuffor.i == RxBuffor.idane)RxBuffor.i=0;
	RxBuffor.i=0;
}
