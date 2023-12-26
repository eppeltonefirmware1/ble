//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dClock.h"
#include "..\\..\\rlDevice\\Include\\dUser_includes.h"
#include "..\\..\\rlDevice\\Include\\dRtc.h"
#include "..\\..\\rlDevice\\Include\\dDeltaSigmaADC.h"
#include "..\\..\\rlDevice\\Include\\dUart.h"
#include "..\\..\\rlDevice\\Include\\dWatchDog.h"
#include "..\\..\\rlDevice\\Include\\dVBat.h"
#include "..\\..\\rlDevice\\Include\\dMisc.h"
#include "..\\..\\rlDevice\\Include\\dIOCtrl.h"
#include "..\\..\\rlDevice\\Include\\d8bitTimer.h"
#include "..\\..\\rlDevice\\Include\\dRtc.h"
#include "..\\..\\rlDevice\\Include\\dSARAdc.h"

//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppPowerOnInit.h"
#include "..\\Include\\AppCommunication.h"
#include "..\\Include\\AppOneSecondTask.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppBatteryManagement.h"
#include "..\\Include\\AppConfig.h"
//------------------------------------------------------

//-------- include dlms specific files----------------
#include "..\\..\\rlDlms\\DLMS_User.h"



void prepareLowPower(void);
void Meter2_Response(const uint8_t* bptr,uint16_t noofBytes);
main(void)
{
	
	init_poweron();
	//setRTCOut(1);
	mcu_flag|=(START_MEASUREMENT+START_SAMPLE_BUFFER);
	mcu_flag |= COMM_RECEIVE_ENABLE;       //Enable reception
	EI();

	
	ls_nm_start_loc=0xFFFF;
	nm_ls_start_index=0xFFFF;
	nm_ls_end_index=0;
	
	
	
	
	//---------------------------------------
	DLMS_Initialize();
	//---------------------------------------
	
	
	
	while(1)
	{
		//Switch2FilClock();
		
		WDT_Restart(0xAC);
		reEnable32KCrystal();
		if(mcu_flag&ONE_SECOND_OVER_FLAG)
		{
			addPulsesActive(no_of_pulses_in_nm_Active);
			addPulsesApp(no_of_pulses_in_nm_App);
			OneSecondTask();
			mcu_flag&=~ONE_SECOND_OVER_FLAG;
			
			//Meter2_Response("Hello",5);
			
		}
		
		
	//-------------------measurement----------
		if(mcu_flag&DO_MEASUREMENT)
		{
			mcu_flag&=~DO_MEASUREMENT;
			
			//do calculation here
			doMeasurement(USER_CODE);
			
			
		}
		if(mcu_flag&DO_FREQ_MEASUREMENT)
		{
			mcu_flag&=~DO_FREQ_MEASUREMENT;
			Calculate_Line_Frq();
		}
	//----------------------------------------
	
	//----------------------------------------
		if(mcu_flag&COMM_RECEIVE_OVER)
		{
			mcu_flag&=~COMM_RECEIVE_OVER;
			Communication(USER_CODE);
			
		}
	//----------------------------------------
	
	//----------------------------------------
	
		/* DLMS process of request and response message */		
		DLMSMessageProcess();
	
	//----------------------------------------
		
	//----------------------------------------
		if(power_fail_state&SAVE_LEVEL1_KWH_STATE_OPEN)
		{
			if(power_fail_state&SAVE_LEVEL1_KWH_STATE_CLOSE)
			{
				
				if(mcu_flag&POWER_STABLE_FLAG)
				{
					mcu_flag|=ONE_SECOND_OVER_FLAG;
					SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_PFAIL_INSTANT);
					mcu_flag&=~ONE_SECOND_OVER_FLAG;
					power_fail_state&=~SAVE_LEVEL2_KWH_STATE_CLOSE;
				}
				power_fail_state&=~SAVE_LEVEL1_KWH_STATE_OPEN;
			}
			
		}
		
		if((mcu_flag&POWER_DOWN_FLAG)||(GET_MAINS_STATUS==0)||(power_fail_state&FORCE_POWER_FAIL))
		{
			#if(defined(NM_HARD_TEST) && (NM_HARD_TEST == 1))
				if(mcu_flag&POWER_STABLE_FLAG)
					if(GET_COVER_STATUS)
					{	
			#endif
						powerStableCounter=0;
						mcu_flag&=~POWER_DOWN_FLAG;
						if(power_fail_state&SAVE_LEVEL2_KWH_STATE_CLOSE)
						{
							mcu_flag|=ONE_SECOND_OVER_FLAG;
							SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_PFAIL_INSTANT);
						}
						
						getSetPfailData(ENERGY_W_RAM);
						PMK1=1;
						prepareLowPower();
						DI();
						
						ClearADCData();
						mcu_flag=0;
						EI();
						noResetInit();
						power_fail_state=0;
						
						mcu_flag|=(START_MEASUREMENT+START_SAMPLE_BUFFER);
						mcu_flag |= COMM_RECEIVE_ENABLE;       //Enable reception

						
						ls_nm_start_loc=0xFFFF;
						nm_ls_start_index=0xFFFF;
						nm_ls_end_index=0;
						is_vfail_with_load=0;
						
			#if(defined(NM_HARD_TEST) && (NM_HARD_TEST == 1))
					}
			#endif

		}
	//----------------------------------------
	
	

		
	}
	
}

void prepareLowPower(void)
{
	activePulsePower=0;
	apparentPulsePower=0;
	shutDownPheripheral();
#if (defined(IS_PRODUCTION_MODE_ENABLED) && (IS_PRODUCTION_MODE_ENABLED == 0))	
	powerfail_op_without_NM();
#endif
}


