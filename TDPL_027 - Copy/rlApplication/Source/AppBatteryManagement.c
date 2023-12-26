//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dUser_includes.h"
#include "..\\..\\rlDevice\\Include\\dTypedef.h"
#include "..\\..\\rlDevice\\Include\\d8bitTimer.h"
#include "..\\..\\rlDevice\\Include\\d12bitTimer.h"
#include "..\\..\\rlDevice\\Include\\dIOCTrl.h"
#include "..\\..\\rlDevice\\Include\\dWatchDog.h"
#include "..\\..\\rlDevice\\Include\\dRtc.h"
#include "..\\..\\rlDevice\\Include\\dVbat.h"
#include "..\\..\\rlDevice\\Include\\dDisplay.h"
#include "..\\..\\rlDevice\\Include\\dUart.h"
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dDeltaSigmaADC.h"
#include "..\\..\\rlDevice\\Include\\dMisc.h"


//-------- include app specific files-------------------
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppCommunication.h"
#include "..\\Include\\AppNMTask.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppTampers.h"
#include "..\\Include\\AppIntervalkWh.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppBatteryManagement.h"
#include "..\\\\Include\\AppPowerOnInit.h"

//------------------------------------------------------

//-------- include dlms specific files----------------
#include "..\\..\\rlDlms\\DLMS_User.h"


#define LCD_SWITCH_ON_WAIT_TIME	1		//seconds
#define COM_SWITCH_ON_WAIT_TIME	5		//seconds
#define PULSE_SWITCH_ON_WAIT_TIME	6	//seconds
#define NM_PULSE_INTERVAL			15	//seconds
#define PFAIL_WAIT_COUNT_5		6		
#define PFAIL_WAIT_COUNT_1		10

#define PFAIL_MODE_STOP								0x0000
#define PFAIL_MODE_LCD								0x0001
#define PFAIL_MODE_COM								0x0002
#define PFAIL_MODE_ADC								0x0008
#define PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD			0x0010
#define PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_LCD	0x0020
#define PFAIL_MODE_SWITCH_PRESS_AWAIT_COM			0x0040
#define PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_COM	0x0080
#define EXIT_PFAIL_OPERATION						0x0100
#define DO_ACTION_AT_POWER_RESTORE					0x0200
#define COVER_OPEN_ACTION							0x0400
#define SWITCH_ON_LCD								0x0800
#define SWITCH_ON_COM								0x1000
#define SWITCH_OFF_LCD								0x2000
#define SWITCH_ON_PULSE								0x4000
#define PFAIL_MODE_PULSE							0x8000




#define SWITCH_ON_8BIT_TIMER	0x10
#define SWITCH_OFF_8BIT_TIMER	0x20

//#if (defined(METER_RATING_5_30) && (METER_RATING_5_30 == 1))
//	#define PER_PULSE_VALUE_FAST	869UL
//	#define PER_PULSE_VALUE_SLOW	81UL
//#else
//	#define PER_PULSE_VALUE_FAST	1875UL
//	#define PER_PULSE_VALUE_SLOW	156UL
//#endif

#define DSP_FAST_MODE_INTERVAL	2
#define DSP_SLOW_MODE_INTERVAL	60
#define DSP_NO_LOAD_MODE_INTERVAL	900

#define DSP_LOAD_SECOND_STAGE_WAIT_TIME		5400
#define DSP_NO_LOAD_FIRST_STAGE_WAIT_TIME	600
#define DSP_NO_LOAD_SECOND_STAGE_WAIT_TIME	86400UL
#define PULSE_MODE_DURATION	30
#define MAG_VCC_ON_TIME		600

#define BATTERY_LCD_LOCK_COUNT	3
#define BATTERY_LCD_DISPLAY_DURATION	0	//64800U

extern Unsigned32					g_Class07_Event_EntriesInUse[];
void updatePfailEvent(void);
void updatePulse(void);
void updateEnergyPulsePara(uint8_t ioption,uint32_t current);
extern uint8_t push_button_pressed_for_nm;

extern uint16_t TAMPER_EVENT_OCC_TIME[MAX_TAMPER_COUNT];
extern uint16_t TAMPER_EVENT_REC_TIME[MAX_TAMPER_COUNT];
#if (defined(IS_PRODUCTION_MODE_ENABLED) && (IS_PRODUCTION_MODE_ENABLED == 0))
void powerfail_op_without_NM(void)
{

	uint16_t op_state=PFAIL_MODE_STOP,op_action_state=PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
	uint8_t com_enable_wait_counter;
	uint8_t display_no_of_para=0,display_no_of_para_count=0;
	uint8_t pfail_wait_count=0;
	//uint16_t sum_of_power=0;
	//uint16_t no_of_pulses;
	uint32_t adc_switch_wait_time=0;
	uint8_t battery_mode_lcd_on_count=0;
	uint8_t battery_mode_nm_on_count=0;
	uint16_t battery_mode_display_time=0;
	uint32_t nowNMkWh=0;
	
	
	resetflag_auto=resetflag_auto&0x01;
	
	if(is_vfail_with_load==0)
		start_Timer8bit0(DSP_FAST_MODE_INTERVAL);
	//else
	//	start_Timer8bit0(1);
	//updateEnergyPulsePara(TRTCMP0/256);
	

	initNMLs();
	mcu_flag=GET_SHUNT_CURRENT_FLAG;
	CurrentTamperStatusFlag=0;
	RxTxBuffer[0]=0x00;
	write_eeprom(RxTxBuffer,RTC_BAT_STATUS_LOC,1);
	frequency_sample_count=0;
	

	isRAMOK();
	
	do
	{
		//Switch2FilClock();
		WDT_Restart(0xAC);
		switch(op_state)
		{
			//-----------------------------------------------------
			case DO_ACTION_AT_POWER_RESTORE:
				// do action at power restore
				op_state=EXIT_PFAIL_OPERATION;
			break;
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			case PFAIL_MODE_STOP:
					
				#if(defined(NM_HARD_TEST) && (NM_HARD_TEST == 1))
					if(GET_COVER_STATUS)
				#endif
				reEnable32KCrystal();
				if(GET_MAINS_STATUS)
				{
					++pfail_wait_count;
					if((TRTCMP0/256==1))
					{
						if(pfail_wait_count>PFAIL_WAIT_COUNT_1)
							op_state=DO_ACTION_AT_POWER_RESTORE;
					}
					else if(pfail_wait_count>PFAIL_WAIT_COUNT_5)
						op_state=DO_ACTION_AT_POWER_RESTORE;
				}
				else
				{
					pfail_wait_count=PFAIL_WAIT_COUNT_1;
					is_dc_magnet_exist_duration=0;
					if(resetflag_auto==1)
					{
						stop_Timer8bit0();
						resetflag_auto=2;
						is_vfail_with_load=0;
						CheckCurrentLimitNm();
						SET_MAG_OFF_VCC
					}
					PMK1=0;
				}
				
				if((op_state!=DO_ACTION_AT_POWER_RESTORE)&&(push_button_pressed_for_nm==0))
				{
					if(op_action_state&PFAIL_MODE_COM)
						switch2Low_Power_MODE(LOW_POWER_HALT);
					else
						switch2Low_Power_MODE(LOW_POWER_STOP);
					op_state=PFAIL_MODE_ADC;
				}
				if(push_button_pressed_for_nm==1)
				{
					push_button_pressed_for_nm=0;
					if(op_action_state&PFAIL_MODE_LCD)
					{
						mcu_flag|=EIGHT_BIT_TIMER1_FLAG;
						mcu_flag&=~DISPLAY_CHANGE_FLAG;
						if(display_no_of_para_count>0)
							display_no_of_para_count--;
						
						
					}
				}
					
				isRAMOK();
				break;
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			case PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD:
				if((op_action_state&PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD)&&((battery_mode_lcd_on_count<BATTERY_LCD_LOCK_COUNT)||(battery_mode_nm_on_count<BATTERY_LCD_LOCK_COUNT)))
				{
					if(GET_PUSH_BUTTON_STATUS)
					{
						nm_false_current_detect_count=0;
						if(battery_mode_lcd_on_count<BATTERY_LCD_LOCK_COUNT)
						{
							mcu_flag&=~(EIGHT_BIT_TIMER1_FLAG|PUSH_RELEASED_NM_MODE);
							if((op_action_state&PFAIL_MODE_LCD)==0)
								init_Timer8bit1(LCD_SWITCH_ON_WAIT_TIME);
								
							op_action_state&=~PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
							op_action_state|=PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_LCD;
						}
						

						
						if(battery_mode_nm_on_count<BATTERY_LCD_LOCK_COUNT)
						{
							if(resetflag_auto==0)
							{
								if(Ins.EffectiveI==0)
								{
									doMeasurement_NM(0);
									doMeasurement_NM(0);
								}
							}
							if(Ins.EffectiveI>0)
							{
								if(battery_mode_lcd_on_count>=BATTERY_LCD_LOCK_COUNT)
								{
									mcu_flag&=~(EIGHT_BIT_TIMER1_FLAG|PUSH_RELEASED_NM_MODE);
									if((op_action_state&PFAIL_MODE_LCD)==0)
										init_Timer8bit1(LCD_SWITCH_ON_WAIT_TIME);
									
									op_action_state&=~PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
									op_action_state|=PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_LCD;
								}
							}
						}
						
					}
				}
				op_state=PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_LCD;
				break;
			//-----------------------------------------------------

			//-----------------------------------------------------
			case PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_LCD:
			
				if(op_action_state&PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_LCD)
				{
					if((GET_PUSH_BUTTON_STATUS==0)||(mcu_flag&EIGHT_BIT_TIMER1_FLAG))
					{
						if((op_action_state&PFAIL_MODE_LCD)==0)
							stop_Timer8bit1();
						if(mcu_flag&EIGHT_BIT_TIMER1_FLAG)
						{
							op_action_state|=SWITCH_ON_LCD;
						}
						else
						{
							if((op_action_state&PFAIL_MODE_LCD)==0)
								op_action_state=PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
							else
								op_action_state|=PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
							
						}
						op_action_state&=~PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_LCD;
						mcu_flag&=~EIGHT_BIT_TIMER1_FLAG;
						
					}
				}
				op_state=PFAIL_MODE_SWITCH_PRESS_AWAIT_COM;
				break;
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			case PFAIL_MODE_SWITCH_PRESS_AWAIT_COM:
				if(op_action_state&PFAIL_MODE_SWITCH_PRESS_AWAIT_COM)
				{
					if((GET_PUSH_BUTTON_STATUS)&&(mcu_flag&PUSH_RELEASED_NM_MODE))
					{
						op_action_state&=~PFAIL_MODE_SWITCH_PRESS_AWAIT_COM;
						com_enable_wait_counter=0;
						start_RTC_Second_Interrupt();
						op_action_state|=PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_COM;
					}
				}
				op_state=PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_COM;
				break;
			//-----------------------------------------------------

			//-----------------------------------------------------
			case PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_COM:
			
					
				if(op_action_state&PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_COM)
				{
					if(mcu_flag&RTC_SECOND_FLAG)
					{
						if((++com_enable_wait_counter>=COM_SWITCH_ON_WAIT_TIME))
						{
							if((op_action_state&PFAIL_MODE_COM)==0)
							{
								if(com_enable_wait_counter==COM_SWITCH_ON_WAIT_TIME)
								{
									setParamIndex(MSG_ON_INDEX,1);		
									mcu_flag|=EIGHT_BIT_TIMER1_FLAG;
									op_action_state|=SWITCH_ON_COM;
									display_no_of_para_count=0;
									op_action_state&=~PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_COM;
												
								}
							}
						}
						mcu_flag&=~RTC_SECOND_FLAG;
					}
					if(GET_PUSH_BUTTON_STATUS==0)
					{
						op_action_state&=~PFAIL_MODE_SWITCH_PRESS_RELEASE_AWAIT_COM;
						op_action_state|=PFAIL_MODE_SWITCH_PRESS_AWAIT_COM;
					}
					
				}
				op_state=SWITCH_ON_LCD;
				break;
			//-----------------------------------------------------
			
				case SWITCH_ON_LCD:
					if(op_action_state&SWITCH_ON_LCD)
					{
						activePulsePower=0;
						op_action_state&=~(SWITCH_ON_LCD|PFAIL_MODE_PULSE);
						mcu_flag&=~DISPLAY_CHANGE_FLAG;
						adc_switch_wait_time=0;
						mcu_flag&=~ADC_LOAD_SECOND_STAGE;
						mcu_flag&=~ADC_NO_LOAD_FIRST_STAGE;
						if(Ins.EffectiveI>0)
						{
							setParaBuff(DISPLAY_MODE_NORMAL);
							op_action_state|=PFAIL_MODE_LCD;
						}
						else
						{
							setParaBuff(DISPLAY_MODE_UOUT_NORMAL);
							op_action_state|=(PFAIL_MODE_LCD|PFAIL_MODE_SWITCH_PRESS_AWAIT_COM);
						}
						
					
						
						Clear_Lcd();
						R_LCD_PowerOn();
						mcu_flag|=EIGHT_BIT_TIMER1_FLAG;
						if((DSP_FAST_MODE_INTERVAL!=(TRTCMP0/256))&&(TSTART00==1))
							start_Timer8bit0(DSP_FAST_MODE_INTERVAL);
							
						if(battery_mode_nm_on_count<BATTERY_LCD_LOCK_COUNT)
						{
							if(Ins.EffectiveI>0)
							{
								op_action_state|=PFAIL_MODE_PULSE;
								mcu_flag|=START_PULSE_NM_MODE;
								mcu_flag|=PULSE_NM_MODE;
								battery_mode_nm_on_count++;
								SET_MAG_ON_VCC
								
							}
						}
						display_no_of_para=getParaCount();
						if(op_action_state&PFAIL_MODE_PULSE)
						{
							setParaBuff(DISPLAY_MODE_PUSH);
							
							display_no_of_para=30;
						}
						
							
						display_no_of_para_count=1;
						
						//battery_mode_lcd_on_count=0;
						getRtcCounter(RTC_CHECK_MODE);
						
					}
					op_state=SWITCH_ON_PULSE;
				break;
				case SWITCH_ON_COM:
					if(op_action_state&SWITCH_ON_COM)
					{
						op_action_state&=~SWITCH_ON_COM;
						display_no_of_para_count=0;
						
						//-----------------------
						init_Uart0();	//enable commuication
						//init_Uart2();
						if(TMKAEN==0)
							init_Timer12bit(0);// enable 12 bit timer
						//-----------------------
						mcu_flag|=COMM_RECEIVE_ENABLE;
						op_action_state|=PFAIL_MODE_COM;
						mcu_flag&=~COMM_TRANSMIT_FLAG;
						SWITCH_ON_COMM_VCC;
						op_action_state|=PFAIL_MODE_COM;
						
						//op_action_state|=PFAIL_MODE_SWITCH_PRESS_AWAIT_COM;
						if((op_action_state&PFAIL_MODE_PULSE)==0)
						{
							display_no_of_para=getParaCount();
							if(display_no_of_para>6)
								display_no_of_para=6;
						}
						if(display_no_of_para<6)
							display_no_of_para=6;
					}
					op_state=PFAIL_MODE_COM;
				break;
				case SWITCH_ON_PULSE:
					op_state=PFAIL_MODE_PULSE;
				break;
				
				case SWITCH_OFF_LCD:
				
					if(op_action_state&SWITCH_OFF_LCD)
					{
						if(((mcu_flag&PUSH_RELEASED_NM_MODE)&&((GET_PUSH_BUTTON_STATUS)==0))||((mcu_flag&PUSH_RELEASED_NM_MODE)==0))						
						{
							ITMC&=~0x80;	// stop 12 bit timer
							TMKAEN=0;
							Uart0_Stop();// switch off uart
							Uart2_Stop();// switch off uart
							SWITCH_OFF_PULSE_LED;
							SWITCH_OFF_COMM_VCC;
							activePulsePower=0;
							mcu_flag&=~(COMM_RECEIVE_ENABLE|START_PULSE_NM_MODE|PULSE_NM_MODE);
							stop_RTC_Second_Interrupt();
							if(battery_mode_display_time>=BATTERY_LCD_DISPLAY_DURATION)
							{
								stop_Timer8bit1();
								R_LCD_PowerOff();//switch off lcd
								op_action_state=PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
								
							}
							else
							{
								if(Ins.EffectiveI>0)
									setParaBuff(DISPLAY_MODE_NORMAL);
								else
									setParaBuff(DISPLAY_MODE_UOUT_NORMAL);
									
								//setNextParaSeq();
								
								display_no_of_para_count=0;
								op_action_state|=PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
								op_action_state&=~SWITCH_OFF_LCD;
								op_action_state&=~PFAIL_MODE_COM;
								
							}
							
						}
						else
						{
							op_action_state&=~SWITCH_OFF_LCD;
							display_no_of_para_count=0;
						}
					}
					
					op_state=COVER_OPEN_ACTION;
				break;
				
			//-----------------------------------------------------
			case PFAIL_MODE_LCD:
				
				if((mcu_flag&EIGHT_BIT_TIMER1_FLAG))
				{
					
					if(op_action_state&PFAIL_MODE_LCD)
					{
						if(display_no_of_para_count<display_no_of_para)
						{
							if((mcu_flag&DISPLAY_CHANGE_FLAG)==0)
							{
								setParamIndex(*displayParamPtr,0);
								setNextParaSeq();
								
							}
							
							Display(BATTERY_MODE);
							
							getRtcCounter(RTC_CHECK_MODE);
							if(mcu_flag&PUSH_RELEASED_NM_MODE)
							{
								if(((op_action_state&PFAIL_MODE_COM)==0)&&((op_action_state&PFAIL_MODE_PULSE)==0))
									display_no_of_para=2;
						
								if((op_action_state&PFAIL_MODE_PULSE)||(battery_mode_display_time>=BATTERY_LCD_DISPLAY_DURATION)||(op_action_state&PFAIL_MODE_COM))
								{
									display_no_of_para_count++;
								}
							}
							
							start_Timer8bit1(getParamTime());
							
						}
						else
						{
							op_action_state|=SWITCH_OFF_LCD;
						}
						
					}
					
					mcu_flag&=~EIGHT_BIT_TIMER1_FLAG;
				}
				op_state=SWITCH_ON_COM;
				break;
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			case PFAIL_MODE_PULSE:
				
				if(op_action_state&PFAIL_MODE_PULSE)
				{
					if(mcu_flag&PUSH_BUTTON_RELEASED)
					{
						mcu_flag&=~PUSH_BUTTON_RELEASED;	
						if(display_no_of_para_count>0)
							display_no_of_para_count--;
					}
				}	
				if((GET_PUSH_BUTTON_STATUS==0)&&(op_action_state&PFAIL_MODE_LCD))
				{
					mcu_flag|=PUSH_RELEASED_NM_MODE;
				}
				
				op_state=PFAIL_MODE_LCD;
				break;
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			case PFAIL_MODE_COM:
				//----------------------------------------
					if(op_action_state&PFAIL_MODE_COM)
					{
						if(mcu_flag&COMM_RECEIVE_OVER)
						{
							mcu_flag&=~COMM_RECEIVE_OVER;
							Communication(USER_CODE);
							
						}
						/* DLMS process of request and response message */		
						DLMSMessageProcess();
						if(mcu_flag&COMM_TRANSMIT_FLAG)
						{
							mcu_flag&=~COMM_TRANSMIT_FLAG;
							display_no_of_para_count=0;
							getRtcCounter(RTC_CHECK_MODE);
							
						}
						
					}
				//----------------------------------------
				op_state=SWITCH_OFF_LCD;
				break;
			//-----------------------------------------------------
			
			//-----------------------------------------------------
			case PFAIL_MODE_ADC:
				if((mcu_flag&ONE_SECOND_OVER_FLAG)&&(resetflag_auto==0))
				{
					mcu_flag&=~ONE_SECOND_OVER_FLAG;
					//start adc in NM mode here
					mcu_flag|=DO_MEASUREMENT;
						
					if(battery_mode_display_time<BATTERY_LCD_DISPLAY_DURATION)
						battery_mode_display_time+=(TRTCMP0/256);
						
				}
				if(mcu_flag&DO_MEASUREMENT)
				{
					mcu_flag&=~DO_MEASUREMENT;
					if(GET_MAINS_STATUS==0)
					{
						if(frequency_sample_count)
							SET_MAG_ON_VCC
							
						doMeasurement_NM(0);
						nmPhCurrent=Ins.PhCurrent;
						nmNuCurrent=Ins.NeuCurrent;
						nmEffectiveI=Ins.EffectiveI;
						
						if(frequency_sample_count)
						{
							if((CurrentTamperStatusFlag &TAMPER_MAG)==0)
								if(GET_MAG_STATUS&&GET_MAG_VCC_STATUS)
									MCU_Delay(1000);
							frequency_sample_count--;	
						}
							
					}
						
					adc_switch_wait_time+=(TRTCMP0/256);
					
					if((Ins.EffectiveI>0)&&(GET_MAINS_STATUS==0)||(GET_MAG_VCC_STATUS&&GET_MAG_STATUS))
					{
							
						

						getRtcCounter(RTC_CHECK_MODE); 
						addPulsesActive(no_of_pulses_in_nm_Active);
						
						addPulsesApp(no_of_pulses_in_nm_App);
						
						
						
						InsSave.PowerOn30+=(TRTCMP0/256);
						InsSave.lsAvgC=InsSave.lsAvgC+nmEffectiveI*(TRTCMP0/256);
						InsSave.lsAvgPC=InsSave.lsAvgPC+nmPhCurrent*(TRTCMP0/256);
						InsSave.lsAvgNC=InsSave.lsAvgNC+nmNuCurrent*(TRTCMP0/256);
							
						InsSave.lsAvgT=InsSave.lsAvgT+InsTemp*(TRTCMP0/256);
						battery_mode_display_time=0;
						
						if(mcu_flag&PULSE_NM_MODE)
						{
							init_Timer12bit(0);// enable 12 bit timer
							mcu_flag&=~PULSE_NM_MODE;
						}
						
						if((ModeID==DISPLAY_MODE_UOUT_NORMAL)||(ModeID==DISPLAY_MODE_UOUT_COVER))
						{
							setParaBuff(DISPLAY_MODE_NORMAL);
							
						}
						else if(ModeID==DISPLAY_MODE_NORMAL)
							if(((op_action_state&PFAIL_MODE_PULSE)==0)&&((op_action_state&PFAIL_MODE_COM)==0)&&(battery_mode_nm_on_count<BATTERY_LCD_LOCK_COUNT))
								display_no_of_para_count=0;
						
						//battery_mode_lcd_on_count=0;
						mcu_flag|=SAVE_AT_NM_EXIT_FLAG;

						if(Ins.EffectiveI>0)
							display_flag=DISP_NM;
						if((TRTCMP0/256)==1)
						{
							if(adc_switch_wait_time>120)
								start_Timer8bit0(DSP_FAST_MODE_INTERVAL);
						}
						
						frequency_sample_count=MAG_VCC_ON_TIME;
						if(GET_MAG_VCC_STATUS&&GET_MAG_STATUS)
						{
							CurrentTamperStatusFlag |=TAMPER_MAG;
							addTime2MagCounter();
							if(TamperTime_counter[2]>=10)
								display_flag|=DISP_MAG;
							
						}
						else
						{
							if(TamperRecord.TamperStatusFlag&TAMPER_MAG)
							{
								if(CurrentTamperStatusFlag &TAMPER_MAG)
									TamperTime_counter[2]=0;
								addTime2MagCounter();
							}
							CurrentTamperStatusFlag &=~TAMPER_MAG;
							display_flag&=~DISP_MAG;
							
						}
						
						if(TamperRecord.TamperStatusFlag&TAMPER_MAG)
						{
	
							Ins.EffectiveP=IMAX_POWER;
							Ins.AppPower=IMAX_POWER;
							if(GET_MAG_VCC_STATUS&&GET_MAG_STATUS)
							{
								nmEffectiveI=IMAX_CURRENT;
								nmPhCurrent=IMAX_CURRENT;
								nmNuCurrent=IMAX_CURRENT;
							}
							display_flag|=DISP_MAG;
							updateEnergyPulsePara(TRTCMP0/256,IMAX_CURRENT);
					
						}
						else
							updateEnergyPulsePara(TRTCMP0/256,nmEffectiveI);
						
						
						nowNMkWh=InsSave.CumkWh+InsSave.ZkWhCounter/METER_CONSTANT;
						
						if((((nowNMkWh-prekWh) >=NM_KWH_THRESHOLD_LL)&&((nowNMkWh-prekWh) <=NM_KWH_THRESHOLD_UL))||(((nowNMkWh-prekWhinterval) >=NM_KWH_THRESHOLD_LL)&&((nowNMkWh-prekWhinterval) <=NM_KWH_THRESHOLD_UL)))
						{
							isRAMOK();
							savekWh(nowNMkWh);
							prekWh=nowNMkWh;
							prekWhinterval=nowNMkWh;
						}	
						getSetPfailData(ENERGY_W_RAM);
						
						if((mcu_flag&START_LS_NM_MODE)==0)
						{
							mcu_flag|=START_LS_NM_MODE;
							mcu_flag&=~POWER_DOWN_FLAG;
							//if(is_vfail_with_load==0)
							//	updateEnergyPulsePara(TRTCMP0/256+2,nmEffectiveI);
							
							if(isPowerfailLog(BATTERY_MODE))
							{
								powerfailLogging();
								updatePfailEvent();
								
							}
							if((DSP_NO_LOAD_SECOND_STAGE_WAIT_TIME==(TRTCMP0/256))&&(TSTART00==1))
							{
								start_Timer8bit0(DSP_SLOW_MODE_INTERVAL);
							}
						}
						
							

						if((InsSave.timeCounter/900 )!=(prevTimeCounter/900))
							doDatalog_NM();
						if(0)//Ins.Voltage>0)
						{
							if((CurrentTamperStatusFlag&TAMPER_AC_DC)==0)
							{
								TamperTime_counter[4]=0;
								
								CurrentTamperStatusFlag|=(TamperRecord.TamperStatusFlag|TAMPER_AC_DC);
								if((TamperRecord.TamperStatusFlag&TAMPER_NEU_MISS)==0)
									CurrentTamperStatusFlag&=~TAMPER_NEU_MISS;
									
								
							}
							display_flag|=DISP_EARTH;
						}
						else
						{
							
							if(Ins.EffectiveI>0)
							{
								if((CurrentTamperStatusFlag&TAMPER_NEU_MISS)==0)
								{
									TamperTime_counter[0]=0;
									CurrentTamperStatusFlag|=(TamperRecord.TamperStatusFlag|TAMPER_NEU_MISS);
									if((TamperRecord.TamperStatusFlag&TAMPER_AC_DC)==0)
										CurrentTamperStatusFlag&=~TAMPER_AC_DC;
										
								}
							}
							
						}
						if(((TamperRecord.TamperStatusFlag&TAMPER_NEU_MISS) ^ (CurrentTamperStatusFlag &TAMPER_NEU_MISS))>0)
						{
							if(TamperTime_counter[0]==0)
								TamperTime_counter[0]=DSP_FAST_MODE_INTERVAL;
							else
								TamperTime_counter[0]+=(TRTCMP0/256);
								
							if(TamperTime_counter[0]>=TAMPER_EVENT_OCC_TIME[0])
							{
								Tamperlog();
							}
						}
						else
						{
							
							if(((TamperRecord.TamperStatusFlag&TAMPER_AC_DC) ^ (CurrentTamperStatusFlag &TAMPER_AC_DC))>0)
							{
								TamperTime_counter[4]+=(TRTCMP0/256);
								if(TamperTime_counter[4]>=TAMPER_EVENT_OCC_TIME[4])
								{
									Tamperlog();
								}
							}
						}
							
						if(((TamperRecord.TamperStatusFlag&TAMPER_MAG) ^ (CurrentTamperStatusFlag &TAMPER_MAG))>0)
						{
								if(CurrentTamperStatusFlag &TAMPER_MAG)
								{
									if(TamperTime_counter[2]>=TAMPER_EVENT_OCC_TIME[2])
									{
										Tamperlog();
										//updateEnergyPulsePara(TRTCMP0/256,IMAX_CURRENT);
									}
								}
								else
								{
									if(TamperTime_counter[2]>=(TAMPER_EVENT_REC_TIME[2]+8))
									{
										Tamperlog();
										ClearIMaxPhasePowers();
										display_flag&=~DISP_MAG;
									}
									
								}
								if(op_action_state&PFAIL_MODE_PULSE)
									mcu_flag|=START_PULSE_NM_MODE;
						}
						
						if(op_action_state&PFAIL_MODE_PULSE)
						{
							if((mcu_flag&START_PULSE_NM_MODE)==0)
							{
								if((InsSave.timeCounter/NM_PULSE_INTERVAL )!=(prevTimeCounter/NM_PULSE_INTERVAL))
									mcu_flag|=START_PULSE_NM_MODE;
							}
							else
							{
								
								if(TamperRecord.TamperStatusFlag&TAMPER_MAG)
									activePulsePower=REFERENCEVOLTAGE*IMAX_CURRENT;
								else
								{
									doMeasurement_NM(1);
									activePulsePower=REFERENCEVOLTAGE*nmEffectiveI;
								}
								mcu_flag&=~START_PULSE_NM_MODE;
							}
						}
						
						
						prevTimeCounter=InsSave.timeCounter;
						
						if((mcu_flag&ADC_LOAD_SECOND_STAGE)==0)
						{
							if((adc_switch_wait_time>DSP_LOAD_SECOND_STAGE_WAIT_TIME)&&(TSTART00==1))
							{
								mcu_flag|=ADC_LOAD_SECOND_STAGE;
								start_Timer8bit0(DSP_SLOW_MODE_INTERVAL);
							}
						}
						Display(BATTERY_MODE);
						SET_MAG_OFF_VCC
					}
					else
					{
						mcu_flag&=~START_LS_NM_MODE;
						display_flag=0;
						Ins.Voltage=0;
						if(Ins.EffectiveI==0)
						{
							is_vfail_with_load=0;
							CheckCurrentLimitNm();
							ClearIMaxPhasePowers();	
							
							
							if((TRTCMP0/256)==1)
							{
								start_Timer8bit0(DSP_FAST_MODE_INTERVAL);
							}
							CurrentTamperStatusFlag&=~TAMPER_MAG;

						}
						SET_MAG_OFF_VCC

						if(!((ModeID==DISPLAY_MODE_UOUT_NORMAL)||(ModeID==DISPLAY_MODE_UOUT_COVER)))
						{
							if(Ins.EffectiveI>0)
								setParaBuff(DISPLAY_MODE_NORMAL);		
							else
								setParaBuff(DISPLAY_MODE_UOUT_NORMAL);
						}
							
						
						
						if((mcu_flag&POWER_DOWN_FLAG)==0)
						{
							mcu_flag|=POWER_DOWN_FLAG;
							TamperTime_counter[0]=0;
							TamperTime_counter[2]=0;
							TamperTime_counter[4]=0;
							CurrentTamperStatusFlag&=~(TAMPER_NEU_MISS|TAMPER_AC_DC);
							if(mcu_flag&SAVE_AT_NM_EXIT_FLAG)
							{
								addPulsesActive(no_of_pulses_in_nm_Active);
								addPulsesApp(no_of_pulses_in_nm_App);
							}
							
						}
						
						if((mcu_flag&ADC_NO_LOAD_FIRST_STAGE)==0)
						{
							if((adc_switch_wait_time>DSP_NO_LOAD_FIRST_STAGE_WAIT_TIME)&&(TSTART00==1))
							{
								mcu_flag|=ADC_NO_LOAD_FIRST_STAGE;
								start_Timer8bit0(DSP_SLOW_MODE_INTERVAL);
							}
						}
						else
						{
							if(adc_switch_wait_time>DSP_NO_LOAD_SECOND_STAGE_WAIT_TIME)
							{
								if((DSP_NO_LOAD_SECOND_STAGE_WAIT_TIME!=(TRTCMP0/256))&&(TSTART00==1))
									start_Timer8bit0(DSP_NO_LOAD_MODE_INTERVAL);
							}							
						}
					}
					if(op_action_state&PFAIL_MODE_PULSE)
					{
						
						if(Ins.EffectiveI==0)
						{
							if((CurrentTamperStatusFlag&TAMPER_MAG)==0)
							{
								activePulsePower=0;
								mcu_flag&=~START_PULSE_NM_MODE;
							}
						}
						
						Display(BATTERY_MODE);
					}					
				}
				op_state=PFAIL_MODE_SWITCH_PRESS_AWAIT_LCD;
				if((InsSave.timeCounter/86400)!=(prevTimeCounter/86400))
				{
					if(BATTERY_LCD_LOCK_COUNT==battery_mode_nm_on_count)
						battery_mode_nm_on_count=0;
					
				}
				
				break;
			//-----------------------------------------------------

			//-----------------------------------------------------
			case COVER_OPEN_ACTION:
				op_state=PFAIL_MODE_STOP;
				if((TamperRecord.TamperStatusFlag & TAMPER_COVEROPEN)==0)// if cover open is not there
				{
					while(GET_COVER_STATUS)	// check cover open pin 
					{
						if(TamperTime_counter[7]++>=COVER_OPEN_WAIT_TIME)
						{
							CurrentTamperStatusFlag |= TAMPER_COVEROPEN;
							getRtcCounter(RTC_CHECK_MODE);
							Tamperlog();
							if((op_action_state&PFAIL_MODE_LCD)==0)
							{
								init_Timer8bit1(LCD_SWITCH_ON_WAIT_TIME);
								Clear_Lcd();
								R_LCD_PowerOn();
								setParaBuff(DISPLAY_MODE_UOUT_NORMAL);
								display_no_of_para=3;
								op_action_state|=PFAIL_MODE_LCD;
								op_action_state|=SWITCH_ON_LCD;
								mcu_flag|=PUSH_RELEASED_NM_MODE;
								
							}
							//setNextParaSeq();
							display_no_of_para_count=0;
							mcu_flag|=EIGHT_BIT_TIMER1_FLAG;
							op_state=SWITCH_ON_LCD;
							break;
						}
					}
				}
				else if(TamperRecord.TamperCount[7]>=5)
					CurrentTamperStatusFlag |= TAMPER_COVEROPEN;
				else if((TamperRecord.TamperStatusFlag & TAMPER_COVEROPEN)&&GET_COVER_STATUS)
					CurrentTamperStatusFlag |= TAMPER_COVEROPEN;
				else if(GET_COVER_STATUS==0)
				{
					CurrentTamperStatusFlag &= ~TAMPER_COVEROPEN;
					if(TamperRecord.TamperStatusFlag & TAMPER_COVEROPEN)
					{
						TamperTime_counter[7]=COVER_OPEN_WAIT_TIME;
						Tamperlog();
					}
				}
					
			
				break;
			//-----------------------------------------------------
			
			default:
				if(op_state!=EXIT_PFAIL_OPERATION)
					op_state=PFAIL_MODE_STOP;
				break;
		}
	}while(op_state!=EXIT_PFAIL_OPERATION);

	isRAMOK();	
	if(nowNMkWh>0)
	{
		
		if((mcu_flag&SAVE_AT_NM_EXIT_FLAG)&&((nowNMkWh-prekWh)<=LIMIT_DIFF_THRESHOLD))
		{
			saveLSatExit();
			SaveEEPROM(PARA_WRITE_AFTER_STATE  ,PARA_TYPE_INSTANT); // save at after location
			InsSave.timeCounter=prevTimeCounter;
			SaveEEPROM(PARA_WRITE_AFTER_STATE ,PARA_TYPE_PFAIL_INSTANT); // save at current location
			getSetPfailData(ENERGY_W_RAM);
		}
	}
	
	
	
	if(battery_mode_nm_on_count>0)
		write_eeprom((uint8_t *)&battery_mode_nm_on_count,BAT_NM_LOCK_COUNT_LOC,1);

	#if (defined(LOG_NM_INFO) && (LOG_NM_INFO == 1))
		write_eeprom((uint8_t *)&nmInfo,NM_INFO_SHT_LOC,12);
	#endif	
	RxTxBuffer[0]=0xAB;
	write_eeprom(RxTxBuffer,RTC_BAT_STATUS_LOC,1);
	
	frequency_sample_count=0;
	if(is_vfail_with_load==0)
		WDT_Restart(0x00);
}



void updateEnergyPulsePara(uint8_t mVal,uint32_t current)
{
	apperentPowerNM=REFERENCEVOLTAGE*current*mVal;
	activePowerNM=apperentPowerNM;

}
void addTime2MagCounter(void)
{
	if(TamperTime_counter[2]==0)
		TamperTime_counter[2]=DSP_FAST_MODE_INTERVAL;
	else
		TamperTime_counter[2]+=(TRTCMP0/256);
}
#endif

void addPulsesActive(uint16_t no_of_pulses)
{
	InsSave.ZkWhCounter+=no_of_pulses;
	InsSave.kWhCounter10m+=(no_of_pulses*10);
	no_of_pulses_in_nm_Active=0;
	
}
void addPulsesApp(uint16_t no_of_pulses)
{
	InsSave.ZkVAhCounter+=no_of_pulses;
	InsSave.kVAhCounter10m+=(no_of_pulses*10);
	no_of_pulses_in_nm_App=0;	
}
void updatePfailEvent(void)
{
	uint16_t EventEntries;
	if(InsSave.PFailCount>PFAIL_EVENTS )
		EventEntries = PFAIL_EVENTS*2;
	else
		EventEntries = InsSave.PFailCount*2;
	
	g_Class07_Event_EntriesInUse[1]=EventEntries;

}
