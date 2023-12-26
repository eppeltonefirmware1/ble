#include "..\\include\\dVBat.h"
#include "..\\include\\dIODefine.h"
#include "..\\include\\dTypedef.h"
#include "..\\include\\dUser_includes.h"
#include "..\\include\\dUart.h"
#include "..\\include\\dDisplay.h"
#include "..\\include\\dIOCtrl.h"
#include "..\\include\\d8bitTimer.h"
#include "..\\include\\dWatchdog.h"
#include "..\\include\\dDeltaSigmaADC.h"
#include "..\\include\\dMisc.h"
#include "..\\include\\dSARAdc.h"


//-------- include app specific files----------------
#include "..\\..\\rlApplication\\Include\\AppVariables.h"
#include "..\\..\\rlApplication\\Include\\AppPowerOnInit.h"
#include "..\\..\\rlApplication\\Include\\AppMisc.h"




void EnableVBATOperation(void)
{

//	BUPCTL1 = 0x80;
	
//	VBATEN=0U;
//	VBATEN=1U;
//	VBATIE=1U;
//	VBATCMPM=1;
//	VBATIS=0U;
//	GDIDIS=0;
//	VBATSEL=0;
//	VBAIF = 0;
//	VBAMK = 1;
//	VBAPR1 = 1;
//    VBAPR0 = 1;
	
	
	BUPCTL1 = 0x00;
	
	
	
}
void shutDownPheripheral(void)
{
	ITMC&=~0x80;	// stop 12 bit timer
	TMKAEN=0;
	//P3 &= ~0x08;//i2c
	R_ADC_Stop();
	StopTemperature();
	Uart0_Stop();
	Uart2_Stop();
	//SCOC=0;
	//LCDM0 &= ~_40_LCD_VOLTAGE_MODE_INTERNAL;
	//LCDM0 |= _80_LCD_VOLTAGE_MODE_CAPACITOR;
	//LCDC0 	= _03_LCD_CLOCK_FSX_FIL_4;	
	//SCOC=1;
	
	R_LCD_PowerOff();
	ADPC = 0x01;
	Ins.Frequency=0x00;
	Ins.Voltage=0;
	CheckCurrentLimitNm();
	Ins.EffectiveP=0;
	Ins.PowerFactor=0;
	Ins.LeadLag=0;
	Ins.PhPower=0;
	Ins.NeuPower=0;
	Ins.AppPower=0;
	display_flag=0;
	
	SET_MAG_OFF_VCC
	PFSEG0=0xF0;
	PFSEG1=0xFF;
	
	
	if((power_fail_state&RTC_BAT_STATUS_FLAG)==0)
		SWITCH_OFF_RTC_VDD;	
		
	SWITCH_OFF_PULSE_LED;
	
	
}
void switch2Low_Power_MODE(uint8_t low_mode)
{
	
	
	switch(low_mode)
	{
		case LOW_POWER_HALT:
			HALT();
			break;
		case LOW_POWER_STOP:
			NOP();
			NOP();
			NOP();
			STOP();
			NOP();
			NOP();
			NOP();
			break;
	}
}

//#pragma interrupt Interrupt_VBATT(vect=INTVBAT)
//static void Interrupt_VBATT(void)
//{
	
//}

