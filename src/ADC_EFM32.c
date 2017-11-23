#include "ADC_EFM32.h"
#include "MCP23017.h"
#include "tester.h"


void Init_ADC_EFM(void)
{
	CMU_ClockEnable(cmuClock_ADC0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	ADC_InitSingle_TypeDef single_init = ADC_INITSINGLE_DEFAULT;
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;

	init.prescale =  ADC_PrescaleCalc(384000, 0); // 384KHz sampling / 4 cycle per sample + 8 cycle decoding = 32KHz
	init.timebase = ADC_TimebaseCalc(0);

	init.warmUpMode = adcWarmupKeepADCWarm;

	ADC_Init(ADC0, &init);


	single_init.acqTime = adcAcqTime4; //Acquisition time (in ADC clock cycles)
	single_init.input = adcSingleInpCh7; //Sample input selection, use single ended or differential input according to setting of diff.
	single_init.reference = adcRef1V25; //Sample reference selection.
	single_init.resolution = adcRes12Bit; //Sample resolution.
	//single_init.rep = true; //Select if continuous conversion until explicit stop.                        // auto rearm
	ADC_InitSingle(ADC0, &single_init);


	//CMU_ClockEnable(cmuClock_ADC0, false);
}

float ADC_Conversation (uint8_t PIN1, uint8_t PIN2)
{
	/*
	 * POM_VZAS2 B0/B3
	 * POM_VZAS1 B2/B4
	 * I_TC      ZAL_POM_I_TC/ZAL_POMVZAS_I
	 *
	 */
	int sample_ptr = 0;
	uint16_t samples[SAMPLE_SIZE];
	MCP23017_Write_Register(MCP23017_IODIRB,Address_MCP23017_U30,0x00); //wyjscia
	MCP23017_Write_Register(MCP23017_OLATB,Address_MCP23017_U30,PIN1|PIN2);   // zalacz pomiar
	while ( sample_ptr < SAMPLE_SIZE ) {
		ADC_Start(ADC0, adcStartSingle);
		while (((ADC0->STATUS)&0x01 != 0));	// Active wait for ADC to complete
		samples[sample_ptr] = ADC_DataSingleGet(ADC0);
		sample_ptr++;
	}
	return (float)(1.25*((samples[0]+samples[1]+samples[2]+samples[3]+samples[4])/SAMPLE_SIZE)/4095);
}
