//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dUser_includes.h"
#include "..\\..\\rlDevice\\Include\\dI2c.h"
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
#include "..\\Include\\AppNMTask.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppTOD.h"
#include "..\\Include\\AppOneSecondTask.h"


static uint8_t doNMLsRAM(void);
//----------------dlms specific files---------------------------------
#include "..\\..\\rlDlms\\meter_app\\r_dlms_data_meter.h"				/* DLMS Data Definitions */
#define MAX_LS_LENGTH	(LS_EVENT_LENGTH*48+5)
#define LS_EVENT_LOG_TIME	48
uint8_t lsbuff[MAX_LS_LENGTH];
uint16_t nm_mode_ls_length;
const uint16_t nmgbltotaldays[]={LS_TOTAL_DAYS,LS_TOTAL_DAYS/2};

void doDatalog_NM(void)
{
	uint8_t task=0;
	

	isRAMOK();
	getRtcCounter(RTC_CHECK_MODE);
	

	getMD_LSPulseCounter();
	if(isIntevalCrossed(3600/md_period))
		task|=RECORD_MD_DATA;
  
	if(isIntevalCrossed(86400UL))
	{
		if((InsSave.DailyDayCount>0)&&getNextBillingDate())
		{
			task|=RECORD_BILL_DATA;
		}
	}
	
	if(mcu_flag & SINGLE_ACTION_CHECK_FLAG )
	{
		task|=doBillSingleAction();
	}
		
	if(mcu_flag & PASSIVE_CAL_CHECK_FLAG )
	{
		task|=doCalanderAction();
	}		
		
	if(task)
	{
		mcu_flag &= ~COMM_RECEIVE_ENABLE;
		mcu_flag|=POWER_STABLE_FLAG;
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
	
	if(isIntevalCrossed(3600/ls_period))
	{
		InsTemp=0;
		task|=doLsNM();
		if((task&RECORD_BILL_DATA)==0)
		{
			if(isIntevalCrossed(86400UL))
			{
				if(getNextBillingDate()&&(InsSave.DailyDayCount>0))
				{
					task|=RECORD_BILL_DATA;
				}
			}		
		}
		//doSARMeasurement();
	}
	
	if(task&RECORD_BILL_DATA)
		CheckBillingOver(BILL_TYPE_AUTO,getNextBillingDate());
	
	if(task&RECORD_DO_BILL_DATA)
	{
		Compute_MD(BILL_TYPE_MAN);
		CheckBillingOver(BILL_TYPE_MAN,InsSave.timeCounter );
	}
	
	
	if(task)
	{
		R_OBIS_Class07_BlockloadUpdate();
		mcu_flag |= COMM_RECEIVE_ENABLE;
	}
	

}
void initNMLs(void)
{
	uint16_t ls_day_length=24*ls_period*LS_EVENT_LENGTH+4; 
	nm_mode_ls_length=0;
	nm_ls_end_index=0;
	ls_nm_start_loc=LOAD_SURVEY_LOC+ (InsSave.DayCount%nmgbltotaldays[ls_period/3])*ls_day_length;
	if(InsSave.IntCount!=0)
		ls_nm_start_loc=ls_nm_start_loc+InsSave.IntCount*LS_EVENT_LENGTH+4;
		
	nm_ls_start_index=getLsEntries();
	
}

uint8_t doLsNM(void)
{
	uint8_t status=0;
	uint32_t backuptimeCounter =prevTimeCounter; 
	if((InsSave.DailyDayCount==0)&&((InsSave.IntCount==0))&&(prevTimeCounter==0))
		prevTimeCounter=InsSave.timeCounter-InsSave.timeCounter%86400;

	
	prevTimeCounter=prevTimeCounter-(prevTimeCounter%(3600/ls_period));
	
	while(InsSave.timeCounter/(3600/ls_period)!=prevTimeCounter/(3600/ls_period))
	{

		status|=doNMLsRAM();
		
	}
	
	prevTimeCounter=backuptimeCounter;
	return status;
}
uint8_t doNMLsRAM(void)
{
	uint8_t status=0;
	uint16_t ls_day_length=24*ls_period*LS_EVENT_LENGTH+4; 
	uint16_t loc=(LOAD_SURVEY_LOC+ls_day_length*nmgbltotaldays[ls_period/3]);
    
	
	if(InsSave.IntCount==0)
	{
		if((nm_mode_ls_length+4)>MAX_LS_LENGTH)
			nm_mode_ls_length=0;
	}
		
	if((nm_mode_ls_length+LS_EVENT_LENGTH)>MAX_LS_LENGTH)
		nm_mode_ls_length=0;
	
	nm_mode_ls_length+=makeLsEventBuffer(lsbuff+nm_mode_ls_length);
	
	if(ls_nm_start_loc>=loc)
	{
		ls_nm_start_loc=LOAD_SURVEY_LOC;
		
	}
	if(InsSave.IntCount==1)
	{
		if(InsSave.DayCount>=nmgbltotaldays[ls_period/3])
			nm_ls_start_index=getLsEntries();
	}

	if((InsSave.IntCount>=24*ls_period)||(InsSave.IntCount>0&&((InsSave.IntCount%LS_EVENT_LOG_TIME)==0)))
	{
		if(InsSave.IntCount>=24*ls_period)
		{
			logDaily00hhEnergy();
			InsSave.timeCounter=prevTimeCounter;
		}
		doNMLsEEPROM();
		SaveEEPROM(PARA_WRITE_AFTER_STATE  ,PARA_TYPE_INSTANT); // save at after location
		SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_PFAIL_INSTANT); // save at current location
		getRtcCounter(RTC_CHECK_MODE);
		nm_ls_end_index=0;
		
	}
	status=RECORD_LS_DATA;
	return status;

}

void doNMLsEEPROM(void)
{

	write_eeprom(lsbuff,ls_nm_start_loc,nm_mode_ls_length%MAX_LS_LENGTH);
	
	ls_nm_start_loc+=nm_mode_ls_length;
	nm_mode_ls_length=0;
	nm_ls_start_index=getLsEntries();
	
}

void saveLSatExit(void)
{
	if(nm_mode_ls_length>0)
		doNMLsEEPROM();
}

void getNMStartDate(uint8_t* buf)
{
	buf[0]=lsbuff[0];
	buf[1]=lsbuff[1];
	buf[2]=lsbuff[2];
	buf[3]=lsbuff[3];
}
void getNMIntervalLS(uint8_t* buf,uint8_t interval)
{
	uint8_t lslength=LS_EVENT_LENGTH;
	
	while(lslength-->0)
		buf[lslength]=lsbuff[interval+lslength];
}