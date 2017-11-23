#include "my_timer.h"

void Timer1_Configure(uint16_t scaler)
{
	CMU_ClockEnable(cmuClock_TIMER1, true);   // Enable TIMER0 peripheral clock
	TIMER_Init_TypeDef timer1Init =  //Select TIMER0 parameters
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
		  TIMER_TopSet(TIMER1, scaler * 16);
		  TIMER_Init(TIMER1, &timer1Init);    /* Configure TIMER */
		  TIMER_IntEnable(TIMER1, TIMER_IF_OF); 	  // Enable overflow interrupt
		  NVIC_EnableIRQ(TIMER1_IRQn);     // Enable TIMER0 interrupt vector in NVIC
}
