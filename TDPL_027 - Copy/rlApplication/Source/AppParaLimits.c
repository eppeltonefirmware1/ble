//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dRtc.h"
#include "..\\..\\rlDevice\\Include\\dWatchDog.h"
#include "..\\..\\rlDevice\\Include\\dTypedef.h"
#include "..\\..\\rlDevice\\Include\\dI2c.h"
//-----------------------------------------------------

//-------- include app specific files----------------
#include "..\\include\\AppVariables.h"
#include "..\\include\\AppParaLimits.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppIntervalkWh.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppConfig.h"




//----------------------------------------------------
#define KWH_OVER_LIMIT 300
#define PULSE_COUNTER_LIMIT			32000U			// 20KWH
#define PULSE_COUNTER_LIMIT_MD_LS	320000U			// 20KWH
#define MD_LIMIT					23000			// 23KW

#define LIMIT_KWH			0
#define LIMIT_MD			1
#define RAM_CHECK1			2
#define RAM_CHECK2			3
#define RAM_CHECK3			4
#define RAM_CHECK4			5
#define RAM_CHECK5			6



void islimitOverflow(void)
{
	uint8_t status=1;
	//-----------------kWh---------------------
	
		if(InsSave.CumkWh>prekWh)
			if((InsSave.CumkWh-prekWh)>KWH_OVER_LIMIT)
				status=0;
		if(prekWh>(InsSave.CumkWh+InsSave.ZkWhCounter/METER_CONSTANT))
			if((prekWh-(InsSave.CumkWh+InsSave.ZkWhCounter/METER_CONSTANT))>KWH_OVER_LIMIT)
				status=0;			
		
	//-----------------------------------------
	
	//------------------rtc forward jump-------
	
		//if(InsSave.timeCounter)
		
		
	//-----------------------------------------
	
	//------------------md---------------------
		// imports
		if(InsSave.daykWMD>MD_LIMIT)
			InsSave.daykWMD=0;
		if(InsSave.daykVAMD>MD_LIMIT)
			InsSave.daykVAMD=0;
			
		if(InsSave.lskWhPulseCounter>MD_LIMIT)
			InsSave.lskWhPulseCounter=0;
		if(InsSave.mdkWhPulseCounter>MD_LIMIT)
			InsSave.mdkWhPulseCounter=0;
			
		if(InsSave.lskVAhPulseCounter>MD_LIMIT)
			InsSave.lskVAhPulseCounter=0;				
		if(InsSave.mdkVAhPulseCounter>MD_LIMIT)
			InsSave.mdkVAhPulseCounter=0;						
			
		if(InsSave.BillMD.Current.kW>MD_LIMIT)
			InsSave.BillMD.Current.kW=0;

		if(InsSave.BillMD.Current.kVA>MD_LIMIT)
			InsSave.BillMD.Current.kVA=0;
			
		if(InsSave.kWhCounter10m>PULSE_COUNTER_LIMIT_MD_LS)
			InsSave.kWhCounter10m=0;
		if(InsSave.kVAhCounter10m>PULSE_COUNTER_LIMIT_MD_LS)
			InsSave.kVAhCounter10m=0;
			
		
			
		if(InsSave.ZkWhCounter>PULSE_COUNTER_LIMIT)
			InsSave.ZkWhCounter=0;
			
		if(InsSave.ZkVAhCounter>PULSE_COUNTER_LIMIT)
			InsSave.ZkVAhCounter=0;
			
			
	//-----------------------------------------
	
	
	if(status==0)
		WDT_Restart(0x00);
}

