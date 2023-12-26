//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dIOCtrl.h"
#include "..\\..\\rlDevice\\Include\\dDeltaSigmaADC.h"
#include "..\\..\\rlDevice\\Include\\dMisc.h"
#include "..\\..\\rlDevice\\Include\\dWatchDog.h"
#include "..\\..\\rlDevice\\Include\\d12bitTimer.h"
#include "..\\..\\rlDevice\\Include\\dSARAdc.h"
#include "..\\..\\rlDevice\\DCImmunity\\dDCImmunity.h"
#include "..\\..\\rlDevice\\DCImmunity\\rl78_sw_dc_correction.h"


//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppTampers.h"
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppCalibration.h"

//----------------dlms specific files---------------------------------
#include "..\\..\\rlDlms\\meter_app\\r_dlms_data_meter.h"				/* DLMS Data Definitions */

#define MAG_WAIT_TIME	5
#define MAG_HOLD_TIME	60
#define PERMANENT_MAG_EXIT_TIME	60

uint16_t TAMPER_EVENT_OCC_TIME[MAX_TAMPER_COUNT]={1800	,1800,120,60,60	,1800,600,60,1800,1800};
uint16_t TAMPER_EVENT_REC_TIME[MAX_TAMPER_COUNT]={120	,120,112,60,120	,120,120,5,120,120};

//uint16_t TAMPER_EVENT_OCC_TIME[MAX_TAMPER_COUNT]={30,30,30,30,30,30,30,30,30,30};
//uint16_t TAMPER_EVENT_REC_TIME[MAX_TAMPER_COUNT]={30,30,30,30,30,30,30,5,30,30};

//--------------Event Code for DLMS-----------------
const uint8_t dlmsEventCode[MAX_TAMPER_COUNT][2]={
{207	,208},		// NM
{67		,68},		// OL
{201	,202},		// MAG
{51		,52},		// REV
{203	,204},		// ND
{69		,70},		// EARTH
{81		,82},		// OU mismatch
{251	,0xFF},		// C-oPEN
{9		,10},		// Low U
{83		,84},		// Temperature
};
//--------------------------------------------------
/*********************** Check tamper and display *****************************/
//0-Current Related,1-Other,2-Non Rollover
const uint8_t EventCountIndex[MAX_TAMPER_COUNT]={
1,
0,
1,
0,
1,
0,
1,
2,
3,
1
};

const uint8_t EventLen[TAMPER_GROUP_COUNT]={
  60,								// 0- current Related
  100,								// Other
	5,								// NOn Rollover
	25,								// Voltage related
};

const uint16_t startAdd[TAMPER_GROUP_COUNT]={
  TAMPER_C_RELATED_LOC,
  TAMPER_OTHER_LOC,
  TAMPER_NON_ROLLOVER_LOC,
  TAMPER_U_RELATED_LOC,
};
const uint16_t TAMPER_SNAP_LOC[MAX_TAMPER_COUNT]={
TAMPER_0,	
TAMPER_1,
TAMPER_2,
TAMPER_3,
TAMPER_4,
TAMPER_5,
TAMPER_6,
TAMPER_7,
TAMPER_8,
TAMPER_9
};


static uint8_t mag_wait_down_time=0;

uint8_t Check_Tamper(void)
{   
	
	//uint32_t itemp=0;
	uint8_t checkOffset=0;
	uint8_t status=0;
	//uint8_t preND=display_flag;
	
   
	display_flag=0;
	CurrentTamperStatusFlag=0;
	
	formfactor=(1000UL*peakVoltage)/Ins.Voltage;	

	
	if((TamperRecord.TamperStatusFlag & TAMPER_COVEROPEN)==0)// if cover open is not there
	{
		while(GET_COVER_STATUS)	// check cover open pin 
		{
			if(TamperTime_counter[7]++>=COVER_OPEN_WAIT_TIME)
			{
				CurrentTamperStatusFlag |= TAMPER_COVEROPEN;
				break;
			}
		}		
	}
	else if(TamperRecord.TamperCount[7]>=5)
		CurrentTamperStatusFlag |= TAMPER_COVEROPEN;
	else if((TamperRecord.TamperStatusFlag & TAMPER_COVEROPEN)&&GET_COVER_STATUS)
		CurrentTamperStatusFlag |= TAMPER_COVEROPEN;
		
	if(InsTemp>TEMPERATURE_U_THD)
	{
		if((TamperRecord.TamperStatusFlag&TAMPER_TEMP)==0)
			TamperTime_counter[9]=TAMPER_EVENT_OCC_TIME[9];
			
		CurrentTamperStatusFlag |= TAMPER_TEMP;
	}
		
		
	if(Ins.Voltage>COM_THRESHOLD_VOLTAGE)
	{
		if(GET_COM_STATUS==0)
			SWITCH_ON_COMM_VCC;
	}
	else
	{
		if(GET_COM_STATUS)
			SWITCH_OFF_COMM_VCC;
	}
	
	#if (defined(POWER_BASE_EARTH_DETECT) && (POWER_BASE_EARTH_DETECT == 1))	// EARTH TAMPER
		if(Ins.EffectiveP  > MIN_TAMPER_DETECTION_POWER) // MIN DETECTION POWER IS 100 WATTS  
		{  						
			if(Ins.NeuPower > Ins.PhPower)            
				itemp=Ins.PhPower;  
			else
				itemp=Ins.NeuPower; 

			if((((Ins.EffectiveP -itemp )*100)/Ins.EffectiveP) >= EARTH_TAMPER_THRESHOLD)// 5%
			{
				CurrentTamperStatusFlag |= TAMPER_EARTH;
				display_flag|=DISP_EARTH;
			}
			else
			{
				if((CurrentTamperStatusFlag&TAMPER_EARTH)==0)
					if((mcu_flag&POWER_STABLE_FLAG)&&(Ins.Voltage<FORCE_POWER_FAIL_THRESHOLD))
						power_fail_state|=FORCE_POWER_FAIL;
			}
			
		}			
	#else // this one is used
		if(Ins.EffectiveI  >=EARTH_TAMPER_I_THRESHOLD)   // 50 ma 
		{  				
			if(isEarth(EARTH_TAMPER_THRESHOLD)==1)
			{
				CurrentTamperStatusFlag |= TAMPER_EARTH;
				display_flag|=DISP_EARTH;
				
			}
			else
			{
				if((CurrentTamperStatusFlag&TAMPER_EARTH)==0)
					if((mcu_flag&POWER_STABLE_FLAG)&&(Ins.Voltage<FORCE_POWER_FAIL_THRESHOLD))
						power_fail_state|=FORCE_POWER_FAIL;
			}
		}
		if(Ins.NeuCurrent>Ins.PhCurrent)
		{
			if(Ins.NeuCurrent>CUR_MISMATCH_TAMPER_I_THRESHOLD)
			{
				if(isEarth(CUR_MISMATCH_TAMPER_THRESHOLD)==1)
					CurrentTamperStatusFlag |= TAMPER_OU;
			}
		}
		if(Ins.EffectiveI>OVER_LOAD_THRESHOLD)
			CurrentTamperStatusFlag |= TAMPER_OVER_LOAD;
		
			
	#endif	

	//reverse tamper
	if(Ins.PhPower>MIN_TAMPER_DETECTION_POWER)
	{
		if(mcu_flag&PH_POWER_REV)
		{
			if((Ins.PhPower==Ins.EffectiveP)||(Ins.PhPower>10000))
			{
				#if (defined(GUJRAT_METER_0) && (GUJRAT_METER_0 == 0))
					CurrentTamperStatusFlag |= TAMPER_REV;
				#endif
				display_flag|=DISP_REV;
			}
			else
				mcu_flag&=~PH_POWER_REV;
		}
	}
	else
		mcu_flag&=~PH_POWER_REV;
	
		
	if(Ins.NeuPower>MIN_TAMPER_DETECTION_POWER)
	{
		if(mcu_flag&NU_POWER_REV)
		{
			if((Ins.NeuPower==Ins.EffectiveP)||(Ins.NeuPower>10000))
			{
				#if (defined(GUJRAT_METER_0) && (GUJRAT_METER_0 == 0))
					CurrentTamperStatusFlag |= TAMPER_REV;
				#endif
				display_flag|=DISP_REV;
			}
			else
				mcu_flag&=~NU_POWER_REV;
		}
	}
	else
		mcu_flag&=~NU_POWER_REV;

	if(CurrentTamperStatusFlag & TAMPER_EARTH)
	{
		if(Ins.Voltage<NM_THRESHOLD_VOLTAGE)
		{
			if(((10UL*peakVoltage)>(178UL*Ins.Voltage/100))&&(Ins.Voltage>100))
				CurrentTamperStatusFlag |= TAMPER_AC_DC;
			else
			{
				CurrentTamperStatusFlag |=TAMPER_NEU_MISS;
				display_flag&=~DISP_EARTH;
			}
		}
		else
		{
			if((Ins.Voltage>(REF_U_FOR_METER+MIN_DROP_U_TAMPER))||(Ins.Voltage<(REF_U_FOR_METER-MIN_DROP_U_TAMPER)))
				if(Ins.EffectiveI>950)
					if(((10UL*peakVoltage)>(162UL*Ins.Voltage/100))&&(Ins.Voltage>100))
						CurrentTamperStatusFlag |= TAMPER_AC_DC;
			
			#if (defined(METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE) && (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1))
				if(g_dc_immunity_state==0)
				{
					if(((Ins.Voltage>(REF_U_FOR_METER+MIN_DROP_U_TAMPER))&&(Ins.PowerFactor<95)&&(Ins.LeadLag==1))&&((display_flag & DISP_REV)==0))
					{
						if(((1000UL*peakVoltage)/Ins.Voltage)>143)
						{
							checkOffset=1;
						}
						
					}
				}
			#endif
			if(Ins.Frequency<4700||Ins.Frequency>5300)
			{
				CurrentTamperStatusFlag |= TAMPER_AC_DC;
			}
			else if(((Ins.Voltage<(REF_U_FOR_METER-MIN_DROP_U_TAMPER))||(checkOffset==1))&&((display_flag & DISP_REV)==0))
			{
				#if (defined(METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE) && (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1))
					if(g_dc_immunity_state==0)
					{
						#if (defined(RCD_TAMPER_TERMINAL3) && (RCD_TAMPER_TERMINAL3 == 1))
							if(Ins.PhCurrent<12000)
							{
								if((Ins.PhCurrent>=950)&&(Ins.NeuCurrent>(3+rlCalib.I2OffSet))&&((Ins.NeuCurrent<=1000)&&(Ins.NeuPower<MIN_TAMPER_DETECTION_POWER)))
								{
									CurrentTamperStatusFlag |= TAMPER_AC_DC;		
								}
							}
						#endif
						#if (defined(RCD_TAMPER_TERMINAL4) && (RCD_TAMPER_TERMINAL4 == 1))
						{
							if((Ins.PhPower<MIN_TAMPER_DETECTION_POWER)&&((Ins.PowerFactor>55)||(checkOffset==1)))
							{
								if((Ins.NeuCurrent>=950)&&(Ins.PhCurrent<80)&&(Ins.NeuCurrent<12000))
								{
									if(Ins.PhCurrent>10)
										CurrentTamperStatusFlag |= TAMPER_AC_DC;
										
								}
							}
									
						}
						#endif
					}
				#endif
				
			}
			if((CurrentTamperStatusFlag & TAMPER_AC_DC)==0)
			{
				if(((Ins.Voltage<(REF_U_FOR_METER-MIN_DROP_U_TAMPER))||(Ins.Voltage>(REF_U_FOR_METER+MIN_DROP_U_TAMPER)))&&(Ins.EffectiveI>950))
				{
					if(((1000UL*peakVoltage)/Ins.Voltage)<135)
					{
						CurrentTamperStatusFlag |= TAMPER_AC_DC;
					}
				}			
			}

		}
	}
	else
	{
		#if (defined(METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE) && (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1))
			if(g_dc_immunity_state==0)
			{
				if((Ins.Voltage<(REF_U_FOR_METER-MIN_DROP_U_TAMPER))&&(Ins.EffectiveI>950))
				{
					if(((1000UL*peakVoltage)/Ins.Voltage)>145)
					{
						CurrentTamperStatusFlag |= TAMPER_AC_DC;
						display_flag|=DISP_EARTH;
					}
				}
				else if((Ins.Voltage>34700)&&(Ins.EffectiveI>950))
				{
					CurrentTamperStatusFlag |= TAMPER_AC_DC;
					display_flag|=DISP_EARTH;
				}
			}
		#endif		
		
	}

	
	if(((mcu_flag&POWER_STABLE_FLAG)==0)&&(((TamperRecord.TamperStatusFlag&TAMPER_MAG)&&(IS_PRODUCTION_MODE==0))==0))
	{
		permanent_mag_wait_time=0;
		mcu_flag&=~IS_DC_MAGNET;
		doSARMeasurement();
	}
	else
	{
			
		if((Ins.Voltage>12000)&&(GET_MAG_STATUS||(mcu_flag&IS_DC_MAGNET)))
		{
			mag_wait_down_time=MAG_WAIT_TIME;
			permanent_mag_wait_time++;
			if(permanent_mag_wait_time>PERMANENT_MAG_EXIT_TIME)
				permanent_mag_wait_time=PERMANENT_MAG_EXIT_TIME+MAG_WAIT_TIME;
			mcu_flag&=~IS_DC_MAGNET;	
			//permanent_mag_wait_time=0;
			
		}
		else
		{
			if(mag_wait_down_time>0)
				mag_wait_down_time--;
			
			if(permanent_mag_wait_time>0)
				permanent_mag_wait_time--;
		}
			
			
		if(mag_wait_down_time>0)
		{
			
			CurrentTamperStatusFlag&=~(TAMPER_EARTH|TAMPER_REV|TAMPER_NEU_MISS|TAMPER_AC_DC);
			CurrentTamperStatusFlag |=TamperRecord.TamperStatusFlag;
			CurrentTamperStatusFlag&=~(TAMPER_NEU_MISS|TAMPER_AC_DC);
			
			
			mag_hold_time++;
		
			mcu_flag&=~EXPORT_MODE;
		
			if(mag_hold_time>5)
				display_flag|=DISP_MAG;
				
			CurrentTamperStatusFlag&=~(TAMPER_EARTH|TAMPER_REV|TAMPER_NEU_MISS|TAMPER_AC_DC);
			display_flag&=~(DISP_EARTH|DISP_REV|DISP_NM);
			mcu_flag&=~NU_POWER_REV;
			mcu_flag&=~PH_POWER_REV;
		}
		else
		{
			mag_hold_time=0;
			cum_mag_toggle_count=0;
			display_flag&=~DISP_MAG;
			permanent_mag_wait_time=0;
			mcu_flag&=~IS_DC_MAGNET;
			if((TamperRecord.TamperStatusFlag&TAMPER_MAG)&&(IS_PRODUCTION_MODE==0))
			{
				display_flag&=~(DISP_EARTH|DISP_REV|DISP_NM);
				mcu_flag&=~EXPORT_MODE;
				display_flag|=DISP_MAG;
				//Ins.PhCurrent=IMAX_CURRENT;
				//Ins.NeuCurrent=IMAX_CURRENT;
				//Ins.EffectiveI=IMAX_CURRENT;
				//Ins.PhPower=IMAX_POWER;
				//Ins.NeuPower=IMAX_POWER;
				Ins.EffectiveP=IMAX_POWER;
				Ins.AppPower=IMAX_POWER;
				//mcu_flag|=IS_DC_MAGNET;
			}
		}
			
		if((mag_hold_time>MAG_HOLD_TIME)||(permanent_mag_wait_time>PERMANENT_MAG_HOLD_TIME))
		{
			
			if((cum_mag_toggle_count>3)||(permanent_mag_wait_time>PERMANENT_MAG_HOLD_TIME))
			{
				cum_mag_toggle_count=4;
				mag_hold_time=MAG_HOLD_TIME;
				TamperTime_counter[2]=0xFFFE;
				CurrentTamperStatusFlag&=~(TAMPER_EARTH|TAMPER_REV|TAMPER_NEU_MISS|TAMPER_AC_DC);
				display_flag&=~(DISP_EARTH|DISP_REV|DISP_NM);
				mcu_flag&=~EXPORT_MODE;
				CurrentTamperStatusFlag|=TAMPER_MAG;
				display_flag|=DISP_MAG;
				Ins.PhCurrent=IMAX_CURRENT;
				Ins.NeuCurrent=IMAX_CURRENT;
				Ins.EffectiveI=IMAX_CURRENT;
				Ins.PhPower=IMAX_POWER;
				Ins.NeuPower=IMAX_POWER;
				Ins.EffectiveP=IMAX_POWER;
				Ins.AppPower=IMAX_POWER;
				Ins.PowerFactor=100;
			}
		}
		else
		{
			if(((mag_wait_down_time==0))&&((CurrentTamperStatusFlag&TAMPER_NEU_MISS)||(CurrentTamperStatusFlag & TAMPER_AC_DC)))
			{
				CurrentTamperStatusFlag&=~TAMPER_EARTH;
				
				if((CurrentTamperStatusFlag&TAMPER_NEU_MISS)||(Ins.Voltage<ND_RUN_AT_REF_THRESHOLD_U))
				{
					apparentPulsePower=REFERENCEVOLTAGE*Ins.EffectiveI;
					activePulsePower=apparentPulsePower;
					status=1;
				}
				display_flag|=DISP_NM;
				CurrentTamperStatusFlag |=TamperRecord.TamperStatusFlag;
				
			}
			else if(Ins.EffectiveI<EARTH_TAMPER_I_THRESHOLD)
			{
				CurrentTamperStatusFlag |=(TamperRecord.TamperStatusFlag&(TAMPER_EARTH|TAMPER_REV|TAMPER_AC_DC));
			}
			
		}
	}
	
	#if (defined(GUJRAT_METER_0) && (GUJRAT_METER_0 == 1))
	{
		if(Ins.EffectiveP>STARTING_POWER_THRESHOLD_L)
		{
			if(((display_flag&DISP_REV)&&((mcu_flag&EXPORT_MODE)==0))||(((display_flag&DISP_REV)==0)&&(mcu_flag&EXPORT_MODE)))
			{
				ClearPulseAccumulation();
				if(display_flag&DISP_REV)
					mcu_flag|=EXPORT_MODE;
				else
					mcu_flag&=~EXPORT_MODE;
			}	
		}
		
	}
	#endif
	
	if((mcu_flag&POWER_STABLE_FLAG)==0)
		display_flag=0x00;
	
	if(TamperRecord.TamperStatusFlag&TAMPER_NEU_MISS)	
		if((CurrentTamperStatusFlag&TAMPER_NEU_MISS)==0)
			if(Ins.Voltage>19000)	
				CurrentTamperStatusFlag&=~TAMPER_NEU_MISS;
			else
				CurrentTamperStatusFlag|=TAMPER_NEU_MISS;
				
	if(TamperRecord.TamperStatusFlag&TAMPER_AC_DC)
		if((CurrentTamperStatusFlag&TAMPER_AC_DC)==0)
			if((Ins.Voltage<27600)&&(Ins.EffectiveI>(REF_I_FOR_METER*10/100)))
				CurrentTamperStatusFlag&=~TAMPER_AC_DC;
			else
				CurrentTamperStatusFlag|=TAMPER_AC_DC;
	if(TamperRecord.TamperStatusFlag&TAMPER_LOW_U)	
		if((CurrentTamperStatusFlag&TAMPER_LOW_U)==0)
			if((Ins.Voltage>19200)&&(Ins.EffectiveI>(REF_I_FOR_METER*2/100)))
				CurrentTamperStatusFlag&=~TAMPER_LOW_U;
			else
				CurrentTamperStatusFlag|=TAMPER_LOW_U;
				
	if((Ins.Voltage<LOW_VOLTAGE_THRESHOLD)&&(Ins.EffectiveI>(REF_I_FOR_METER*2/100)))	
		CurrentTamperStatusFlag|=TAMPER_LOW_U;
		
	if(TamperRecord.TamperStatusFlag&TAMPER_LOW_U)
		if((CurrentTamperStatusFlag&TAMPER_OVER_LOAD)==0)
			if(Ins.EffectiveI<IMAX_CURRENT)
				CurrentTamperStatusFlag &= ~TAMPER_OVER_LOAD;
			else
				CurrentTamperStatusFlag |= TAMPER_OVER_LOAD;
				
	if(TamperRecord.TamperStatusFlag&TAMPER_TEMP)
		if(InsTemp>TEMPERATURE_L_THD)
			CurrentTamperStatusFlag |=TAMPER_TEMP;
				
				
		
	return status;
}
/*--------------------------------------------------------------------------*/

void Tamperlog(void)
{   
	uint8_t i,save_flag=0;
	uint16_t TamperType=1;
	
	uint16_t loc=0;
	uint8_t eventtype;
     
	for(i=0;i<MAX_TAMPER_COUNT;i++)
	{   

		if(((TamperRecord.TamperStatusFlag & TamperType) ^ (CurrentTamperStatusFlag & TamperType))>0)
		{   
			eventtype = (TamperRecord.TamperStatusFlag ^ TamperType) & TamperType ?0:1;  // occurence or recovery
			
			TamperTime_counter[i]++;

			//-------------------------------------------------------------

			if(((TamperTime_counter[i] >= TAMPER_EVENT_OCC_TIME[i])&&(eventtype==0))||((TamperTime_counter[i] >= TAMPER_EVENT_REC_TIME[i])&&(eventtype==1)))
			{ 
				if((TamperType==TAMPER_COVEROPEN))
				{
					setParaBuff(DISPLAY_MODE_AUTO_COVER);
				}
				
				SaveEEPROM(PARA_WRITE_BEFORE_STATE,PARA_TYPE_TAMPER );
				save_flag=1;
				mcu_flag&= ~COMM_RECEIVE_ENABLE; 
				TamperRecord.TamperStatusFlag ^= TamperType;
				TamperTime_counter[i] = 0;
				if((TamperType==TAMPER_COVEROPEN))
				{
					if(eventtype==1)
					continue;
				}


				if(TamperRecord.eventcounts[EventCountIndex[i]]>=EventLen[EventCountIndex[i]])
					TamperRecord.eventcounts[EventCountIndex[i]]=0x00;

				loc=(startAdd[EventCountIndex[i]]+TamperRecord.eventcounts[EventCountIndex[i]]*TAMPER_DATA_LENGTH);	
				makeTamperSnapDataNew(i,eventtype);
				write_eeprom(RxTxBuffer,loc,TAMPER_DATA_LENGTH);
				TamperRecord.lastEventCode[EventCountIndex[i]]=loc;
				TamperRecord.eventcounts[EventCountIndex[i]]++;

				mcu_flag|= COMM_RECEIVE_ENABLE; 
				if(TamperRecord.TamperStatusFlag & TamperType)   // if occurance
				{   
					if(i==7)
					{
						if(TamperRecord.TamperCount[7]==0)
							TamperRecord.lastEventAddr[i]=loc;
					}
					else
						TamperRecord.lastEventAddr[i]=loc;
						
					TamperRecord.TamperCount[i]=TamperRecord.TamperCount[i]+1;
					TamperRecord.lastOccRes[0]=loc;
				}
				else
					TamperRecord.lastOccRes[1]=loc;
			}
		}
		else
		{
			TamperTime_counter[i] = 0;
		}
		TamperType<<=1;
	}

	if(save_flag==1)
	{
		R_OBIS_Class07_EventUpdate();
		SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_TAMPER );
		
	}
			
}
/*----------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------*/

uint16_t getTotalTamperCount(void)
{
	uint8_t i;
	uint16_t tcount=0;
	for(i=0;i<MAX_TAMPER_COUNT;i++)
		tcount=tcount+TamperRecord.TamperCount[i] ;
	return tcount;  
}


	
void getEventLog(uint8_t*ptr, uint8_t tampertype,uint8_t eventno)
{
	uint16_t addr; 
	uint16_t loggedEvents=getCurrentLogCount(tampertype);
	
	if(loggedEvents>=EventLen[tampertype])
		addr=TamperRecord.eventcounts[tampertype]%EventLen[tampertype];
	else
		addr=0;

	

	
	addr=addr+eventno;
		
	addr=addr%EventLen[tampertype];
	
	addr=startAdd[tampertype]+addr*TAMPER_DATA_LENGTH;
	
	
	read_eeprom(ptr,addr,TAMPER_DATA_LENGTH);
	
	
}

uint8_t getCurrentLogCount(uint8_t tamperid)
{
	uint16_t tamper_count[4];
	tamper_count[0]=TamperRecord.TamperCount[1]+TamperRecord.TamperCount[3]+TamperRecord.TamperCount[5];
	tamper_count[1]=TamperRecord.TamperCount[0]+TamperRecord.TamperCount[2]+TamperRecord.TamperCount[4]+TamperRecord.TamperCount[6];
	tamper_count[2]=TamperRecord.TamperCount[7];
	tamper_count[3]=TamperRecord.TamperCount[8];	
	
	if(TamperRecord.eventcounts[tamperid]<tamper_count[tamperid])
		return EventLen[tamperid];
	else
		return TamperRecord.eventcounts[tamperid];

}

uint8_t getEventMaxEntries(uint8_t tamperid)
{
	uint8_t maxEntry=0;
	switch(tamperid)
	{
		case 0:
			maxEntry=EventLen[0];
			break;
		case 1:
			maxEntry=PFAIL_EVENTS*2;
			break;
		case 2:
			maxEntry=SW_EVENTS;
			break;
		case 3:
			maxEntry=EventLen[1];
			break;
		case 4:
			maxEntry=EventLen[2];
			break;
		case 5:
			maxEntry=EventLen[3];
			break;			
	}
	return maxEntry;
}



void logSWTamper(uint8_t eventid)
{
	uint8_t logBuffer[6];
	SaveEEPROM(PARA_WRITE_BEFORE_STATE,PARA_TYPE_TAMPER );
	
	logBuffer[0]=InsSave.timeCounter;
	logBuffer[1]=InsSave.timeCounter>>8;
	logBuffer[2]=InsSave.timeCounter>>16;
	logBuffer[3]=InsSave.timeCounter>>24;
	logBuffer[4]=eventid;
	
	write_eeprom((uint8_t *)&logBuffer,SOFT_CHANGE_LOG_LOC+(TamperRecord.sw_log_count%SW_EVENTS)*5,5);
	
	TamperRecord.sw_log_count++;
	R_OBIS_Class07_EventUpdate();
	SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_TAMPER );  
	
}

void getPfailLog(uint8_t*ptr,uint8_t eventno)
{
	
	uint16_t addr; 
	
	if(InsSave.PFailCount>PFAIL_EVENTS)
		addr=InsSave.PFailCount%PFAIL_EVENTS;
	else
		addr=0;


	addr=addr+(eventno/2);

	addr=addr%PFAIL_EVENTS;

	addr=POWER_FAIL_LOC+addr*8; 
	
	
	read_eeprom(ptr,addr+(eventno%2)*4,4);
	
	*(ptr+4)=0x65+eventno%2;
	*(ptr+5)=0x00;;
		
}



void getSWLog(uint8_t*ptr,uint8_t eventno)
{
	
	uint16_t addr; 
  

	if(TamperRecord.sw_log_count>SW_EVENTS)
		addr=TamperRecord.sw_log_count%SW_EVENTS;
	else
		addr=0;

	addr=addr+eventno;

	addr=addr%SW_EVENTS;
	addr=SOFT_CHANGE_LOG_LOC+addr*5; 

	read_eeprom(ptr,addr,5);

	//*(ptr+4)=((*ptr+4))&0xFF);
	*(ptr+5)=0x00;;
	
}

void ClearCopen(void)
{
	uint8_t i;

	for(i=0;i<TAMPER_DATA_LENGTH;RxTxBuffer[i++] = 0x00);
		
	
	write_eeprom(RxTxBuffer,TAMPER_NON_ROLLOVER_LOC,TAMPER_DATA_LENGTH);

	TamperRecord.TamperStatusFlag &= ~TAMPER_COVEROPEN;
	TamperRecord.TamperCount[7]=0x00;
	TamperRecord.lastEventCode[EventCountIndex[7]]=0x00;
	if(TamperRecord.eventcounts[EventCountIndex[7]]>0)
		TamperRecord.eventcounts[EventCountIndex[7]]=0;
	
		
	R_OBIS_Class07_EventUpdate();
	
	SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_TAMPER );
	setParaBuff(DISPLAY_MODE_NORMAL); // restore orignal display sheet
	
}

uint8_t getlastEventID(uint8_t eventtype)
{
	
	if(TamperRecord.lastOccRes[eventtype]>0)
		read_eeprom((uint8_t*)&eventtype,TamperRecord.lastOccRes[eventtype]+4,2);
	else
		eventtype=0;
		
	return eventtype;
}

uint8_t isEarth(uint16_t earthLimit)
{
	uint16_t status=0;
	uint32_t GCurrent1=0;
	uint32_t GCurrent2=0;
	
	if(Ins.NeuCurrent  > Ins.PhCurrent)          // Earth Tamper Detection
	{
		GCurrent1=Ins.NeuCurrent;
		GCurrent2=Ins.PhCurrent; 
		
	}
	else
	{
		GCurrent1=Ins.PhCurrent; 
		GCurrent2=Ins.NeuCurrent;
	}
	
	if(Ins.EffectiveI >=EARTH_TAMPER_I_THRESHOLD)   // 50 ma 
		if(GCurrent1 >=EARTH_TAMPER_I_THRESHOLD)   // 50 ma 
			if((((GCurrent1 -GCurrent2 )*10000)/GCurrent1) >= earthLimit)
				status= 1;
	
	return status;
	
}

uint16_t get3TamperCount(void)
{
	uint16_t count;
	count=TamperRecord.TamperCount[7]+TamperRecord.TamperCount[2];
	return count;
}

void makeTamperSnapDataNew(uint8_t index,uint8_t eventtype)
{
	mcu_flag&= ~COMM_RECEIVE_ENABLE;
	if(index==7)
		makeByte(InsSave.timeCounter,0,4);		// 4 bytes
	else if(eventtype==0)
		makeByte(InsSave.timeCounter-TAMPER_EVENT_OCC_TIME[index],0,4);		// 4 bytes
	else
		makeByte(InsSave.timeCounter-TAMPER_EVENT_REC_TIME[index],0,4);		// 4 bytes
		
	makeByte(dlmsEventCode[index][eventtype],4,2); 					  	// 2  byte

	if(TamperRecord.TamperStatusFlag&TAMPER_MAG)
	{
		makeByte(IMAX_CURRENT,6,4);									// 4 bytes
		makeByte(IMAX_CURRENT,10,4);									// 4 bytes
	}
	else
	{
		makeByte((Ins.PhCurrent/10)*10,6,4);									// 4 bytes
		makeByte((Ins.NeuCurrent/10)*10,10,4);									// 4 bytes
		
	}


	makeByte(Ins.Voltage,14,2);										// 2 bytes
	if(Ins.LeadLag==1)
		makeByte(0x80|Ins.PowerFactor ,16,1);  								// 1 bytes
	else
		makeByte(Ins.PowerFactor ,16,1);  								// 1 bytes

	makeByte(InsSave.CumkWh+InsSave.ZkWhCounter/METER_CONSTANT,17,4);           // 4 bytes
	makeByte(InsSave.CumkVAh+InsSave.ZkVAhCounter/METER_CONSTANT,21,4);           // 4 bytes
	write_eeprom(RxTxBuffer,TAMPER_SNAP_LOC[index],TAMPER_DATA_LENGTH);
	mcu_flag|= COMM_RECEIVE_ENABLE;
	
}
