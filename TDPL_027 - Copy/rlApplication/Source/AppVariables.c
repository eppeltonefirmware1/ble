//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dTypedef.h"
//-----------------------------------------------------

//-------- include app specific files----------------
#include "..\\include\\AppMacros.h"
#include "..\\include\\AppConfig.h"

//------------------------------------------------------


//----------------4 bytes data--------------
uint8_t ramCHECK1='R';
uint32_t activePulsePower,apparentPulsePower;
uint32_t activePulsePowerSumNM,activePowerNM;
uint32_t apperentPulsePowerSumNM,apperentPowerNM;
volatile uint32_t mcu_flag;
uint32_t prekWhCounter,prekVAhCounter,prekWh,prevTimeCounter,prekWhinterval;
#if (defined(LOG_NM_INFO) && (LOG_NM_INFO == 1))
uint32_t nmInfo[3];
#endif
uint32_t nmPhCurrent,nmNuCurrent,nmEffectiveI;
//-----------------------------------------

//--------------2 bytes data-----------------
uint8_t ramCHECK2='A';
uint16_t frequency_sample_count;
uint16_t TamperTime_counter[MAX_TAMPER_COUNT];
uint16_t pushModeTime,gblFrq,gblWave;
uint16_t display_flag=0,ls_nm_start_loc,nm_ls_start_index,nm_ls_end_index,CurrentTamperStatusFlag=0;
uint16_t nm_false_current_detect_count,InsTemp=250;
volatile uint16_t mag_toggle_count_duration=0,mag_pulse_width,is_dc_magnet_exist_duration=0,peakVoltage;
volatile uint16_t no_of_pulses_in_nm_Active,no_of_pulses_in_nm_App;

//----------------------------------

//---------------rtc----------------------
uint8_t ramCHECK3='M',isforcedUinity=0,resetflag_auto;
#if (defined(TATA_POWER_TYPE_1) && (TATA_POWER_TYPE_1 == 1))
uint8_t idisplayMode=0;
#else
uint8_t idisplayMode=1;
#endif
uint8_t RxTxBuffer[MAX_COMM_BUFFER_LENGTH];
//----------------------------------------

//----------------------------------------
uint8_t md_period=2,ls_period,pf_by_sample,new_md_period;
uint8_t powerStableCounter=0;
volatile uint8_t mag_hold_time,mag_toggle_count=0,cum_mag_toggle_count=0,power_fail_state,permanent_mag_wait_time,received_uart;
volatile uint8_t is_vfail_with_load;


//--------------display-------------------
uint8_t ramCHECK4='O';
uint8_t refresh_time=0,ModeID=0,button_hold_time=0,currentZoneID,currentSeasonID,esd_time_counter_occ,esd_time_counter_res;
const uint8_t *displayParamPtr;
uint8_t ramCHECK5='K';
uint8_t harChU,harChI,har_wait_time;
//----------------------------------------


struct
{   

	uint16_t	Voltage     	;
	uint8_t 	PowerFactor 	;
	uint8_t 	LeadLag     	;	
	uint32_t  	PhCurrent   	;
	uint32_t  	NeuCurrent  	;
	uint32_t 	PhPower     	;
	uint32_t 	NeuPower    	;
	uint32_t 	AppPower    	;
	uint32_t  	Frequency   	;
	uint32_t  	EffectiveI   	;
	uint32_t  	EffectiveP   	;
    
}Ins;

struct MD
{
	uint16_t kW;
	uint16_t kVA;
	uint32_t kW_Date;
	uint32_t kVA_Date;
};


struct billMD
{
	uint32_t CumkWMD;
	uint32_t CumkVAMD;
	struct MD Current;
};

struct TOD
{
	struct MD TODMD; 
	uint32_t kWh;
	uint32_t kVAh;
};

struct TOD_8
{
	struct TOD TOD[8];
};

//#pragma section bss EngRam
struct
{
	uint32_t  	CumkWh				;
	uint32_t  	CumkVAh				;
	struct billMD	BillMD			;		//116 bytes
	struct TOD_8	TODEnergy		;		//64 bytes
	uint32_t    MonthPowerOnDuration;
	uint32_t	CumPowerOnDuration	;
	uint16_t 	DayCount			;
	uint16_t 	DailyDayCount		;
	uint16_t  	PFailCount			;
	uint8_t		MDResetCount		;
	uint8_t 	IntCount			;
	uint32_t  	CumPowerOffDuration	;
	uint32_t  	timeCounter			; // real time
	uint32_t  	lsAvgV				;
	uint32_t  	lsAvgC				;
	uint32_t  	kWhCounter10m		;
	uint32_t  	kVAhCounter10m		;
	uint16_t  	mdkWhPulseCounter	;
	uint16_t  	mdkVAhPulseCounter	;
	uint16_t  	lskWhPulseCounter	;
	uint16_t  	lskVAhPulseCounter	;
	uint16_t  	ZkWhCounter			;
	uint16_t  	ZkVAhCounter		;
	uint32_t  	lsAvgT				;
	uint32_t  	lsAvgNC				;
	uint32_t  	lsAvgPC				;
	uint16_t  	PowerOn30			;
	uint16_t  	daykWMD				;
	uint16_t  	daykVAMD			;
	uint16_t	lsAvg900PC			;
	uint16_t	lsAvg900NC			;
	uint16_t	lsAvg900C			;
	uint16_t	lsAvg900V			;
	uint16_t  	crc2				;
	
}InsSave;
//#pragma section

union 
{   
	uint32_t long_data;
	struct
	{ 
		uint16_t lsb;
		uint16_t msb;
	}Short;
	uint8_t byte[4];
}Data;



struct 
{
	uint16_t 	TamperCount[MAX_TAMPER_COUNT];
	uint8_t  	eventcounts[TAMPER_GROUP_COUNT];//only 0,1,2,3
	uint16_t 	sw_log_count;
	uint16_t  	TamperStatusFlag	;
	uint16_t 	lastEventCode[TAMPER_GROUP_COUNT];
	uint16_t 	lastEventAddr[MAX_TAMPER_COUNT];
	uint16_t	lastOccRes[2];
	uint16_t 	crc;		
	
}TamperRecord;




struct 
{
	uint32_t CalanderDateTime;
	uint32_t BillDateTime;
	
}ScheduleActionDateTime;
