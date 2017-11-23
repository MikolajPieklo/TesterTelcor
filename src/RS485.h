#include "em_usart.h"
#include "em_cmu.h"
#include "em_gpio.h"
//#include "MCP23017.h"
//#include <stdbool.h>

#ifndef SRC_RS485_H_
#define SRC_RS485_H_

#define COM_PORT gpioPortE // USART location #1: PD0 and PD1
#define UART_TX_pin 2      // PE2
#define UART_RX_pin 3      // PE3
#define BUFFERSIZE          256

void RS485_Init(void);
uint16_t ModRTU_CRC(uint8_t buf[]);
void UART1_RX_IRQHandler(void);
void UART1_TX_IRQHandler(void);

uint8_t RS485_MM_R00(void); //Odczyt stanu 10000/10001/10002
void RS485_MM_R01(void);     //Ustawienia domyslne

void RS485_MM_R11(void); // Producent
void RS485_MM_R12(void); // Urzadzenie

void RS485_MM_W_Restart(void); //Restart
void RS485_MM_WykonajPomiar(void); //Wykonaj pomiary - komenda 110

void RS485_MM_K01(void); //Wykonaj pomiar na kanale 1
void RS485_MM_K02(void); //Wykonaj pomiar na kanale 2
void RS485_MM_K03(void); //Wykonaj pomiar na kanale 3
void RS485_MM_K04(void); //Wykonaj pomiar na kanale 4
void RS485_MM_K05(void); //Wykonaj pomiar na kanale 5

volatile struct Buffor
{
	uint8_t Buffor[11];
	uint8_t i;
	uint8_t idane;
	uint8_t znak;
	uint8_t ansi;
}RxBuffor, TxBuffor;

#endif /* SRC_RS485_H_ */
