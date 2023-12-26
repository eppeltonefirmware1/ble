//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dI2c.h"
//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppVariables.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppMacros.h"
#include "..\\Include\\AppTampers.h"
#include "..\\Include\\AppConfig.h"


uint8_t doCalanderAction(void)
{
	
	if(InsSave.timeCounter>=ScheduleActionDateTime.CalanderDateTime )
	{
		setParamIndex(MSG_ACT00_INDEX,1);
		Display(USER_CODE);
		read_eeprom((uint8_t *)&RxTxBuffer,PASSIVE_ZONE_TIME,57);
		write_eeprom((uint8_t *)&RxTxBuffer,ZONE_TIME,57);
		RxTxBuffer[0]=0xFF;
		write_eeprom(RxTxBuffer,PASSIVE_SEA_ACT_TIME+4,1);

		mcu_flag &=~ PASSIVE_CAL_CHECK_FLAG;
		
		InsSave.timeCounter=ScheduleActionDateTime.CalanderDateTime;
		return RECORD_DO_BILL_DATA;
		
	}
	else
		return 0;
		
}

uint8_t doBillSingleAction(void)
{
	if(InsSave.timeCounter>=ScheduleActionDateTime.BillDateTime )
	{
		setParamIndex(MSG_ACT01_INDEX,1);
		Display(USER_CODE);
		mcu_flag &=~ SINGLE_ACTION_CHECK_FLAG;
		RxTxBuffer[0] =0xFF;
		write_eeprom((uint8_t *)&RxTxBuffer,PRE_BILL_SINGLE_ACTION+4,1);
		InsSave.timeCounter=ScheduleActionDateTime.BillDateTime;
		return RECORD_DO_BILL_DATA;
	}
	else
		return 0;
}