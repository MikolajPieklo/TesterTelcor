

// Bits
#define		 ADS1247_WAKEUP 0x00
#define		 ADS1247_SLEEP  0x02
#define		 ADS1247_SYNC	0x4 //
#define		 ADS1247_RESET  0x6 //Reset device
#define		 ADS1247_RDATA  0x12 //Read Data
#define		 ADS1247_RDATAC 0x14 //Read Data in countinuous
#define 	 ADS1247_SDATAC 0x16 //Read Data stop
#define		 ADS1247_RREG   0x20
#define 	 ADS1247_WREG   0x40
#define 	 ADS1247_SYSOCAL   0x60
#define 	 ADS1247_SYSGCAL   0x61
#define 	 ADS1247_SELFOCAL  0x62

// Register
#define		 ADS1247_R_MUX0 	0x00
#define      ADS1247_R_VBIAS 	0x01
#define      ADS1247_R_MUX1  	0x02
#define      ADS1247_R_SYS0  	0x03
#define  	 ADS1247_R_OFC0  	0x04
#define 	 ADS1247_R_OFC1  	0x05
#define 	 ADS1247_R_OFC2  	0x06
#define 	 ADS1247_R_FSC0  	0x07
#define 	 ADS1247_R_FSC1  	0x08
#define 	 ADS1247_R_FSC2  	0x09
#define 	 ADS1247_R_IDAC0 	0x0A
#define 	 ADS1247_R_IDAC1 	0x0B
#define 	 ADS1247_R_GPIOCFG  0x0C
#define 	 ADS1247_R_GPIOCDIR 0x0D
#define 	 ADS1247_R_GPIODAT  0x0E
