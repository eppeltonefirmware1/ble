

	 
#define EEP_PAGE_LENGTH         64

#define CAL_CONST				0


#define PRE_BILL_SINGLE_ACTION	54		// 4 single action date,1- execution times
#define PASSIVE_SEA_ACT_TIME	59		// 4 bytes activation date,1- activation date set indication 0xAB
#define PASSIVE_ZONE_TIME       64     	//(((2*8=16) bytes)=16) bytes  
#define PASSIVE_ZONE_NO         80     	// 1 byte Max Zone No.+8 zones's no= 9 bytes
#define PASSIVE_ZONE_TIME2      89     	//(((2*8=16) bytes)=16) bytes  
#define PASSIVE_ZONE_NO2        105     // 1 byte Max Zone No.+8 zones's no= 9 bytes
#define PASSIVE_ZONE_SEASON     114     // 2 (4 bytes)season+ 2 season no=7 bytes  


#define DLMS_LLS_SECRET_ADDR 	140    //18=(16 byte+2 bytes crc )
#define DLMS_HLS_SECRET_ADDR	158    //18=(16 byte+ 2 bytes crc)
#define TOD_CALANDER_NAME		176		// 64 (Calander Name+ Season Name)
#define TOD_PCALANDER_NAME		240		// 64 (Calander Name+ Season Name)

#define BAT_NM_LOCK_COUNT_LOC	304

#define SERIAL_NO_LOC           960     //20 bytes = 16 bytes serial number including :+ 2 bytes mfg month, year
#define MD_IP_LOC               1076     //2 bytes
#define TAMPER_LOG_TIME_LOC     1078     //16 bytes
#define TAMPER_REC_TIME_LOC     1094     //16 bytes
#define LS_IP                 	1110     //1 bytes
#define ZONE_TIME               1111     //(((2*8=16) bytes)=16) bytes  
#define ZONE_NO                 1127     // 1 byte Max Zone No.+8 zones's no= 9 bytes
#define ZONE_TIME2              1136     //(((2*8=16) bytes)=16) bytes  
#define ZONE_NO2                1152     // 1 byte Max Zone No.+8 zones's no= 9 bytes
#define ZONE_SEASON             1161     // 2 (4 bytes)season+ 2 season no=7 bytes  
#define PRE_BILL_DATE           1168     //2 bytes =1 byte for day + 1byte billing types-0- Each,1 -Odd,2-Even 

#define CAL_CONST_2				1216
#define PROD_STATUS_LOC			1246
#define INTERVAL_KWH_LOC       	1256


#define PFAIL_INS_SAVE_LOC      1984   // 60 bytes
#define INS_SAVE_BEFORE_LOC     2048    //300 bytes
#define TAMPER_SAVE_BEFORE_LOC  2366    //66 bytes


#define INS_SAVE_AFTER_LOC      2432    //300 bytes
#define TAMPER_SAVE_AFTER_LOC   2750    //66 bytes


#define BILL_DATA_LOC           2816   	//38*12=456)

#define TOD_BILL_LOC            3328   	//(161*12=1932) bytes 



#define SEASON_1_KWH_BEFORE		5312	//192 bytes
#define SEASON_1_KWH_AFTER		5504	// 192 bytes

#define SEASON_2_KWH_BEFORE		5696	//192 bytes
#define SEASON_2_KWH_AFTER		5888	// 192 bytes


#define TAMPER_C_RELATED_LOC    6464   	//(85*25=1500  ~=2176)
#define TAMPER_OTHER_LOC		8640    //(125*25=3750  ~=3136)
#define TAMPER_NON_ROLLOVER_LOC 11776   //(5*25=125  ~=128)
#define TAMPER_U_RELATED_LOC	11904  //(25*25=625=~640)



#define NM_INFO_SHT_LOC			12544
#define NM_INFO_CT_LOC			12448
#define NM_INFO_FALSE_LOC		12452
#define IDISPLAY_MODE_LOC		12456	// 4 bytes (2 bytes display mode + calculation +2 bytes crc)
#define UPDATE_MD_INTERVAL_LOC	12464	// 4 bytes (2 bytes md + 2 bytes CRC)
#define RTC_BAT_STATUS_LOC		12468
#define METER_ON_REF_DT_LOC		12472	// 6 bytes (4 rtc ref time for off time calculation +2 bytes crc)
#define BLE_ID_WCOUNT_LOC		12478



#define SOFT_CHANGE_LOG_LOC     12736   //(5*20=100)	bytes
#define POWER_FAIL_LOC          12864   //(100*8)=832

#define DAILY_KWH_LOC           13696   //(20*90=1856)bytes
#define LOAD_SURVEY_LOC         15552   //(484*90) = 43584

#define EEP_START_LOC           0//INTERVAL_KWH_LOC

#define EEP_END_LOC             59200
 	

