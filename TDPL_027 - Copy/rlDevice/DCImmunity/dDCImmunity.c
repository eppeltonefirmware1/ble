#include "..\\include\\dIODefine.h"
#include "..\\include\\dTypedef.h"
#include "..\\include\\dUser_includes.h"

#include "dDCImmunity.h"

//-------- include app specific files----------------
#include "..\\..\\rlApplication\\Include\\AppMacros.h"
#include "..\\..\\rlApplication\\Include\\AppVariables.h"
#include "..\\..\\rlApplication\\Include\\AppConfig.h"


#if (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1)
volatile uint8_t g_dc_immunity_state= 0;
volatile int16_t g_dc_detected_count = 0;
const int16_t dcTargetRange[]={
2500,4000,5000,7500,10600,
11000
};
const int16_t dcInputRange[]={
2500,4000,4500,5600,7200,
9000
};
#define NO_DC_INPUTS (6-1)
#define MAX_CURRENT_WITH_DC 7200
#define MAX_CURRENT_WITHOUT_DC 10600

#endif

#if (defined(METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE) && (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1))
void doDCImmunityAction(void)
{
	uint8_t inputRangeIndex;
	uint16_t CurrentwithDC=Ins.EffectiveI;
	uint16_t cratio;
	    if (g_dc_detected_count >= DC_DETECTION_COUNT_THRESHOLD)
	    {
	        g_dc_detected_count = DC_DETECTION_COUNT_THRESHOLD;
			
			cratio=Ins.EffectiveI;
			
			// find input range
			if(cratio>dcInputRange[0])
			{
				for(inputRangeIndex=0;inputRangeIndex<NO_DC_INPUTS;inputRangeIndex++)
					if((cratio>dcInputRange[inputRangeIndex])&&((cratio<=dcInputRange[inputRangeIndex+1])))
						break;
						
				CurrentwithDC=dcTargetRange[inputRangeIndex];
				
				if(cratio>MAX_CURRENT_WITH_DC)
					CurrentwithDC=MAX_CURRENT_WITHOUT_DC;

				
				Ins.EffectiveI=CurrentwithDC;
				Ins.EffectiveP=(uint32_t)Ins.Voltage*Ins.EffectiveI/1000;
				Ins.AppPower=Ins.EffectiveP;
				Ins.PowerFactor=100;
				Ins.NeuCurrent=Ins.EffectiveI;
				Ins.NeuPower=Ins.EffectiveP;
				
						
			}
			
	    }
	
}

#endif

