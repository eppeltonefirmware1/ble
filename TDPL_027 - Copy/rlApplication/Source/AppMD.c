//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dI2c.h"
//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppMD.h"
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppTOD.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppOneSecondTask.h"


/****************** Maximum Demand ********************************************/
void Compute_MD(uint8_t reset_option)
{   
		
		uint16_t ZenergykWh=InsSave.ZkWhCounter;
		uint16_t ZenergykVAh=InsSave.ZkVAhCounter;
		//uint16_t ZenergyExpkWh=InsSave.ExpZkWhCounter;
		//uint16_t ZenergyExpkVAh=InsSave.ExpZkVAhCounter;
		
		uint32_t logtimeCounter=prevTimeCounter-(prevTimeCounter%(3600/md_period));
		//uint8_t preSeason=0;
		
		logtimeCounter=logtimeCounter+(3600/md_period);
			
		
		ZenergykWh=ZenergykWh/METER_CONSTANT;
		ZenergykVAh=ZenergykVAh/METER_CONSTANT;
		//ZenergyExpkWh=ZenergyExpkWh/METER_CONSTANT;
		//ZenergyExpkVAh=ZenergyExpkVAh/METER_CONSTANT;
		
		
		
		
		if(ZenergykWh>ZenergykVAh)
		{
			ZenergykVAh=ZenergykWh;
			//InsSave.ZkVAhCounter=InsSave.ZkWhCounter;
		}
		
		
		InsSave.CumkWh+=ZenergykWh;
		//InsSave.ExpCumkWh+=ZenergyExpkWh;
		#if (defined(GUJRAT_METER_0) && (GUJRAT_METER_0 == 1))
			InsSave.TODEnergy.TOD[currentZoneID].kWh+=ZenergyExpkWh;
		#else
			InsSave.TODEnergy.TOD[currentZoneID].kWh+=ZenergykWh;
		#endif
		//if(((logtimeCounter%86400)>=72000)||(((logtimeCounter%86400)<=18000)))
		//	InsSave.NonSolarCumkWh+=ZenergyExpkWh;
		
		
		if(reset_option==BILL_TYPE_AUTO)
		{	
			InsSave.mdkVAhPulseCounter=checkkWh2kVAh(InsSave.mdkWhPulseCounter,InsSave.mdkVAhPulseCounter);

			if((InsSave.mdkWhPulseCounter*md_period) >InsSave.BillMD.Current.kW)
			{
				if(InsSave.BillMD.Current.kW)
				{
					if(InsSave.BillMD.CumkWMD>=InsSave.BillMD.Current.kW)
						InsSave.BillMD.CumkWMD-=InsSave.BillMD.Current.kW;
				}
				
				InsSave.BillMD.Current.kW=InsSave.mdkWhPulseCounter*md_period;
					
				InsSave.BillMD.Current.kW_Date=logtimeCounter;
				InsSave.BillMD.CumkWMD+=InsSave.BillMD.Current.kW;
			}
			
			#if (defined(GUJRAT_METER_0) && (GUJRAT_METER_0 == 1))
			{
				if((ZenergyExpkWh*md_period) >InsSave.TODEnergy.TOD[currentZoneID].TODMD.kW)
				{
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kW=ZenergyExpkWh*md_period;
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kW_Date=logtimeCounter;
				}			
			}
			#else
			{
				if((InsSave.mdkWhPulseCounter*md_period) >InsSave.TODEnergy.TOD[currentZoneID].TODMD.kW)
				{
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kW=InsSave.mdkWhPulseCounter*md_period;
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kW_Date=logtimeCounter;
				}				
			}
			#endif
		}
		
		
		InsSave.CumkVAh+=ZenergykVAh;
		//InsSave.ExpCumkVAh=0;//ZenergyExpkVAh;
		#if (defined(GUJRAT_METER_0) && (GUJRAT_METER_0 == 1))
			InsSave.TODEnergy.TOD[currentZoneID].kVAh+=ZenergyExpkVAh;
		#else
			InsSave.TODEnergy.TOD[currentZoneID].kVAh+=ZenergykVAh;
		#endif
		
		if(reset_option==BILL_TYPE_AUTO)
		{
			if((InsSave.mdkVAhPulseCounter*md_period)>InsSave.BillMD.Current.kVA)
			{
				if(InsSave.BillMD.Current.kVA)
				{
					if(InsSave.BillMD.CumkVAMD>=InsSave.BillMD.Current.kVA)
						InsSave.BillMD.CumkVAMD-=InsSave.BillMD.Current.kVA;
				}				
				InsSave.BillMD.Current.kVA=InsSave.mdkVAhPulseCounter*md_period;
					
				InsSave.BillMD.Current.kVA_Date=logtimeCounter;
				InsSave.BillMD.CumkVAMD+=InsSave.BillMD.Current.kVA;
			}
			
			#if (defined(GUJRAT_METER_0) && (GUJRAT_METER_0 == 1))
			{
				if((ZenergyExpkVAh*md_period)>InsSave.TODEnergy.TOD[currentZoneID].TODMD.kVA)
				{
				
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kVA=ZenergyExpkVAh*md_period;
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kVA_Date=logtimeCounter;
					
				}
			}
			#else
			{
				if((InsSave.mdkVAhPulseCounter*md_period)>InsSave.TODEnergy.TOD[currentZoneID].TODMD.kVA)
				{
				
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kVA=InsSave.mdkVAhPulseCounter*md_period;
					InsSave.TODEnergy.TOD[currentZoneID].TODMD.kVA_Date=logtimeCounter;
					
				}				
			}
			#endif
		}
		
		if(reset_option==BILL_TYPE_AUTO)
		{
			InsSave.mdkWhPulseCounter=0;
			InsSave.mdkVAhPulseCounter=0;
		}
		
		InsSave.ZkWhCounter=InsSave.ZkWhCounter-ZenergykWh*METER_CONSTANT;
		InsSave.ZkVAhCounter=InsSave.ZkVAhCounter-ZenergykVAh*METER_CONSTANT;
		
		//InsSave.ExpZkWhCounter=InsSave.ExpZkWhCounter-ZenergyExpkWh*METER_CONSTANT;
		//InsSave.ExpZkVAhCounter=InsSave.ExpZkVAhCounter-ZenergyExpkVAh*METER_CONSTANT;
		
		
		InsSave.MonthPowerOnDuration+=InsSave.PowerOn30;
		InsSave.CumPowerOnDuration+=InsSave.PowerOn30;
		
		getAvgTem();
		
//		if(InsSave.PowerOn30!=0)
//			InsSave.lsAvgV=InsSave.lsAvgV/InsSave.PowerOn30;
//		if(InsSave.PowerOn30!=0)
//			InsSave.lsAvgC=InsSave.lsAvgC/InsSave.PowerOn30;
//		if(InsSave.PowerOn30!=0)
//			InsSave.lsAvgPC=InsSave.lsAvgPC/InsSave.PowerOn30;
//		if(InsSave.PowerOn30!=0)
//			InsSave.lsAvgNC=InsSave.lsAvgNC/InsSave.PowerOn30;
			
		InsSave.PowerOn30=0;
		currentZoneID=getCurrentZoneNo(InsSave.timeCounter);
		
//	//--------------------------------------------------
//	preSeason=currentSeasonID;
//	currentSeasonID=getCurrentSeason(InsSave.timeCounter);
//	if(preSeason!=currentSeasonID)
//	{
//		currentZoneID=getCurrentZoneNo(InsSave.timeCounter);
//		saveSeasonkWh(0,preSeason);
//		saveSeasonkWh(1,preSeason);
//		saveSeasonkWh(3,currentSeasonID);
//		SaveEEPROM(PARA_WRITE_BEFORE_STATE ,PARA_TYPE_INSTANT); // save at before location when MD, LS or month over			
//	}		
		
}
