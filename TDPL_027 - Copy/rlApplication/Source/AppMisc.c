//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dRtc.h"
#include "..\\..\\rlDevice\\Include\\dWatchDog.h"
#include "..\\..\\rlDevice\\Include\\dIOCtrl.h"
#include "..\\..\\rlDevice\\Include\\d8bitTimer.h"
#include "..\\..\\rlDevice\\Include\\d12bitTimer.h"
#include "..\\..\\rlDevice\\Include\\dDisplay.h"
#include "..\\..\\rlDevice\\Include\\dUser_includes.h"
#include "..\\..\\rlDevice\\Include\\dMisc.h"
//-----------------------------------------------------

//-------- include app specific files----------------
#include "..\\include\\AppVariables.h"
#include "..\\include\\AppLcd.h"
#include "..\\include\\AppEeprom.h"
#include "..\\include\\AppMacros.h"
#include "..\\include\\AppConfig.h"
#include "..\\include\\AppParaLimits.h"
#include "..\\include\\AppMisc.h"

//----------------------------------------------------

const uint16_t SEASON_LOC[][2]={{SEASON_1_KWH_BEFORE,SEASON_1_KWH_AFTER},{SEASON_2_KWH_BEFORE,SEASON_2_KWH_AFTER}};
uint32_t ConvertTimeCounterToTime(uint32_t TimeSeconds, uint8_t Flag,Apprtc * rtc)
{   
	uint32_t tempvalue;
    uint32_t DaySeconds;
    uint8_t count;      
    uint8_t current_hours,current_minutes,current_seconds;
    uint8_t current_week_day,current_month,current_day;
    uint16_t current_year;  //yr is taken as integer since later v may choose it to display frm 2000 instead of frm 00.
       
    DaySeconds=(TimeSeconds)%(86400L); //seconds in the current day
    current_hours=DaySeconds/3600;    //getting the hours of the current day
    tempvalue=DaySeconds%3600;    //getting the sec in the current hour
    current_seconds=tempvalue%60;   //getting the seconds of the current day
    current_minutes=tempvalue/60;   //getting the minutes of the current day               
   
    tempvalue=TimeSeconds/86400L;   //now total no of days is available  (for maximum 180 years) 
    current_week_day=(tempvalue+6)%7;
      
    current_year=0; 
    count=1;
 
	while(tempvalue>=365)   
	{   
		if(count==1) 
		{   
			if(tempvalue>365)
				tempvalue-=366;
			else  
				break;
		}
		else  
			tempvalue-=365;

		current_year++;
		if(count==4) 
			count=0;
			
		count++;
	}   
        
   //calculating the no. of months,day of the month:
   //So the tempvalue now contain only the months and day of month information.
          
    tempvalue+=1;
    count=31;        //count=no of days in a month
    current_month=1; 
    while(tempvalue>count)             //  then only we should increment month
    {   
		current_month++;
        tempvalue=tempvalue-count;
        switch(current_month)
        {//--------------------------------
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                count=31;
                break;
            case 4:
            case 6:
            case 9:
            case 11:      
                count=30;
                break;
            default:      
                if( (current_year %400==0) ||( (current_year%4==0) && (current_year %100!=0) ) )
                    count=29;
                else
                    count=28;
            break;  
        }
     }
     
    if(tempvalue==0)
        tempvalue=1;
    current_day=tempvalue;     
    
    if(TimeSeconds==0)          //for time counter 0 i return the time 0 instead
    {                           //of 1:1:0 since for record where data is not present
        current_seconds=0;       //and data is to be shown,0:0:0 will show that data
        current_minutes=0;       //does not exist
        current_hours=0;
        current_day=0;
        current_month=0;
        current_year=0;   
        current_week_day=0;     
    }    
	if(0!=rtc)
	{
	    rtc->sec=current_seconds;
	    rtc->min=current_minutes;
	    rtc->hour=current_hours;
		if(current_day==0)
		{
			rtc->day=1;
			rtc->month=1;
			rtc->week=6;
		}
		else
		{
	    	rtc->day=current_day;
	    	rtc->month=current_month;
		    rtc->week=current_week_day;
		}
		rtc->year=current_year;   
	}
    
    if(Flag==TIME_VAL)
        return current_hours*10000L+current_minutes*100L+current_seconds;
    else if(Flag==DATE_VAL)
        return current_day*10000L+current_month*100L+current_year;   
    else if(Flag==MONTH_VAL)
        return current_month;
    else if(Flag==YEAR_VAL)
        return current_year;
    else 
        return 0;
 }

/*********************** Convert time format to 4-bytes *********************/
uint32_t ConvertTimeToTimeCounter(Apprtc * rtc)
{   
	uint32_t Total_NoOfDays=0;
    uint32_t Counter=0;

    if(rtc->year>0)
        Total_NoOfDays=((rtc->year-1)>>2)+1;  //no of leap years
    
    switch(rtc->month)
    {//------------------------------------ 
        case  12:
            Total_NoOfDays+=30;

        //------------------------------------ 
        case  11:
            Total_NoOfDays+=31;

        //------------------------------------ 
        case  10:
            Total_NoOfDays+=30;

        //------------------------------------ 
        case  9:
            Total_NoOfDays+=31;

        //------------------------------------ 
        case  8:
            Total_NoOfDays+=31;

        //------------------------------------ 
        case  7: 
            Total_NoOfDays+=30;

        //------------------------------------ 
        case  6:
            Total_NoOfDays+=31;

        //------------------------------------ 
        case  5:
            Total_NoOfDays+=30;

        //------------------------------------ 
        case  4:
            Total_NoOfDays+=31;

        //------------------------------------ 
        case  3:
            Total_NoOfDays+=28;

        //------------------------------------ 
        case  2:
            Total_NoOfDays+=31;

        //------------------------------------ 
        case  1:
            Total_NoOfDays+=0;

        //------------------------------------ 
        default:     
            break;  
    }
    if(rtc->month>2)
    {   
        if( (rtc->year %400==0) || ((rtc->year%4==0) && (rtc->year %100!=0)) )
        {   
            Total_NoOfDays+=1;
        }
    }
    
    if(rtc->day>0)
    {   Total_NoOfDays+=rtc->day-1;
    }
    Counter = ((rtc->year*365L) + Total_NoOfDays)*86400;
    Counter += ((rtc->hour*3600L)+(rtc->min*60) + rtc->sec);
    
    return Counter;
}

uint16_t Crc_Cal(const uint8_t * str,uint16_t length)
{   
    uint8_t j;
	uint16_t i;
    uint16_t CheckSum=0xffff;
	
    for (i=0; i<length; i++)
    {   
		CheckSum ^= (uint8_t)str[i];
        for (j=0; j<8; j++)
        {   
			if (CheckSum & 1)
            {   
				CheckSum >>= 1;
                CheckSum ^= 0xA001;
            }
            else
                CheckSum >>= 1;
        }
    }
    return CheckSum;
}

//uint16_t Crc_Cal_Advanced(const uint8_t * str,uint8_t length,uint16_t prevCRC)
//{   
//    uint8_t i,j;
//    uint16_t CheckSum=prevCRC;
	
//    for (i=0; i<length; i++)
//    {   
//		CheckSum ^= (uint8_t)str[i];
//        for (j=0; j<8; j++)
//        {   
//			if (CheckSum & 1)
//            {   
//				CheckSum >>= 1;
//                CheckSum ^= 0xA001;
//            }
//            else
//                CheckSum >>= 1;
//        }
//    }
//    return CheckSum;
//}

void SaveEEPROM(uint8_t state,uint8_t paramType)
{
  uint16_t crc,currentloc,beforeloc,length;
  uint8_t *p;

  	
	switch(paramType)
	{
		case PARA_TYPE_INSTANT:
			currentloc=INS_SAVE_AFTER_LOC;
			beforeloc=INS_SAVE_BEFORE_LOC;
			length=STRUCT_INSSAVE_SIZE;
			p=(uint8_t *)&InsSave.CumkWh;
		break;
		
		case PARA_TYPE_TAMPER:
			currentloc=TAMPER_SAVE_AFTER_LOC;
			beforeloc=TAMPER_SAVE_BEFORE_LOC;
			length=BACKUPTAMPER_LEN;
			p=(uint8_t *)&TamperRecord.TamperCount;
			break;
		
		case PARA_TYPE_PFAIL_INSTANT:
			currentloc = PFAIL_INS_SAVE_LOC;
			length=PFAILSAVEDATA_LEN;
			p=(uint8_t *)&InsSave.timeCounter;
		break;
	}
  
	switch(state)
	{
		case 0:

			read_eeprom(p,currentloc,length);
			crc = Crc_Cal(p,length);
			if(crc!=0)
			{
				read_eeprom(p,beforeloc,length);
			}
		break;
			
		case 1://para write before state
			crc = Crc_Cal(p,length-2);
			p[length-2] = crc & 0x00ff;
			p[length-1] = crc / 256; 
			write_eeprom(p,beforeloc,length );
		break;
		
		case PARA_WRITE_AFTER_STATE://para write after state
		case PARA_TYPE_DAILY_STATE:
			crc = Crc_Cal(p,length-2);
			p[length-2] = crc & 0x00ff;
			p[length-1] = crc / 256; 
			write_eeprom(p,currentloc,length );
		break;		

	}
  
}

void makeByte(uint32_t data,uint8_t startByte, uint8_t noOfBytes)
{
	uint8_t i=0;
	for(i=0;i<noOfBytes;i++)
	{
		RxTxBuffer[startByte+i]=data;
		data>>=8;
	}
  
}

uint32_t getByteValue(const uint8_t *bptr,uint8_t startByte, uint8_t noOfBytes)
{
	uint32_t byte4Val=0;
	
	while(noOfBytes--)
	{
		byte4Val+=bptr[startByte+noOfBytes];
		if(noOfBytes>0)
			byte4Val<<=8;
	}
	
	return byte4Val;
  
}
uint8_t isPowerfailLog(uint8_t powerMode)
{
	uint8_t status=0;
	if((GET_MAINS_STATUS)||(powerMode==BATTERY_MODE))
		if((InsSave.timeCounter>=prevTimeCounter)&&(prevTimeCounter>0)) // if POWER_FAIL_PERSIT_TIME seconds are over
		{
			if((InsSave.timeCounter-prevTimeCounter)>=POWER_FAIL_PERSIT_TIME)
				status=1;
			InsSave.CumPowerOffDuration+=(InsSave.timeCounter-prevTimeCounter);	
		}
			
	return status;
	
}		
void powerfailLogging(void)
{
		
	getRtcCounter(RTC_NO_CHECK_MODE);
	makeByte(prevTimeCounter,0,4);
	makeByte(InsSave.timeCounter,4,4);

	write_eeprom(RxTxBuffer,POWER_FAIL_LOC+(InsSave.PFailCount%PFAIL_EVENTS)*8,8);

	InsSave.PFailCount++;

}


void isRAMOK(void)
{
	uint8_t status=1;
	if('R'!=ramCHECK1)
		status=0;
	if('A'!=ramCHECK2)
		status=0;
	if('M'!=ramCHECK3)
		status=0;
	if('O'!=ramCHECK4)
		status=0;
	if('K'!=ramCHECK5)
		status=0;

	if(status==0)
		WDT_Restart(0x00);
	islimitOverflow();
}

uint16_t updateMDInterval(uint8_t ioption, uint16_t md_current_value)
{
	uint32_t md_value=0;
	if(MD_PERIOD_CHANGE_FLAG==ioption)
	{
		read_eeprom((uint8_t*)&md_value,UPDATE_MD_INTERVAL_LOC,4);
		if(Crc_Cal((uint8_t*)&md_value,4)==0)
		{
			md_current_value=md_value&0xFFFF;
			write_eeprom((uint8_t*)&md_current_value,MD_IP_LOC,2);
			md_current_value=3600/md_current_value;
		}
	}
	else if(MD_PERIOD_SET_FLAG==ioption)
	{
		makeByte(md_current_value,0,2);
		md_value = Crc_Cal((uint8_t*)&md_current_value,2);
		makeByte(md_value,2,2);
		
		write_eeprom(RxTxBuffer,UPDATE_MD_INTERVAL_LOC,4);
	}
	else if(MD_PERIOD_READ_FLAG==ioption)
	{
		read_eeprom((uint8_t*)&md_value,UPDATE_MD_INTERVAL_LOC,4);
		if(Crc_Cal((uint8_t*)&md_value,4)==0)
		{
			md_current_value=md_value&0xFFFF;
			
			switch(md_current_value)
			{
				case 3600:
					md_current_value=1;
				break;		
				case 1800:
					md_current_value=2;
				break;		
				case 900:
					md_current_value=4;
				break;		
				default:
					md_current_value=md_period;
				break;				
			}
			
		}
		
	}
	
	return md_current_value;
}


//void Switch2FilClock(void)
//{
//	uint8_t is_12bit_timeron=0;
//	if((power_fail_state&OSD_FAIL_ACTION_DONE)==0)
//	{
//		if(power_fail_state&RTC_BAT_STATUS_FLAG)
//		{
//			DI();
			
			
//			power_fail_state|=OSD_FAIL_ACTION_DONE;
//			if(TMKAEN==1)
//				is_12bit_timeron=0x01;
				
//			ITMC&=~0x80;	// stop 12 bit timer
//			TMKAEN=0;
//			if(TSTART00==1)
//				Data.Short.lsb=TRTCMP0;
//			else
//				Data.Short.lsb=0;
//			if(TSTART10==1)
//				Data.Short.msb=TRTCMP1;
//			else
//				Data.Short.msb=0;
				
//			stop_Timer8bit0();
//			stop_Timer8bit1();
//			MCU_Delay(100);
//			OSMC|=0x10;
//			SCOC=0;
//			LCDM0 &= ~_40_LCD_VOLTAGE_MODE_INTERNAL;
//			LCDM0 |= _80_LCD_VOLTAGE_MODE_CAPACITOR;
//			SCOC=1;
			
//			LCDC0 	= _05_LCD_CLOCK_FSX_FIL_6;
			
//			MCU_Delay(10);
			
//			if(is_12bit_timeron==1)
//				init_Timer12bit(1);
//			if(Data.Short.lsb!=0)	
//				start_Timer8bit0(Data.Short.lsb/256);
//			if(Data.Short.msb!=0)
//				start_Timer8bit1(Data.Short.msb/256);
				
//			EI();

//		}
//	}
//}

//uint32_t saveSeasonkWh(uint8_t seasonState,uint8_t seasonNO)
//{
	
//	uint16_t loc=SEASON_LOC[seasonNO][0];
//	uint16_t crc;
//	uint8_t i;
//	uint32_t sum=0;
//	if(seasonState==1)
//		loc=SEASON_LOC[seasonNO][1];
	
//	if((seasonState==0)||(seasonState==1))
//	{
//		for(i=0;i<8;i++)
//		{
//			makeByte(InsSave.TODEnergy.TOD[i].kWh,i*8,4);
//			makeByte(InsSave.TODEnergy.TOD[i].kVAh,i*8+4,4);
//		}
//		write_eeprom(RxTxBuffer,loc,64);
//		crc = Crc_Cal(RxTxBuffer,64);
//		write_eeprom((uint8_t*)&crc,loc+64,2);
//	}
//	else if((seasonState==3)||(seasonState==4)||(seasonState==5))
//	{
//		loc=SEASON_LOC[seasonNO][1];
//		read_eeprom(RxTxBuffer,loc,64);
//		read_eeprom((uint8_t*)&crc,loc+64,2);
//		if(crc!=Crc_Cal(RxTxBuffer,64))
//		{
//			loc=SEASON_LOC[seasonNO][0];
//			read_eeprom(RxTxBuffer,loc,64);
//			read_eeprom((uint8_t*)&crc,loc+64,2);
//		}
		
//		for(i=0;i<8;i++)
//		{
//			if(seasonState==3)
//			{
//				InsSave.TODEnergy.TOD[i].kWh=getByteValue(RxTxBuffer,i*8,4);
//				InsSave.TODEnergy.TOD[i].kVAh=getByteValue(RxTxBuffer,i*8+4,4);
//			}
//			else
//			{
//				if(seasonState==4)
//					sum+=getByteValue(RxTxBuffer,i*8,4);
//				else if(seasonState==5)
//					sum+=getByteValue(RxTxBuffer,i*8+4,4);
//			}
//		}
		
//	}
//	return sum;
//}
uint8_t getSetPfailData(uint8_t wrOption)
{
uint16_t crc;
uint8_t *p=(uint8_t *)&InsSave.timeCounter;
uint8_t status=1;
	// add crc
	if(wrOption==ENERGY_W_RAM)
	{
		// add crc to ram data
		crc = Crc_Cal(p,PFAILSAVEDATA_LEN-2);
		p[PFAILSAVEDATA_LEN-2] = crc &0x00FF;
		p[PFAILSAVEDATA_LEN-1] = crc >>8;
		crc=0;		
	}
	else if(wrOption==ENERGY_R_RAM)
	{
		// verify crc of ram data
		crc = Crc_Cal(p,PFAILSAVEDATA_LEN);
		if(crc)
		{
			read_eeprom(RxTxBuffer,PFAIL_INS_SAVE_LOC,PFAILSAVEDATA_LEN);
			status=0;
		}
		
	}
	return status;
	
}

uint32_t getCumPowerOffDuration(void)
{
	return InsSave.CumPowerOffDuration;
}

void fillComBufferZero(void)
{
	uint16_t bytes;
	for(bytes=0;bytes<MAX_COMM_BUFFER_LENGTH;RxTxBuffer[bytes++] = 0x00);    // set zero
}
void getAvgTem(void)
{
	if(InsSave.PowerOn30!=0)
		InsSave.lsAvgT=InsSave.lsAvgT/InsSave.PowerOn30;
	
}
uint8_t isIntevalCrossed(uint32_t interval)
{
	uint8_t status=0;
	if((InsSave.timeCounter/interval)!=(prevTimeCounter /interval))
		status= 1;
		
	return status;
}
void makeAvgVals(uint8_t seconds)
{
	
}