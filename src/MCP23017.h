/*
 * Pin Reset -> High
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <em_i2c.h>

#define Address_MCP23017_U10 0x44
#define Address_MCP23017_U22 0x42
#define Address_MCP23017_U30 0x4C
#define Address_MCP23017_U17 0x40
#define Address_MCP23017_U18 0x4E


// registers BANK=1
#define MCP23017_IODIRA   0x00
#define MCP23017_IODIRB   0x01
#define MCP23017_IPOLA 	  0x02
#define MCP23017_IPOLB    0x03
#define MCP23017_GPINTENA 0x04
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALA  0x06
#define MCP23017_DEFVALB  0x07
#define MCP23017_INTCONA  0x08
#define MCP23017_INTCONB  0x09
#define MCP23017_IOCONA   0x0A
#define MCP23017_IOCONB   0x0B
#define MCP23017_GPPUA    0x0C
#define MCP23017_GPPUB    0x0D
#define MCP23017_INTFA    0x0E
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPA  0x10
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOA    0x12
#define MCP23017_GPIOB    0x13
#define MCP23017_OLATA    0x14
#define MCP23017_OLATB    0x15

#define MCP23017_INT_ERR 255


// Makra do konfiguracji I/O

#define _GPIOXO  0x00  // reset all pin to output function

#define _GPIOA0I 0x01
#define _GPIOA1I 0x02
#define _GPIOA2I 0x04
#define _GPIOA3I 0x08
#define _GPIOA4I 0x10
#define _GPIOA5I 0x20
#define _GPIOA6I 0x40
#define _GPIOA7I 0x80

#define _GPIOB0I 0x01
#define _GPIOB1I 0x02
#define _GPIOB2I 0x04
#define _GPIOB3I 0x08
#define _GPIOB4I 0x10
#define _GPIOB5I 0x20
#define _GPIOB6I 0x40
#define _GPIOB7I 0x80

// Makra do konfiguracji stanow

#define _GPIOXC  0x00  // reset all pin to output function

#define _GPIOA0S 0x01
#define _GPIOA1S 0x02
#define _GPIOA2S 0x04
#define _GPIOA3S 0x08
#define _GPIOA4S 0x10
#define _GPIOA5S 0x20
#define _GPIOA6S 0x40
#define _GPIOA7S 0x80

#define _GPIOB0S 0x01
#define _GPIOB1S 0x02
#define _GPIOB2S 0x04
#define _GPIOB3S 0x08
#define _GPIOB4S 0x10
#define _GPIOB5S 0x20
#define _GPIOB6S 0x40
#define _GPIOB7S 0x80


  I2C_TransferSeq_TypeDef i2cTransfert;
  I2C_TransferReturn_TypeDef ret;

void MCP23017_Init_I2C(void);
/*
 * Funkcje
 */

  void MCP23017_Configure_Default(uint8_t add);

  uint8_t MCP23017_Write_Register(uint8_t reg, uint8_t address,uint8_t value);
  uint8_t MCP23017_Read_Register(uint8_t reg, uint8_t address);


  struct Uklad
  {
   uint8_t IOCON;  //konfiguracja
   uint8_t IODIRA; //1(INPUT) 0(OUTPUT)
   uint8_t IODIRB;
   uint8_t GPPUA;  //1(PULLUP) 0()
   uint8_t GPPUB;
   uint8_t GPIOA;  //1(High) 0(Low)
   uint8_t GPIOB;
   uint8_t OLATA;  //1(High) 0(Low)
   uint8_t OLATB;
  }U10,U17,U18,U22,U30;

