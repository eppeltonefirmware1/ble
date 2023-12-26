//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dIOCtrl.h"
#include "..\\..\\rlDevice\\Include\\dRtc.h"
#include "..\\..\\rlDevice\\Include\\dSARAdc.h"

//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppMacros.h"
#include "..\\Include\\AppScheduleAction.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppBilling.h"
#include "..\\Include\\AppMD.h"
#include "..\\Include\\AppBlockDailyLS.h"
#include "..\\Include\\AppTampers.h"
#include "..\\Include\\AppIntervalkWh.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppTOD.h"
#include "..\\Include\\AppOneSecondTask.h"



//----------------dlms specific files---------------------------------
#include "..\\..\\rlDlms\\meter_app\\r_dlms_data_meter.h"				/* DLMS Data Definitions */
void updatePfailEvent(void);
void isSetBleID(void);

void OneSecondTask(void)
{
	uint8_t task=0;
	uint8_t clear_nm_clock=0;
	
	uint32_t nowkWh=InsSave.CumkWh+InsSave.ZkWhCounter/METER_CONSTANT;

	InsSave.timeCounter++;
		
	isRAMOK();	
	
	if(mcu_flag&POWER_STABLE_FLAG)
	{
		InsSave.lsAvgV=InsSave.lsAvgV+Ins.Voltage;
		InsSave.lsAvgC=InsSave.lsAvgC+Ins.EffectiveI;
		InsSave.lsAvgNC=InsSave.lsAvgNC+Ins.NeuCurrent;
		InsSave.lsAvgPC=InsSave.lsAvgPC+Ins.PhCurrent;
		InsSave.lsAvgT=InsSave.lsAvgT+InsTemp;
		InsSave.PowerOn30++;
	}
		
	if((((nowkWh-prekWh) >=NORMAL_KWH_THRESHOLD_LL)&&((nowkWh-prekWh) <=NORMAL_KWH_THRESHOLD_UL))||(((nowkWh-prekWhinterval) >=NORMAL_KWH_THRESHOLD_LL)&&((nowkWh-prekWhinterval) <=NORMAL_KWH_THRESHOLD_UL)))
	{
		savekWh(nowkWh);
		prekWh=nowkWh;
		prekWhinterval=nowkWh;
		
	}
		
	getMD_LSPulseCounter();
	if(((mcu_flag&POWER_STABLE_FLAG)==0)&&isPowerfailLog(USER_CODE))
		task|=RECORD_POWER_FAIL_DATA;
		
	if(isIntevalCrossed(300))
		getRtcCounter(RTC_NO_CHECK_MODE);
	
	if(isIntevalCrossed(3600/md_period))
		task|=RECORD_MD_DATA;

	if(isIntevalCrossed(3600/ls_period))
		task|=RECORD_LS_DATA;
  
	if(isIntevalCrossed(86400UL))
	{
		if(getNextBillingDate()&&(InsSave.DailyDayCount>0||InsSave.IntCount>0))
		{
			task|=RECORD_BILL_DATA;
		}
		
		write_eeprom((uint8_t *)&clear_nm_clock,BAT_NM_LOCK_COUNT_LOC,1);
	}
	
	if(mcu_flag & SINGLE_ACTION_CHECK_FLAG )
	{
		task|=doBillSingleAction();
	}
		
	if(mcu_flag & PASSIVE_CAL_CHECK_FLAG )
	{
		task|=doCalanderAction();
	}
	if(isIntevalCrossed(30))
		doSARMeasurement();
		
	if(task)
	{
		mcu_flag &= ~COMM_RECEIVE_ENABLE;
		SaveEEPROM(PARA_WRITE_BEFORE_STATE ,PARA_TYPE_INSTANT); // save at before location when MD, LS or month over
	}
	
	if(task&RECORD_MD_DATA)
		Compute_MD(BILL_TYPE_AUTO);
	else if(task&RECORD_MAN_MD_DATA)
		Compute_MD(BILL_TYPE_MAN);

	

	if(new_md_period!=md_period)
	{
		
		if((((InsSave.timeCounter%3600)%1800)/(3600/new_md_period))==(((InsSave.timeCounter%3600)%1800)/(3600/md_period)))
		{
			md_period=updateMDInterval(MD_PERIOD_CHANGE_FLAG,md_period);
		}
	}		
	
	if(task&RECORD_LS_DATA)
	{
		doLs();
		if((task&RECORD_BILL_DATA)==0)
		{
			if(isIntevalCrossed(86400UL))
			{
				if(getNextBillingDate()&&(InsSave.DailyDayCount>0||InsSave.IntCount>0))
				{
					task|=RECORD_BILL_DATA;
				}
			}		
		}
	}
	
	if(task&RECORD_BILL_DATA)
		CheckBillingOver(BILL_TYPE_AUTO,getNextBillingDate());
	
	if(task&RECORD_DO_BILL_DATA)
	{
		Compute_MD(BILL_TYPE_MAN);
		CheckBillingOver(BILL_TYPE_MAN,InsSave.timeCounter );
		getRtcCounter(RTC_NO_CHECK_MODE);
	}

	if(task&RECORD_POWER_FAIL_DATA)
	{
		powerfailLogging();
		updatePfailEvent();
	}
	
	prevTimeCounter=InsSave.timeCounter;
	
	if(task)
	{
		R_OBIS_Class07_BlockloadUpdate();
		SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_PFAIL_INSTANT); // save at current location
		SaveEEPROM(PARA_WRITE_AFTER_STATE  ,PARA_TYPE_INSTANT); // save at after location
		
		mcu_flag |= COMM_RECEIVE_ENABLE;
		
	}
	
	
	Tamperlog();		
	
	
	//-------------------------------------
	CheckParaTimeOut();
	Display(USER_CODE);
	//-------------------------------------
	
	
	if((mcu_flag&POWER_STABLE_FLAG)==0)
	{
		if(powerStableCounter<5)
		{
			powerStableCounter++;
			if(powerStableCounter==5)
			{
				SaveEEPROM(PARA_WRITE_BEFORE_STATE ,PARA_TYPE_INSTANT); // save at before location when MD, LS or month over
				
			}
				
		}
		else
		{
			power_fail_state|=SAVE_LEVEL1_KWH_STATE_OPEN;			
			mcu_flag|=POWER_STABLE_FLAG;
			powerStableCounter=0;
			SWITCH_ON_RTC_VDD;
			SaveEEPROM(PARA_WRITE_AFTER_STATE  ,PARA_TYPE_INSTANT); // save at after location
			SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_PFAIL_INSTANT); // save at current location
			
		}
	}
	else 
	{
		if((power_fail_state&SAVE_LEVEL1_KWH_STATE_OPEN)==0)
		{
			if(powerStableCounter<10)
				powerStableCounter++;
			else
			{
				power_fail_state|=SAVE_LEVEL2_KWH_STATE_OPEN;
				powerStableCounter=0;
			}
		}
		
	}
		
	isRAMOK();	
	getSetPfailData(ENERGY_W_RAM);
	//isSetBleID();
}	
void getMD_LSPulseCounter(void)
{
	uint16_t energykWh;
	uint16_t energykVAh;	
	
	if((InsSave.timeCounter/900)!=(prevTimeCounter /900))
	{
		energykWh=InsSave.kWhCounter10m/METER_CONSTANT;
		energykVAh=InsSave.kVAhCounter10m/METER_CONSTANT;
	
		InsSave.mdkWhPulseCounter+=energykWh;
		InsSave.lskWhPulseCounter+=energykWh;
		InsSave.mdkVAhPulseCounter+=energykVAh;
		InsSave.lskVAhPulseCounter+=energykVAh;
		InsSave.kWhCounter10m=InsSave.kWhCounter10m-(uint32_t)energykWh*METER_CONSTANT;
		InsSave.kVAhCounter10m=InsSave.kVAhCounter10m-(uint32_t)energykVAh*METER_CONSTANT;	
		
		InsSave.lsAvg900PC=InsSave.lsAvg900PC+InsSave.lsAvgPC/90000;
		InsSave.lsAvg900NC=InsSave.lsAvg900NC+InsSave.lsAvgNC/90000;
		InsSave.lsAvg900C=InsSave.lsAvg900C+InsSave.lsAvgC/90000;
		InsSave.lsAvg900V=InsSave.lsAvg900V+InsSave.lsAvgV/180000;
		InsSave.lsAvgPC=0;
		InsSave.lsAvgNC=0;
		InsSave.lsAvgC=0;
		InsSave.lsAvgV=0;
	
	}
}
uint16_t checkkWh2kVAh(uint16_t blkkwh,uint16_t blkkVAh)
{
	if(blkkwh>blkkVAh)
		blkkVAh=blkkwh;
	return blkkVAh;
}