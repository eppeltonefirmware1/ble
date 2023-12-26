//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dIOCtrl.h"
//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppBlockDailyLS.h"
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\Appbilling.h"
#include "..\\Include\\AppOneSecondTask.h"

//-------- include dlms specific files-------------------
#include "..\\..\\rlDlms\\meter_app\\r_dlms_data_meter.h"

const uint16_t gbltotaldays[]={LS_TOTAL_DAYS,LS_TOTAL_DAYS/2};
/*--------------------------------------------------------------------------*/
void Load_Survey(void)
{   
	uint8_t length=0;
	uint16_t loc;
	uint16_t ls_day_length=24*ls_period*LS_EVENT_LENGTH+4; 
		
	loc=LOAD_SURVEY_LOC+ (InsSave.DayCount%gbltotaldays[ls_period/3])*ls_day_length;
	if(InsSave.IntCount)
		loc=loc+InsSave.IntCount*LS_EVENT_LENGTH+4;
		
	length=makeLsEventBuffer(RxTxBuffer);
	write_eeprom(RxTxBuffer,loc,length);
	
		
	if(InsSave.IntCount>=24*ls_period)
	{
		logDaily00hhEnergy();
	}

}

/*--------------------------------------------------------------------------*/
uint16_t GetLsLoadLoc(uint8_t dayno)
{ 
	uint16_t addr;
	uint16_t ls_day_length=24*ls_period*LS_EVENT_LENGTH+4; 

	if(InsSave.DayCount>=gbltotaldays[ls_period/3])
	{
		if(InsSave.IntCount==0)
			addr=InsSave.DayCount%gbltotaldays[ls_period/3];
		else
			addr=InsSave.DayCount%gbltotaldays[ls_period/3]+1;
	}
	else
		addr=0;

	addr=addr+dayno;
	addr=addr%gbltotaldays[ls_period/3];

	addr=LOAD_SURVEY_LOC+addr*ls_day_length; 
	return addr; 
}
/*--------------------------------------------------------------------------*/
uint16_t GetDailyLoc(uint16_t dayno)
{ 
	uint16_t addr;


	if(InsSave.DailyDayCount>DAILYLS_TOTAL_DAYS)
		addr=InsSave.DailyDayCount%DAILYLS_TOTAL_DAYS;
	else
		addr=0;
	
	

	addr=addr+dayno;
	addr=addr%DAILYLS_TOTAL_DAYS;

	addr=DAILY_KWH_LOC+addr*LS_DAILY_LENGTH; 

	return addr; 
	
}

/*--------------------------------------------------------------------------*/
void doLs(void)
{
	uint32_t backuptimeCounter =prevTimeCounter; 
	if((InsSave.DailyDayCount==0)&&((InsSave.IntCount==0)))
		prevTimeCounter=InsSave.timeCounter-InsSave.timeCounter%86400;

	
	prevTimeCounter=prevTimeCounter-(prevTimeCounter%(3600/ls_period));
	
	while(InsSave.timeCounter/(3600/ls_period)!=prevTimeCounter/(3600/ls_period))
	{

		Load_Survey();
		
	}
	
	prevTimeCounter=backuptimeCounter;
	
}

uint16_t getLsEntries(void)
{
	
uint16_t entries;


	if(InsSave.DayCount>=gbltotaldays[ls_period/3])
	{
		if(InsSave.IntCount>0)
			entries=gbltotaldays[ls_period/3]-1;
		else
			entries=gbltotaldays[ls_period/3];
	}
	else
		entries=InsSave.DayCount;

	entries=entries*24*ls_period+InsSave.IntCount;

	return entries;

}


void ClearLsData(void)
{
	uint8_t lsDayno;
	
//	uint32_t backuplsAvgV=InsSave.lsAvg900V;
//	uint32_t backupPC=InsSave.lsAvg900PC;
//	uint32_t backNC=InsSave.lsAvg900NC;
//	uint32_t backC=InsSave.lsAvg900C;
//	uint32_t backTem=InsSave.lsAvgT;

//	uint16_t energykWh=InsSave.lskWhPulseCounter;
//	uint16_t energykVAh=InsSave.lskVAhPulseCounter;
	
	
	fillComBufferZero();

	for(lsDayno=0;lsDayno<LS_TOTAL_DAYS;lsDayno++ )
		write_eeprom((uint8_t *)&RxTxBuffer ,LOAD_SURVEY_LOC+lsDayno*64,64);

	prevTimeCounter=InsSave.timeCounter-(InsSave.timeCounter%DAY_CROSSOVER_SECONDS);
	InsSave.IntCount=0;
	InsSave.DayCount=0;
	SaveEEPROM(PARA_WRITE_BEFORE_STATE ,PARA_TYPE_INSTANT);	
	ClearlsAvgValues();
	
	InsSave.lskWhPulseCounter=0;
	InsSave.lskVAhPulseCounter=0;
	
	doLs(); 

//	InsSave.lsAvgV=backuplsAvgV;
//	InsSave.lsAvgPC=backupPC;
//	InsSave.lsAvgNC=backNC;
//	InsSave.lsAvgC=backC;
//	InsSave.lsAvgT=backTem;
//	InsSave.lskWhPulseCounter=energykWh;
//	InsSave.lskVAhPulseCounter=energykVAh;
	
	prevTimeCounter=InsSave.timeCounter;
	SaveEEPROM(PARA_WRITE_AFTER_STATE  ,PARA_TYPE_INSTANT); 
	SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_PFAIL_INSTANT);
	R_OBIS_Class07_BlockloadUpdate();	
}

void getByRange(uint8_t obj_id, uint32_t time_start, uint32_t time_end, uint32_t *idx_start, uint32_t *idx_stop)
{
	uint32_t start=0;
	uint32_t period;
	uint16_t loc = 0,baseloc=LOAD_SURVEY_LOC;
	uint8_t entryavailable=0;
	uint16_t totaldays=gbltotaldays[ls_period/3];
	uint16_t day_length=24*ls_period*LS_EVENT_LENGTH+4; 
	
	uint16_t startEntry=0,totalEntry=getLsEntries();
	
	*idx_start=0;
	*idx_stop=0;
	
	if(obj_id == 2)
	{
		loc = GetLsLoadLoc(0); // oldest day location
		period=3600/ls_period;
		

		
	}
	else if(obj_id == 4)
	{
		loc = GetDailyLoc(0);	// oldest day location
		period=86400;
		baseloc=DAILY_KWH_LOC;
		totaldays=DAILYLS_TOTAL_DAYS;
		day_length=LS_DAILY_LENGTH;
		if(InsSave.DailyDayCount>DAILYLS_TOTAL_DAYS)
			totalEntry=DAILYLS_TOTAL_DAYS;
		else
			totalEntry=InsSave.DailyDayCount;
		
		
	}
	read_eeprom((uint8_t*) &start, loc, 4);	// oldest day date-time
	time_start=time_start-time_start%period;
	time_end=time_end-time_end%period;

	
	for(startEntry=0;startEntry<totalEntry;startEntry++)
	{
		
		if((start>=time_start)&&(entryavailable==0))
		{
			*idx_start=startEntry+1;
			entryavailable=1;
		}
		if((start<=time_end)&&(entryavailable))
		{
			*idx_stop=startEntry+1;
			
		}
		if(start%86400==0)
		{
			loc=loc+day_length;
			if(loc>=(baseloc+totaldays*day_length))
				loc=baseloc;
			read_eeprom((uint8_t*) &start, loc, 4);	// oldest day date-time
		}
		else
		{
			if(obj_id==2)
				start=start+period;
		}
	}
	
	if(*idx_stop<*idx_start)
	{
		*idx_stop=0;
		*idx_start=0;
	}
}
void logDaily00hhEnergy(void)
{
	uint16_t loc;
	loc=DAILY_KWH_LOC+(InsSave.DailyDayCount%DAILYLS_TOTAL_DAYS)*LS_DAILY_LENGTH;
	prevTimeCounter=InsSave.timeCounter-(InsSave.timeCounter%DAY_CROSSOVER_SECONDS);
	makeByte(prevTimeCounter,0,4);
	makeByte(InsSave.CumkWh ,4,4);
	makeByte(InsSave.CumkVAh ,8,4);
	if(getNextBillingDate()==0)
	{
		read_eeprom(RxTxBuffer+12,GetBillLoc(0)+12,2);
		read_eeprom(RxTxBuffer+14,GetBillLoc(0)+18,2);
	}
	else
	{
		makeByte(InsSave.BillMD.Current.kW,12,2);
		makeByte(InsSave.BillMD.Current.kVA,14,2);
	}
	
	makeByte(InsSave.daykWMD,16,2);
	makeByte(InsSave.daykVAMD,18,2);
	write_eeprom(RxTxBuffer,loc,LS_DAILY_LENGTH);
	InsSave.DayCount++;
	InsSave.IntCount=0;
	InsSave.DailyDayCount++;
	InsSave.daykWMD=0;
	InsSave.daykVAMD=0;
	
}
void getAvgLsValues(void)
{
	uint16_t ls_no_of_slots=4/ls_period;
	InsSave.lsAvg900PC=InsSave.lsAvg900PC/ls_no_of_slots;
	InsSave.lsAvg900NC=InsSave.lsAvg900NC/ls_no_of_slots;
	InsSave.lsAvg900C=InsSave.lsAvg900C/ls_no_of_slots;
	InsSave.lsAvg900V=InsSave.lsAvg900V/ls_no_of_slots;
	getAvgTem();
	InsSave.lsAvgT=InsSave.lsAvgT/10;
	
	
}
void ClearlsAvgValues(void)
{
	InsSave.lsAvg900V=0;
	InsSave.lsAvg900PC=0;
	InsSave.lsAvg900NC=0;
	InsSave.lsAvg900C=0;
	InsSave.lsAvgT=0;
	
}
uint8_t makeLsEventBuffer(uint8_t *lsbptr)
{
	uint8_t lslength=0;

	#if (defined(IS_EXPORT_METER) && (IS_EXPORT_METER == 1))
	uint16_t energyExpkWh=InsSave.ExpkWhCounter/320;
	#endif
	uint64_t lsAVal;
		

	
	prevTimeCounter=prevTimeCounter+(3600/ls_period );
	if(InsSave.IntCount==0)
	{
		lsbptr[lslength++]=prevTimeCounter;
		lsbptr[lslength++]=prevTimeCounter>>8;
		lsbptr[lslength++]=prevTimeCounter>>16;
		lsbptr[lslength++]=prevTimeCounter>>24;
		
	}

	
	getAvgLsValues();
	InsSave.lskVAhPulseCounter=checkkWh2kVAh(InsSave.lskWhPulseCounter,InsSave.lskVAhPulseCounter);
	
	getDailyMd(InsSave.lskWhPulseCounter,InsSave.lskVAhPulseCounter);
	lsAVal=InsSave.lskVAhPulseCounter;		//16 bits
	lsAVal<<=10;							//10 bits
	lsAVal|=InsSave.lsAvg900PC&0x03FF;	
	lsAVal<<=10;							//10 bits
	lsAVal|=InsSave.lsAvg900NC&0x03FF;		
	lsAVal<<=10;							//10 bits
	lsAVal|=InsSave.lsAvg900C&0x03FF;		
	lsAVal<<=10;							//10 bits
	lsAVal|=InsSave.lsAvgT&0x03FF;		
	

	// avg voltage, kWh , kVAh,Effective I/Ph Current,Neu Current
	lsbptr[lslength++]=InsSave.lsAvg900V;
	lsbptr[lslength++]=InsSave.lskWhPulseCounter;		//8 bits
	lsbptr[lslength++]=InsSave.lskWhPulseCounter>>8;	//8 bits
	lsbptr[lslength++]=lsAVal;
	lsbptr[lslength++]=lsAVal>>8;
	lsbptr[lslength++]=lsAVal>>16;
	lsbptr[lslength++]=lsAVal>>24;
	lsbptr[lslength++]=lsAVal>>32;
	lsbptr[lslength++]=lsAVal>>40;
	lsbptr[lslength++]=lsAVal>>48;
	
	InsSave.IntCount++;	
	
	InsSave.lskWhPulseCounter=0;
	InsSave.lskVAhPulseCounter=0;
		
	InsSave.MonthPowerOnDuration+=InsSave.PowerOn30;
	InsSave.CumPowerOnDuration+=InsSave.PowerOn30;
	ClearlsAvgValues();	
	InsSave.PowerOn30=0;
	
	return lslength;
}
void getDailyMd(uint16_t energykWh,uint16_t energykVAh)
{
	if((energykWh*ls_period) >InsSave.daykWMD)
		InsSave.daykWMD=(energykWh*ls_period);
	if((energykVAh*ls_period) >InsSave.daykVAMD)
		InsSave.daykVAMD=(energykVAh*ls_period);
	
}

