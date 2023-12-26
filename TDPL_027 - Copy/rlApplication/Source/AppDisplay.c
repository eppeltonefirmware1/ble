//-------- include device specific files----------------
#include "..\\..\\rlDevice\\Include\\dDisplay.h"
#include "..\\..\\rlDevice\\Include\\dIOdefine.h"
#include "..\\..\\rlDevice\\Include\\dTypedef.h"
#include "..\\..\\rlDevice\\Include\\dDeltaSigmaADC.h"
#include "..\\..\\rlDevice\\Include\\dI2c.h"
#include "..\\..\\rlDevice\\Include\\dIOCtrl.h"
//-----------------------------------------------------

//-------- include app specific files-------------------
#include "..\\Include\\AppLcd.h"
#include "..\\Include\\AppEeprom.h"
#include "..\\Include\\AppMisc.h"
#include "..\\Include\\AppBilling.h"
#include "..\\Include\\AppConfig.h"
#include "..\\Include\\AppTampers.h"
#include "..\\Include\\AppTOD.h"
#include "..\\Include\\AppAccessProd.h"
//------------------------------------------------------
extern uint16_t TAMPER_EVENT_OCC_TIME[MAX_TAMPER_COUNT];

#if (defined(LCD18D) && (LCD18D == 1))
	#if (defined(LCD18A) && (LCD18A == 1))
		#define OK_BIT		0x80
		
	#endif
	#if (defined(LCD18D) && (LCD18A == 0))
		#define OK_BIT		0x01
		
	#endif
	
#endif

#if (defined(LCD18C) && (LCD18C == 1))
	#define OK_BIT		0x80
	#define VBAT_BIT	0x20
#endif





#define LCD_DATA_REG_PTR	(uint8_t*)&SEG0

uint8_t pinConfig[10]={
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	16,
	17,
};


//0-Auto
//1-Push
//2-Cover Open in Normal Mode
//3- Power fail Normal
//4- Cover Open in Power fail
//5- production
//6- mode 3
//7- NM mode display
//8- production bush mode



const uint8_t lcdParamIDBuff0[][66]={
	//{2,0xFD,0xF9,21,0x29,0xF9,0x3F,0x93,0x0C,0x0B,0x00,0x15,0x0D,0x16,0x43,0x80,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{29,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0x0D,0x16,0x43,0x80,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{35,0x29,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0x0D,0x16,0x43,0x80,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x10,0x19,0x3A,0x94,0x95,0xD1,0xD2,0xD3,0x24,0x96,0x97,0x90,0x91,0x27,0x33},
	{29,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0x0D,0x16,0x43,0x80,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{29,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0x0D,0x16,0x43,0x80,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{29,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0x0D,0x16,0x43,0x80,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{8,0x29,0x0C,0x0B,0x01,0x04,0x07,0x09,0x6E},
	{2,0x10,0x19},
	{30,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15},
	{5,0x29,0x02,0x03,0x0A,0x23}
	
};
const uint8_t lcdParamIDBuff1[][68]={
	{61,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0x0D,0x16,0x43,0x80,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{67,0x29,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0x0D,0x16,0x43,0x80,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x10,0x19,0x3A,0x94,0x95,0xD1,0xD2,0xD3,0x24,0x96,0x97,0x90,0x91,0x27,0x33},
	{61,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0x0D,0x16,0x43,0x80,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{61,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0x0D,0x16,0x43,0x80,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{61,0x29,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0xF8,0x3F,0x93,0x0C,0x0B,0x00,0x15,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0x0D,0x16,0x43,0x80,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0x40,0x45,0x79,0x02,0x03,0x01,0x12,0x13,0x33},
	{9,0x29,0xFE,0x0C,0x0B,0x01,0x04,0x07,0x09,0x06E},
	{2,0x10,0x19},
	{30,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15,0x00,0x15},
	{4,0x29,0x02,0x03,0x0A}

};
const uint8_t *idisplayModePtr=lcdParamIDBuff0[0];
void getPdPointer(uint8_t imode);
#if (defined(IS_DISPLAY_PUSH_LOCK) && (IS_DISPLAY_PUSH_LOCK == 1))
struct 
{
	uint8_t push_lock_hold_time:7;
	uint8_t lock:1;
}display_lock;
void movebackLock(void);
#endif

void siwitch2Auto(void);
void getNextZoneReg(uint8_t startAddr,uint8_t endAddr);
uint16_t BleIDCount(void);

struct lcdmap
{   
	const uint8_t *Data;
    uint8_t eep_addr;
    uint8_t digits;
    uint8_t offset;
    uint8_t modifier;
    uint8_t divisor;
    uint8_t symbol1;
    uint8_t symbol2;
	uint8_t symbol3;
    uint8_t id;
	uint8_t txtid;
	uint8_t time;
	uint8_t startdigit;
};



const uint8_t txtmsg[][8]={
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	},	//0
{CHR_C	,CHR__	,CHR_O	,CHR_P	,CHR_E	,CHR_n	,NONE	,NONE	},	//1-C-open
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_h	},	//2-Ph
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_n	,CHR_U	},	//3-nU
{NONE	,CHR_A	,CHR_u	,CHR_t	,CHR_o	,NONE	,NONE	,NONE	},	//4-Auto
{NONE	,CHR_P	,CHR_u	,CHR_S	,CHR_h	,NONE	,NONE	,NONE	},	//5-Push
{CHR_E	,CHR_E	,CHR_P	,CHR_L	,CHR_0	,CHR_1	,NONE	,NONE	},	//6-EEPL01
#if (defined(TATA_POWER_TYPE_1) && (TATA_POWER_TYPE_1 == 1))
{CHR_r	,CHR_t	,CHR_4	,CHR_6	,CHR_8	,CHR_0	,CHR_2	,CHR_7 	},	//7-FORMAT 4.68.027
#else
{CHR_r	,CHR_t	,CHR_4	,CHR_6	,CHR_8	,CHR_1	,CHR_0	,CHR_0 	},	//7-FORMAT 4.68.100
#endif
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_F	},	//8-Pf
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_1	},//9-01
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_2	},//10-02
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_3	},//11-03
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_4	},//12-04
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_5	},//13-05
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_6	},//14-06
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_7	},//15-07
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_8	},//16-08
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_9	},//17-09
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_0	},//18-10
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_1	},//19-11
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_2	},//20-12
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_F	,CHR_r	},//21-Fr
{CHR_d	,CHR_O	,CHR_n	,CHR_E	,NONE	,NONE	,NONE	,NONE	},//22-Done
{CHR_F	,CHR_A	,CHR_I	,CHR_L	,NONE	,NONE	,NONE	,NONE	},//23-Fail
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_n	},//24-Sr
{0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	,0xFF	},//25-lcd check
{CHR_A	,CHR_C	,CHR_t	,CHR__	,CHR_0	,CHR_0	,NONE	,NONE	},//26-Act00
{CHR_A	,CHR_C	,CHR_t	,CHR__	,CHR_0	,CHR_1	,NONE	,NONE	},//27-Act01
{NONE	,NONE	,CHR_O	,CHR_n	,NONE	,NONE	,NONE	,NONE	},//28-On
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_O	,CHR_H	},//29-OH
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_1	},//30-Av-1
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_2	},//31-Av-2
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_3	},//32-Av-3
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_4	},//33-Av-4
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_5	},//34-Av-5
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_6	},//35-Av-6
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_7	},//36-Av-7
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_8	},//37-Av-8
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_H	,CHR_9	},//38-Av-9
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_0	},//39-Av-10
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_1	},//40-Av-11
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_1	,CHR_2	},//41-Av-12
{CHR_P	,CHR_U	,CHR_L	,CHR_S	,CHR_E	,NONE	,NONE	,NONE	},//42-Pulse
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_C	},//43-tc
{CHR_ml	,CHR_mr	,CHR_O	,CHR_d	,CHR_E	,NONE	,CHR_1	,NONE	},//44-Mode 1
{CHR_ml	,CHR_mr	,CHR_O	,CHR_d	,CHR_E	,NONE	,CHR_2	,NONE	},//45-Mode 2
{CHR_ml	,CHR_mr	,CHR_O	,CHR_d	,CHR_E	,NONE	,CHR_3	,NONE	},//46-Mode 3
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_1	},//47-t1
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_2	},//48-t2
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_3	},//49-t3
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_4	},//50-t4
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_5	},//51-t5
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_6	},//52-t6
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_7	},//53-t7
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_t	,CHR_8	},//54-t8
{CHR_A	,CHR_U	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_F	},//55-AvPf
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	},//56-E
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_1	},//57-E1
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_2	},//58-E2
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_3	},//59-E3
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_4	},//60-E4
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_5	},//61-E5
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_E	,CHR_6	},//62-E6
{CHR_n	,CHR_ml	,CHR_mr	,NONE	,NONE	,NONE	,NONE	,NONE	},//63-nm
{CHR_n	,CHR_d	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	},//64-nd
{CHR_r	,CHR_E	,CHR_U	,NONE	,NONE	,NONE	,NONE	,NONE	},//65-rev
{CHR_ml	,CHR_mr	,CHR_G	,NONE	,NONE	,NONE	,NONE	,NONE	},//66-mag
{CHR_E	,CHR_a	,CHR_r	,NONE	,NONE	,NONE	,NONE	,NONE	},//67-Ear
{CHR_E	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_n	,CHR_S	},//68-non solar
{CHR_N	,CHR_F	,CHR_A	,CHR_I	,CHR_L	,NONE	,NONE	,NONE	},//69-nFail
{CHR_n	,CHR_O	,CHR_C	,CHR_L	,CHR_r	,NONE	,NONE	,NONE	},//70-noClr
{NONE	,NONE	,NONE	,CHR__	,CHR__	,CHR__	,CHR_P	,CHR_F	},//71-Pf -.--
{CHR_n	,CHR_O	,CHR_t	,CHR_a	,CHR_P	,CHR_r	,CHR_t	,CHR_S	},//72-noTapr
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_P	,CHR_n	},//73- Pn
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_r	,CHR_C	},//74- rC
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_C	,CHR_u	},//75- Cu
{CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,NONE	,NONE	},//76-dots for date & time
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_o	,CHR_C	},//77- ID to
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_r	,CHR_C	},//78- ID tr
{CHR_I	,CHR_d	,NONE	,CHR__	,CHR__	,CHR__	,CHR_t	,CHR_o	},//79- id --- to
{CHR_I	,CHR_d	,NONE	,CHR__	,CHR__	,CHR__	,CHR_t	,CHR_r	},//80- id --- tr
{CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR_o	,CHR_C	},//81-dots for date & time to
{CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR__	,CHR_r	,CHR_C	},//82-dots for date & time tr
{NONE	,CHR_G	,CHR_O	,CHR_O	,CHR_d	,NONE	,CHR_b	,CHR_t	},//83- good bt
{NONE	,NONE	,CHR_b	,CHR_a	,CHR_d	,NONE	,CHR_b	,CHR_t	},//84- bad bt
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_o	,CHR_C	},//85- oC
{NONE	,NONE	,NONE	,NONE	,NONE	,NONE	,CHR_r	,CHR_C	},//86- rC
//{CHR_t	,CHR_a	,CHR_n	,CHR_P	,CHR_E	,CHR_r	,CHR_t	,CHR_S	},//87-TanPer
{CHR_t	,CHR_a	,CHR_n	,CHR_P	,CHR_E	,CHR_r	,NONE	,NONE	},//87-TanPer
{CHR_C	,CHR_O	,NONE	,NONE	,NONE	,NONE	,NONE	,NONE	},//88-CO
{NONE	,CHR_G	,CHR_O	,CHR_O	,CHR_d	,NONE	,CHR_r	,CHR_t	},//89- good rt
{NONE	,NONE	,CHR_b	,CHR_a	,CHR_d	,NONE	,CHR_r	,CHR_t	},//90- bad rt
{NONE	,CHR_G	,CHR_O	,CHR_O	,CHR_d	,NONE	,NONE	,NONE	},//91- good nvm
{NONE	,NONE	,CHR_O	,CHR_h	,NONE	,NONE	,NONE	,NONE	},//92- Oh
{NONE	,CHR_d	,CHR_n	,CHR_L	,CHR_d	,NONE	,NONE	,NONE	},//93- dnld
{NONE	,CHR_E	,CHR_S	,CHR_d	,NONE	,NONE	,NONE	,NONE	},//94- ESd
{NONE   ,NONE	,CHR_L	,CHR_O	,CHR_C	,CHR_d	,NONE	,NONE	}, //95-locd
{CHR_u	,CHR_n  ,CHR_L	,CHR_O	,CHR_C	,CHR_d	,NONE	,NONE	}, //96-unlocd
{NONE	,NONE   ,CHR_E	,CHR_P	,NONE	,NONE	,NONE	,NONE	} //97- serial prefix



};


#if (defined(LCD18D) && (LCD18D == 1))
const struct lcdmap disp[] = {

	
{RAM_KWH		,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_100	,DISP_k+DISP_W		,DISP_h+DISP_CUM		,NONE						,0x00	,0x00	,0x0A,0x02},	//kWh
{RAM_VOLT   	,0xFF 	,DIGIT_3  	,NONE  	,SHORT_VAL	,DIV_1 		,NONE           	,DISP_V					,DISP_DEC3       			,0x01	,0x00	,0x0A,0x02},	//U
{RAM_PH_I   	,0xFF 	,DIGIT_4  	,NONE  	,LONG_VAL	,DIV_1     	,NONE				,DISP_A 				,DISP_DEC5					,0x02	,0x02	,0x0A,0x03},	// Ph Current
{RAM_NU_I   	,0xFF 	,DIGIT_4  	,NONE  	,LONG_VAL	,DIV_1     	,NONE				,DISP_A 				,DISP_DEC5					,0x03	,0x03	,0x0A,0x03},	// Nu Current
{RAM_EFF_I  	,0xFF 	,DIGIT_4  	,NONE  	,LONG_VAL	,DIV_1     	,NONE				,DISP_A 				,DISP_DEC5					,0x04	,0x00	,0x0A,0x03},	// Eff. Current
{RAM_PH_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1  	,DISP_W         	,NONE					,DISP_DEC3					,0x05	,0x02	,0x0A,0x02},	// Ph Power W
{RAM_NU_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1  	,DISP_W         	,NONE					,DISP_DEC3					,0x06	,0x03	,0x0A,0x02},	// Nu Power W
{RAM_EFF_P  	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1  	,DISP_W         	,NONE					,DISP_DEC3					,0x07	,0x00	,0x0A,0x02},	// Eff. Power W
{RAM_S  		,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1  	,DISP_W         	,DISP_V+DISP_A			,DISP_DEC3					,0x08	,0x00	,0x0A,0x02},	// Eff. Power kVA
{RAM_PF     	,0xFF  	,DIGIT_3  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE          		,NONE					,DISP_DEC3                  ,0x09	,0x08	,0x0A,0x02},	// Pf.
{RAM_FREQ   	,0xFF  	,DIGIT_3    ,NONE  	,SHORT_VAL	,DIV_1     	,NONE          		,NONE					,DISP_DEC3                  ,0x0A	,0x15	,0x0A,0x02},	//FRQ
{RAM_TIME   	,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_TIME  		,DISP_DOTS				,NONE 	                    ,0x0B	,0x00	,0x0A,0x02},	//TIME
{RAM_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_DATE  		,DISP_DOTS				,NONE 	                    ,0x0C	,0x00	,0x0A,0x02},	//Date
{RAM_MD_kW  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_10  	,DISP_k+DISP_W		,DISP_MD				,DISP_DEC3  				,0x0D	,0x00	,0x0A,0x02},	// Current kW MD 
{RAM_kW_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_TIME+DISP_k+DISP_W  		,DISP_MD+DISP_DOTS				,NONE 	   	                ,0x0E	,0x00	,0x0A,0x02},	//Current kW MD TIME
{RAM_kW_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_DATE+DISP_k+DISP_W  		,DISP_MD+DISP_DOTS				,NONE 	  					,0x0F	,0x00	,0x0A,0x02},	//Current kW MD Date
{RAM_KWH		,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k+DISP_W		,DISP_h+DISP_CUM		,DISP_DEC5					,0x10	,0x00	,0x0A,0x02},	//High ResolutionkWh
{RAM_VOLT   	,0xFF 	,DIGIT_1  	,NONE  	,SHORT_VAL	,DIV_1 		,NONE           	,NONE					,NONE       				,0x11	,0x00	,0x20,0x02},	//Erase Counter
{RAM_PH_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,DISP_k|DISP_W      ,NONE					,DISP_DEC3					,0x12	,0x02	,0x0A,0x02},	// Ph Power kW
{RAM_NU_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,DISP_k|DISP_W      ,NONE					,DISP_DEC3					,0x13	,0x03	,0x0A,0x02},	// Nu Power kW
{RAM_EFF_P  	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,DISP_k+DISP_W     	,NONE					,DISP_DEC3					,0x14	,0x00	,0x0A,0x02},	// Eff. Power kW

{RAM_KVAH		,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_100	,DISP_k				,DISP_h|DISP_V+DISP_A|DISP_CUM	,NONE				,0x15	,0x00	,0x0A,0x02},	//kVAh
{RAM_MD_kVA  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_10  	,DISP_k				,DISP_V+DISP_A+DISP_MD	,DISP_DEC3					,0x16	,0x00	,0x0A,0x02},	// Current kVA MD
{RAM_kVA_DATE   ,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_TIME+DISP_k	,DISP_MD+DISP_DOTS+DISP_V+DISP_A			,NONE						,0x17	,0x00	,0x0A,0x02},	//Current kVA MD TIME
{RAM_kVA_DATE   ,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_DATE+DISP_k	,DISP_MD+DISP_DOTS+DISP_V+DISP_A			,NONE 						,0x18	,0x00	,0x0A,0x02},	//Current kVA MD Date
{RAM_KVAH		,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k				,DISP_h+DISP_V+DISP_A|DISP_CUM	,DISP_DEC5					,0x19	,0x00	,0x0A,0x02},	//High resolution kVAh

//{RAM_EXPKWH		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_k+DISP_W		,DISP_h+DISP_CUM		,NONE						,0x1A	,0x38	,0x0A,0x02},	//kWh
//{RAM_EXPKVAH	,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_k				,DISP_V+DISP_A			,NONE						,0x1B	,0x38	,0x0A,0x02},	//kVAh
//{RAM_EXPKWH		,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k+DISP_W		,DISP_h+DISP_CUM		,DISP_DEC5					,0x1C	,0x38	,0x0A,0x01},	//High ResolutionkWh
//{RAM_EXPKVAH	,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_k				,DISP_V+DISP_A			,DISP_DEC5					,0x1D	,0x38	,0x0A,0x01},	//High resolution kVAh

{RAM_CUMPON		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_3600	,NONE				,DISP_h					,NONE						,0x1E	,0x49	,0x0A,0x02},	// cumulative Power ON hours
{RAM_CUMD_kW  	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_1  	,DISP_k				,DISP_W+DISP_MD  		,DISP_DEC3					,0x1F	,0x4B	,0x0A,0x02},	// Cumulative kW MD
{RAM_RESET_CNT 	,0xFF 	,DIGIT_1  	,NONE  	,CHAR_VAL	,DIV_1  	,NONE				,NONE					,NONE						,0x20	,0x4A	,0x0A,0x02},	// reset count
{RAM_CUMD_kVA  	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_10  	,DISP_k				,DISP_V+DISP_V+DISP_MD  ,DISP_DEC3					,0x21	,0x4B	,0x0A,0x02},	// Cumulative kVA MD
{RAM_KWH		,0xFF	,DIGIT_2	,NONE	,LONG_VAL	,DIV_10		,DISP_k+DISP_W		,DISP_h+DISP_CUM			,DISP_DEC2				,0x22	,0x00	,0x0A,0x02},	//kWh-1 DECIMAL
{RAM_TEMP		,0xFF	,DIGIT_2	,NONE	,SHORT_VAL	,DIV_10		,NONE				,NONE					,DISP_DEC2					,0x23	,0x00	,0x0A,0x02},	//Temperature

// C-OPEN
{MSG_CO   		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x24	,0x58	,0x0A,0x02},
// tamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x25	,0x57	,0x0A,0x08},
// notamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x26	,0x48	,0x0A,0x08},
// backup battery status
{MSG_BAT   	  	,0x00  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x27	,0x53	,0x0A,0x08},
// noClr
{MSG_NO_CLR   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x28	,0x46	,0x02,0x08},


//lcd ALL
{MSG_ALL    	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,0xFF  				,0xFF 					,0xFF                      		,0x29	,0x19	,0x05,0x08},
// Auto
{MSG_AUTO   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2A	,0x04	,0x02,0x08},
// Push
{MSG_PUSH   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2B	,0x05	,0x02,0x08},
// Done
{MSG_DONE   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2C	,0x16	,0x0A,0x08},
// Fail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2D	,0x17	,0x0A,0x08},
// FW Ver
{MSG_FW   		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2E	,0x07	,0x05,0x08},
// ACT-00
{MSG_ACT00 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2F	,0x1A	,0x05,0x08},
// ACT-01
{MSG_ACT01  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x30	,0x1B	,0x05,0x08},
// ON
{MSG_ON  		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x31	,0x1C	,0x05,0x08},
// PULSE
{MSG_PULSE  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x32	,0x2A	,0x05,0x08},
// C-OPEN
{MSG_C_OPEN 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x33	,0x01	,0x05,0x08},

// Mode 1
{MSG_MODE1 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x34	,0x2C	,0x05,0x08},
// Mode 2
{MSG_MODE2 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x35	,0x2D	,0x05,0x08},
// Mode 3
{MSG_MODE3 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x36	,0x2E	,0x05,0x08},
// NM
{MSG_NM 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x37	,0x3F	,0x0A,0x02},
// ND
{MSG_ND 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x38	,0x40	,0x0A,0x02},
// REV
{MSG_REV 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x39	,0x41	,0x0A,0x02},
// MAG
{MSG_MAG 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x3A	,0x42	,0x0A,0x02},
// EAR
{MSG_EAR 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE   							,0x3B	,0x43	,0x0A,0x02},
// nFail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x3C	,0x45	,0x0A,0x08},




// Cover TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  		,DISP_DOTS				,NONE 	   	                ,0x3D	,0x00	,0x05,0x02},	
//Cover Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,DATE_VAL	,DIV_1     	,DISP_DATE  		,DISP_DOTS				,NONE 	   	                ,0x3E	,0x00	,0x05,0x02},
//Serial Number
{EEP_DATA   	,0x00  	,DIGIT_1  	,NONE  	,LONG_VAL 	,DIV_1     	,DISP_SR        	,NONE					,NONE					    ,0x3F	,0x00	,0x01,0x08},



// H1 kW MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_k+DISP_W	,DISP_MD				,DISP_DEC3  				,0x40	,0x09	,0x0A,0x02},	
// H1 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS				,NONE 	   	                ,0x41	,0x09	,0x0A,0x02},	
// H1 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_DOTS				,NONE 	   	                ,0x42	,0x09	,0x0A,0x02},
// H1 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM		,NONE						,0x43	,0x09	,0x0A,0x02},
// H1 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,DISP_PF				,DISP_DEC3                  ,0x44	,0x1E	,0x0A,0x02},

// H2 kW MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_k+DISP_W	,DISP_MD				,DISP_DEC3  				,0x45	,0x0A	,0x0A,0x02},	
// H2 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS				,NONE 	   	                ,0x46	,0x0A	,0x0A,0x02},	
// H2 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_DOTS				,NONE 	   	                ,0x47	,0x0A	,0x0A,0x02},
// H2 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM		,NONE						,0x48	,0x0A	,0x0A,0x02},
// H2 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,DISP_PF				,DISP_DEC3                  ,0x49	,0x1F	,0x0A,0x02},

// H3 kW MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_k+DISP_W	,DISP_MD				,DISP_DEC3  				,0x4A	,0x0B	,0x0A,0x02},	
// H3 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS				,NONE 	   	                ,0x4B	,0x0B	,0x0A,0x02},	
// H3 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_DOTS				,NONE 	   	                ,0x4C	,0x0B	,0x0A,0x02},
// H3 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM		,NONE						,0x4D	,0x0B	,0x0A,0x02},
// H3 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,DISP_PF				,DISP_DEC3                  ,0x4E	,0x20	,0x0A,0x02},

// H4 kW MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_k+DISP_W	,DISP_MD				,DISP_DEC3  				,0x4F	,0x0C	,0x0A,0x02},	
// H4 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS				,NONE 	   	                ,0x50	,0x0C	,0x0A,0x02},	
// H4 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_DOTS				,NONE 	   	                ,0x51	,0x0C	,0x0A,0x02},
// H4 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM		,NONE						,0x52	,0x0C	,0x0A,0x02},
// H4 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,DISP_PF				,DISP_DEC3                  ,0x53	,0x21	,0x0A,0x02},

// H5 kW MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_k+DISP_W	,DISP_MD				,DISP_DEC3  				,0x54	,0x0D	,0x0A,0x02},	
// H5 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS				,NONE 	   	                ,0x55	,0x0D	,0x0A,0x02},	
// H5 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_DOTS				,NONE 	   	                ,0x56	,0x0D	,0x0A,0x02},
// H5 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM		,NONE						,0x57	,0x0D	,0x0A,0x02},
// H5 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,DISP_PF				,DISP_DEC3                  ,0x58	,0x22	,0x0A,0x02},

// H6 kW MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_k+DISP_W	,DISP_MD				,DISP_DEC3  				,0x59	,0x0E	,0x0A,0x02},	
// H6 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS				,NONE 	   	                ,0x5A	,0x0E	,0x0A,0x02},	
// H6 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_DATE  	,DISP_DOTS				,NONE 	   	                ,0x5B	,0x0E	,0x0A,0x02},
// H6 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_k+DISP_W	,DISP_h+DISP_CUM		,NONE						,0x5C	,0x0E	,0x0A,0x02},
// H6 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          	,DISP_PF				,DISP_DEC3                  ,0x5D	,0x23	,0x0A,0x02},


// total tamper count
{RET_DATA   	,0xFE  	,DIGIT_1  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE    				,NONE        				,0x6E	,0x2B	,0x0A,0x02},
// current month avg. pf
{RET_DATA   	,0xFE  	,DIGIT_3  	,0x00  	,CHAR_VAL 	,DIV_1  	,NONE 			,NONE					,DISP_DEC3            		,0x6F	,0x37	,0x0A,0x02},

// tamper last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,DISP_DATE  	,DISP_DOTS 	            ,NONE          				,0x70	,0x55	,0x0A,0x02},
// tamper last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS 	            ,NONE          				,0x71	,0x55	,0x0A,0x02},	

// tamper last restore date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,DISP_DATE  	,DISP_DOTS 	            ,NONE          				,0x72	,0x56	,0x0A,0x02},
// tamper last restore time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  	,DISP_DOTS 	            ,NONE          				,0x73	,0x56	,0x0A,0x02},	

// TAMPER id occ
{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE            		,NONE						,0x74	,0x4D	,0x0A,0x08},
// TAMPER id res
{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE            		,NONE						,0x75	,0x4E	,0x0A,0x08},

// H1 kVA MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x12  	,SHORT_VAL	,DIV_10  	,DISP_k			,DISP_V+DISP_A+DISP_MD	,DISP_DEC3					,0x79	,0x09	,0x0A,0x02},
//H1 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM			,NONE						,0x80	,0x09	,0x0A,0x02},
//H2 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM			,NONE						,0x81	,0x0A	,0x0A,0x02},
//H3 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM			,NONE						,0x82	,0x0B	,0x0A,0x02},
//H4 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM			,NONE						,0x83	,0x0C	,0x0A,0x02},
//H5 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM			,NONE						,0x84	,0x0D	,0x0A,0x02},
//H6 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_k			,DISP_h|DISP_V+DISP_A|DISP_CUM			,NONE						,0x85	,0x0E	,0x0A,0x02},

// rtc battery status
{MSG_BAT   	  	,0xFF  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x90	,0x59	,0x0A,0x08},
// nvm status
{MSG_BAT   	  	,0x00  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x91	,0x5B	,0x0A,0x08},
//{MSG_FORMFACTOR ,0xFF  	,DIGIT_8  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x92	,0x00	,0x0A,0x02},
{MSG_OK 		,0xFF  	,DIGIT_8  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x93	,0x5C	,0x0A,0x08},
// mag last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,DISP_DATE  				,DISP_DOTS 	          ,NONE				,0x94	,0x55	,0x0A,0x02},
// mag last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  				,DISP_DOTS 	          ,NONE				,0x95	,0x55	,0x0A,0x02},	

// co oldest OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,DISP_DATE  				,DISP_DOTS 	          ,NONE				,0x96	,0x55	,0x0A,0x02},
// co oldest OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,DISP_TIME  				,DISP_DOTS 	          ,NONE				,0x97	,0x55	,0x0A,0x02},	

// TOD Register 1 kWh
{TOD_kWH1   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA0	,47	,0x06,0x02},
// TOD Register 2 kWh
{TOD_kWH2   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA1	,48	,0x06,0x02},
// TOD Register 3 kWh
{TOD_kWH3   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA2	,49	,0x06,0x02},
// TOD Register 4 kWh
{TOD_kWH4   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA3	,50	,0x06,0x02},
// TOD Register 5 kWh
{TOD_kWH5   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA4	,51	,0x06,0x02},
// TOD Register 6 kWh
{TOD_kWH6   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA5	,52	,0x06,0x02},
// TOD Register 7 kWh
{TOD_kWH7   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA6	,53	,0x06,0x02},
// TOD Register 8 kWh
{TOD_kWH8   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xA7	,54	,0x06,0x02},

// TOD Register 1 kVAh
{TOD_kVAh1   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xA8	,47	,0x06,0x02},
// TOD Register 2 kVAh
{TOD_kVAh2   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xA9	,48	,0x06,0x02},
// TOD Register 3 kVAh
{TOD_kVAh3   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xAA	,49	,0x06,0x02},
// TOD Register 4 kVAh
{TOD_kVAh4   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xAB	,50	,0x06,0x02},
// TOD Register 5 kVAh
{TOD_kVAh5   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xAC	,51	,0x06,0x02},
// TOD Register 6 kVAh
{TOD_kVAh6   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xAD	,52	,0x06,0x02},
// TOD Register 7 kVAh
{TOD_kVAh7   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xAE	,53	,0x06,0x02},
// TOD Register 8 kVAh
{TOD_kVAh8   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xAF	,54	,0x06,0x02},

// TOD H1 Zone 1 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xB8	,47	,0x06,0x02},
// TOD H1 Zone 2 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xB9	,48	,0x06,0x02},
// TOD H1 Zone 3 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xBA	,49	,0x06,0x02},
// TOD H1 Zone 4 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xBB	,50	,0x06,0x02},
// TOD H1 Zone 5 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xBC	,51	,0x06,0x02},
// TOD H1 Zone 6 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xBD	,52	,0x06,0x02},
// TOD H1 Zone 7 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xBE	,53	,0x06,0x02},
// TOD H1 Zone 8 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_k+DISP_W|DISP_TOD		,DISP_h+DISP_CUM	  ,NONE				,0xBF	,54	,0x06,0x02},



// TOD H1 Zone 1 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC0	,47	,0x06,0x02},
// TOD H1 Zone 2 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC1	,48	,0x06,0x02},
// TOD H1 Zone 3 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC2	,49	,0x06,0x02},
// TOD H1 Zone 4 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC3	,50	,0x06,0x02},
// TOD H1 Zone 5 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC4	,51	,0x06,0x02},
// TOD H1 Zone 6 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC5	,52	,0x06,0x02},
// TOD H1 Zone 7 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC6	,53	,0x06,0x02},
// TOD H1 Zone 8 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_k|DISP_TOD			,DISP_h|DISP_V+DISP_A|DISP_CUM		  ,NONE				,0xC7	,54	,0x06,0x02},

// dnld
{MSG_DNLD   ,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     		,NONE  						,NONE                 ,NONE    			,0xD0	,0x5D	,0x04,0x08},
// ESD
{MSG_ESD 	,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     		,NONE  						,NONE                 ,NONE    			,0xD1	,0x5E	,0x0A,0x02},
// esd last OCC date
{EEP_DATA   ,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     		,DISP_DATE  				,DISP_DOTS 	          ,NONE        		,0xD2	,0x55	,0x0A,0x02},
// esd last OCC time
{EEP_DATA   ,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     		,DISP_TIME  				,DISP_DOTS 	          ,NONE     	  	,0xD3	,0x55	,0x0A,0x02},

//Serial Number 2
{EEP_DATA   ,0x00  	,DIGIT_1  	,NONE  	,LONG_VAL 	,DIV_1     		,DISP_SR        			,NONE				  ,NONE				,0xF8	,0x61	,0x01,0x08},

//wave count
{RAM_GFRQ	,0xFF	,DIGIT_2	,NONE	,SHORT_VAL	,DIV_1			,NONE						,NONE				  ,NONE				,0xF9	,0x09	,0x05,0x02},
//ble count
{RET_DATA   ,0xFE  	,DIGIT_1  	,0x00  	,SHORT_VAL 	,DIV_1  		,NONE 						,NONE    			  ,NONE        		,0xFA	,0x2B	,0x0A,0x02},
// locd
{MSG_LOCK 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     		,NONE  						,NONE 				  ,NONE             ,0xFB	,0x5F	,0x05,0x08},
//unlocd
{MSG_UNLOCK ,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     		,NONE  						,NONE 				  ,NONE             ,0xFC	,0x60	,0x05,0x08},

//esd count
{RAM_GFRQ	,0xFF	,DIGIT_2	,NONE	,SHORT_VAL	,DIV_1			,NONE						,NONE				  ,NONE				,0xFD	,0x0A	,0x05,0x02},
//production status
{RET_DATA   ,0xFE 	,DIGIT_6  	,0x00  	,LONG_VAL	,DIV_1     		,NONE  						,NONE				  ,NONE 	   		,0xFE	,0x00	,0x0A,0x01},



};
#endif

#if (defined(LCD18C) && (LCD18C == 1))
const struct lcdmap disp[] = {
	
{RAM_KWH		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,NONE		,0x00	,0x00	,0x0A,0x02},	//kWh
{RAM_VOLT   	,0xFF 	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_10 	,NONE           			,DISP_V|DISP_DEC1			  					,NONE  		,0x01	,0x00	,0x0A,0x02},	//U
{RAM_PH_I   	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_10     ,DISP_AR					,DISP_AL|DISP_DEC2 								,NONE		,0x02	,0x02	,0x0A,0x02},	// Ph Current
{RAM_NU_I   	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_10     ,DISP_AR					,DISP_AL|DISP_DEC2 								,NONE		,0x03	,0x03	,0x0A,0x02},	// Nu Current
{RAM_EFF_I  	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_10     ,DISP_AR					,DISP_AL|DISP_DEC2 								,NONE		,0x04	,0x00	,0x0A,0x02},	// Eff. Current
{RAM_PH_P   	,0xFF	,DIGIT_4  	,NONE  	,LONG_VAL 	,DIV_1  	,NONE         				,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x05	,0x02	,0x0A,0x02},	// Ph Power W
{RAM_NU_P   	,0xFF	,DIGIT_4  	,NONE  	,LONG_VAL 	,DIV_1  	,NONE         				,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x06	,0x03	,0x0A,0x02},	// Nu Power W
{RAM_EFF_P  	,0xFF	,DIGIT_4  	,NONE  	,LONG_VAL 	,DIV_1  	,NONE         				,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x07	,0x00	,0x0A,0x02},	// Eff. Power W
{RAM_S  		,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         				,DISP_V|DISP_T3|DISP_AL|DISP_DEC2				,NONE		,0x08	,0x00	,0x0A,0x02},	// Eff. Power kVA

{RAM_PF     	,0xFF  	,DIGIT_3  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE          				,DISP_DEC2										,NONE       ,0x09	,0x08	,0x0A,0x02},	// Pf.
{RAM_FREQ   	,0xFF  	,DIGIT_3    ,NONE  	,SHORT_VAL	,DIV_1     	,NONE          				,DISP_DEC2										,NONE       ,0x0A	,0x15	,0x0A,0x02},	//FRQ
{RAM_TIME   	,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS										,DISP_TIME 	,0x0B	,0x00	,0x0A,0x02},	//TIME
{RAM_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,NONE  						,DISP_DOTS										,DISP_DATE 	,0x0C	,0x00	,0x0A,0x02},	//Date
{RAM_MD_kW  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,NONE  		,0x0D	,0x00	,0x0A,0x02},	// Current kW MD 
{RAM_kW_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_TIME	,0x0E	,0x00	,0x0A,0x02},	//Current kW MD TIME
{RAM_kW_DATE   	,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_DATE 	,0x0F	,0x00	,0x0A,0x02},	//Current kW MD Date
{RAM_KWH		,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_1		,DISP_h|DISP_DEC4			,DISP_V|DISP_T3|DISP_AL+DISP_k					,NONE		,0x10	,0x00	,0x0A,0x02},	//High ResolutionkWh

{RAM_VOLT   	,0xFF 	,DIGIT_1  	,NONE  	,SHORT_VAL	,DIV_1 		,NONE           			,NONE											,NONE       ,0x11	,0x00	,0x20,0x02},	//Erase Counter
{RAM_PH_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         				,DISP_V|DISP_T3|DISP_AL|DISP_DEC2|DISP_k		,NONE		,0x12	,0x02	,0x0A,0x02},	// Ph Power kW

{RAM_NU_P   	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         				,DISP_V|DISP_T3|DISP_AL|DISP_DEC2|DISP_k		,NONE		,0x13	,0x03	,0x0A,0x02},	// Nu Power kW
{RAM_EFF_P  	,0xFF	,DIGIT_3  	,NONE  	,LONG_VAL 	,DIV_1000  	,NONE         				,DISP_V|DISP_T3|DISP_AL|DISP_DEC2|DISP_k		,NONE		,0x14	,0x00	,0x0A,0x02},	// Eff. Power kW

{RAM_KVAH		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL							,NONE		,0x15	,0x00	,0x0A,0x02},	//kVAh

{RAM_MD_kVA  	,0xFF 	,DIGIT_4  	,NONE  	,SHORT_VAL	,DIV_10  	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DEC2				,NONE  		,0x16	,0x00	,0x0A,0x02},	// Current kVA MD
{RAM_kVA_DATE   ,0xFF 	,DIGIT_6  	,NONE  	,TIME_VAL	,DIV_1     	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DOTS				,DISP_TIME	,0x17	,0x00	,0x0A,0x02},	//Current kVA MD TIME
{RAM_kVA_DATE   ,0xFF 	,DIGIT_6  	,NONE  	,DATE_VAL	,DIV_1     	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DOTS				,DISP_DATE	,0x18	,0x00	,0x0A,0x02},	//Current kVA MD Date
{RAM_KVAH		,0xFF	,DIGIT_6	,NONE	,LONG_VAL	,DIV_1		,DISP_AR|DISP_h|DISP_DEC4	,DISP_k|DISP_V|DISP_AL							,NONE		,0x19	,0x00	,0x0A,0x02},	//High resolution kVAh

{RAM_EXPKWH		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,NONE		,0x1A	,0x38	,0x0A,0x02},	//kWh
{RAM_EXPKVAH	,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL							,NONE		,0x1B	,0x38	,0x0A,0x02},	//kVAh
{RAM_EXPKWH		,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_h|DISP_DEC4			,DISP_V|DISP_T3|DISP_AL+DISP_k					,NONE		,0x1C	,0x38	,0x0A,0x02},	//High ResolutionkWh
{RAM_EXPKVAH	,0xFF	,DIGIT_5	,NONE	,LONG_VAL	,DIV_1		,DISP_AR|DISP_h|DISP_DEC4	,DISP_k|DISP_V|DISP_AL							,NONE		,0x1D	,0x38	,0x0A,0x02},	//High resolution kVAh

{RAM_CUMPON		,0xFF	,DIGIT_1	,NONE	,LONG_VAL	,DIV_3600	,DISP_h						,NONE											,NONE		,0x1E	,0x49	,0x0A,0x02},	// cumulative Power ON hours
{RAM_CUMD_kW  	,0xFF 	,DIGIT_4  	,NONE  	,LONG_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,NONE  		,0x1F	,0x4B	,0x0A,0x02},	// Cumulative kW MD
{RAM_RESET_CNT 	,0xFF 	,DIGIT_1  	,NONE  	,CHAR_VAL	,DIV_1  	,NONE						,NONE											,NONE		,0x20	,0x4A	,0x0A,0x02},	// reset count
{RAM_CUMD_kVA  	,0xFF 	,DIGIT_3  	,NONE  	,LONG_VAL	,DIV_10  	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DEC2				,NONE  		,0x21	,0x4B	,0x0A,0x02},	// Cumulative kVA MD
{RAM_KWH		,0xFF	,DIGIT_2	,NONE	,LONG_VAL	,DIV_10		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC1		,NONE		,0x22	,0x00	,0x0A,0x02},	//kWh-1 DECIMAL

// C-OPEN
{MSG_CO   		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL	,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x24	,0x58	,0x0A,0x02},
// tamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x25	,0x57	,0x0A,0x08},
// notamper
{MSG_NO_TAPER 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x26	,0x48	,0x0A,0x08},
// backup battery status
{MSG_BAT   	  	,0x00  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x27	,0x53	,0x0A,0x08},
// noClr
{MSG_NO_CLR   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    					,0x28	,0x46	,0x02,0x08},


//lcd ALL
{MSG_ALL    	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,0xFF  				,0xFF 					,0xFF                      		,0x29	,0x19	,0x0A,0x08},
// Auto
{MSG_AUTO   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2A	,0x04	,0x02,0x08},
// Push
{MSG_PUSH   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2B	,0x05	,0x02,0x08},
// Done
{MSG_DONE   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2C	,0x16	,0x0A,0x08},
// Fail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2D	,0x17	,0x0A,0x08},
// FW Ver
{MSG_FW   		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE 				,DISP_DEC3|DISP_DEC1    ,NONE     						,0x2E	,0x07	,0x05,0x08},
// ACT-00
{MSG_ACT00 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                      		,0x2F	,0x1A	,0x05,0x08},
// ACT-01
{MSG_ACT01  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x30	,0x1B	,0x05,0x08},
// ON
{MSG_ON  		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x31	,0x1C	,0x05,0x08},
// PULSE
{MSG_PULSE  	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x32	,0x2A	,0x05,0x08},
// C-OPEN
{MSG_C_OPEN 	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x33	,0x01	,0x05,0x08},

// Mode 1
{MSG_MODE1 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x34	,0x2C	,0x05,0x08},
// Mode 2
{MSG_MODE2 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x35	,0x2D	,0x05,0x08},
// Mode 3
{MSG_MODE3 		,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE 					,NONE                       		,0x36	,0x2E	,0x05,0x08},
// NM
{MSG_NM 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x37	,0x3F	,0x0A,0x02},
// ND
{MSG_ND 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x38	,0x40	,0x0A,0x02},
// REV
{MSG_REV 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x39	,0x41	,0x0A,0x02},
// MAG
{MSG_MAG 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x3A	,0x42	,0x0A,0x02},
// EAR
{MSG_EAR 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE   							,0x3B	,0x43	,0x0A,0x02},
// nFail
{MSG_FAIL   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    							,0x3C	,0x45	,0x0A,0x08},




// Cover TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS				,DISP_TIME 	   	            ,0x3D	,0x00	,0x05,0x02},	
//Cover Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,DATE_VAL	,DIV_1     	,NONE  			,DISP_DOTS				,DISP_DATE 	   	            ,0x3E	,0x00	,0x05,0x02},
//Serial Number
{EEP_DATA   	,0x00  	,DIGIT_1  	,NONE  	,LONG_VAL 	,DIV_1     	,NONE        	,NONE					,NONE					    ,0x3F	,0x00	,0x0A,0x08},



// H1 kW MD
{EEP_DATA  		,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  			,0x40	,0x09	,0x0A,0x02},	
// H1 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME	,0x41	,0x09	,0x0A,0x02},	
// H1 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE	,0x42	,0x09	,0x0A,0x02},
// H1 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,DISP_BP			,0x43	,0x09	,0x0A,0x02},
// H1 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          				,DISP_DEC2										,DISP_BP	        ,0x44	,0x1E	,0x0A,0x02},

// H2 kW MD
{EEP_DATA  		,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  			,0x45	,0x0A	,0x0A,0x02},	
// H2 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME	,0x46	,0x0A	,0x0A,0x02},	
// H2 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE	,0x47	,0x0A	,0x0A,0x02},
// H2 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,DISP_BP			,0x48	,0x0A	,0x0A,0x02},
// H2 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          				,DISP_DEC2										,DISP_BP       		,0x49	,0x1F	,0x0A,0x02},

// H3 kW MD
{EEP_DATA  		,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  			,0x4A	,0x0B	,0x0A,0x02},	
// H3 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME	,0x4B	,0x0B	,0x0A,0x02},	
// H3 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE	,0x4C	,0x0B	,0x0A,0x02},
// H3 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,DISP_BP			,0x4D	,0x0B	,0x0A,0x02},
// H3 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          				,DISP_DEC2										,DISP_BP       		,0x4E	,0x20	,0x0A,0x02},

// H4 kW MD
{EEP_DATA  		,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  			,0x4F	,0x0C	,0x0A,0x02},	
// H4 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME	,0x50	,0x0C	,0x0A,0x02},	
// H4 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE	,0x51	,0x0C	,0x0A,0x02},
// H4 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,DISP_BP			,0x52	,0x0C	,0x0A,0x02},
// H4 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          				,DISP_DEC2										,DISP_BP       		,0x53	,0x21	,0x0A,0x02},

// H5 kW MD
{EEP_DATA  		,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  			,0x54	,0x0D	,0x0A,0x02},	
// H5 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP|DISP_TIME	,0x55	,0x0D	,0x0A,0x02},	
// H5 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP|DISP_DATE	,0x56	,0x0D	,0x0A,0x02},
// H5 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,DISP_BP			,0x57	,0x0D	,0x0A,0x02},
// H5 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          				,DISP_DEC2										,DISP_BP       		,0x58	,0x22	,0x0A,0x02},

// H6 kW MD
{EEP_DATA  		,0x00 	,DIGIT_3  	,0x0C  	,SHORT_VAL	,DIV_10  	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DEC2		,DISP_BP  			,0x59	,0x0E	,0x0A,0x02},	
// H6 kW MD TIME
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,TIME_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_TIME	,0x5A	,0x0E	,0x0A,0x02},	
// H6 kW MD Date
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x0E  	,DATE_VAL	,DIV_1     	,DISP_MD					,DISP_V|DISP_T3|DISP_AL+DISP_k|DISP_DOTS		,DISP_BP|DISP_DATE	,0x5B	,0x0E	,0x0A,0x02},
// H6 kWh
{EEP_DATA		,0x00	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100	,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k					,DISP_BP			,0x5C	,0x0E	,0x0A,0x02},
// H6 Avg.Pf
{EEP_DATA   	,0x00  	,DIGIT_3  	,0x18  	,CHAR_VAL 	,DIV_1  	,NONE          				,DISP_DEC2										,DISP_BP       		,0x5D	,0x23	,0x0A,0x02},


// total tamper count
{RET_DATA   	,0xFE  	,DIGIT_1  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE    				,NONE        				,0x6E	,0x2B	,0x0A,0x02},
// current month avg. pf
{RET_DATA   	,0xFE  	,DIGIT_3  	,0x00  	,CHAR_VAL 	,DIV_1  	,NONE 			,DISP_DEC2					,NONE            		,0x6F	,0x37	,0x0A,0x02},

// tamper last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  			,DISP_DOTS 	            ,DISP_DATE          		,0x70	,0x55	,0x0A,0x02},
// tamper last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS 	            ,DISP_TIME     				,0x71	,0x55	,0x0A,0x02},	

// tamper last restore date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  			,DISP_DOTS 	            ,DISP_DATE          		,0x72	,0x56	,0x0A,0x02},
// tamper last restore time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  			,DISP_DOTS 	            ,DISP_TIME          		,0x73	,0x56	,0x0A,0x02},	

// TAMPER id occ
{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE            		,NONE						,0x74	,0x4D	,0x0A,0x08},
// TAMPER id res
{MSG_T_NAME   	,0xFE  ,DIGIT_8  	,0x00  	,SHORT_VAL 	,DIV_1  	,NONE 			,NONE            		,NONE						,0x75	,0x4E	,0x0A,0x08},

// H1 kVA MD
{EEP_DATA  		,0x00 	,DIGIT_4  	,0x12  	,SHORT_VAL	,DIV_10  	,DISP_MD|DISP_AR			,DISP_V|DISP_AL+DISP_k|DISP_DEC2,DISP_BP,0x79	,0x09	,0x0A,0x02},
//H1 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0x80	,0x09	,0x0A,0x02},
//H2 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0x81	,0x0A	,0x0A,0x02},
//H3 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0x82	,0x0B	,0x0A,0x02},
//H4 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0x83	,0x0C	,0x0A,0x02},
//H5 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0x84	,0x0D	,0x0A,0x02},
//H6 kVAh
{EEP_DATA		,0x00	,DIGIT_1	,0x04	,LONG_VAL	,DIV_100	,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0x85	,0x0E	,0x0A,0x02},

// rtc battery status
{MSG_BAT   	  	,0xFF  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x90	,0x59	,0x0A,0x08},
// nvm status
{MSG_BAT   	  	,0x00  	,DIGIT_8  	,NONE  	,CHAR_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x91	,0x5B	,0x0A,0x08},
{MSG_FORMFACTOR ,0xFF  	,DIGIT_8  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x92	,0x00	,0x0A,0x02},
{MSG_FORMFACTOR ,0xFF  	,DIGIT_8  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  						,NONE                 ,NONE    			,0x93	,0x5C	,0x0A,0x08},
// mag last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	          ,DISP_DATE        ,0x94	,0x55	,0x0A,0x02},
// mag last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	          ,DISP_TIME     	,0x95	,0x55	,0x0A,0x02},	

// co oldest OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  						,DISP_DOTS 	          ,DISP_DATE        ,0x96	,0x55	,0x0A,0x02},
// co oldest OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  						,DISP_DOTS 	          ,DISP_TIME     	,0x97	,0x55	,0x0A,0x02},	


// TOD Register 1 kWh
{TOD_kWH1   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA0	,47	,0x06,0x02},
// TOD Register 2 kWh
{TOD_kWH2   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA1	,48	,0x06,0x02},
// TOD Register 3 kWh
{TOD_kWH3   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA2	,49	,0x06,0x02},
// TOD Register 4 kWh
{TOD_kWH4   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA3	,50	,0x06,0x02},
// TOD Register 5 kWh
{TOD_kWH5   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA4	,51	,0x06,0x02},
// TOD Register 6 kWh
{TOD_kWH6   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA5	,52	,0x06,0x02},
// TOD Register 7 kWh
{TOD_kWH7   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA6	,53	,0x06,0x02},
// TOD Register 8 kWh
{TOD_kWH8   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_h|DISP_TOD			,DISP_V|DISP_T3|DISP_AL+DISP_k,NONE				,0xA7	,54	,0x06,0x02},

// TOD Register 1 kVAh
{TOD_kVAh1   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xA8	,47	,0x06,0x02},
// TOD Register 2 kVAh
{TOD_kVAh2   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xA9	,48	,0x06,0x02},
// TOD Register 3 kVAh
{TOD_kVAh3   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xAA	,49	,0x06,0x02},
// TOD Register 4 kVAh
{TOD_kVAh4   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xAB	,50	,0x06,0x02},
// TOD Register 5 kVAh
{TOD_kVAh5   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xAC	,51	,0x06,0x02},
// TOD Register 6 kVAh
{TOD_kVAh6   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xAD	,52	,0x06,0x02},
// TOD Register 7 kVAh
{TOD_kVAh7   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xAE	,53	,0x06,0x02},
// TOD Register 8 kVAh
{TOD_kVAh8   ,0xFF	,DIGIT_1	,0x00	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,NONE						,0xAF	,54	,0x06,0x02},

// TOD H1 Zone 1 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xB8	,47	,0x06,0x02},
// TOD H1 Zone 2 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xB9	,48	,0x06,0x02},
// TOD H1 Zone 3 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xBA	,49	,0x06,0x02},
// TOD H1 Zone 4 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xBB	,50	,0x06,0x02},
// TOD H1 Zone 5 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xBC	,51	,0x06,0x02},
// TOD H1 Zone 6 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xBD	,52	,0x06,0x02},
// TOD H1 Zone 7 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xBE	,53	,0x06,0x02},
// TOD H1 Zone 8 kWh
{EEP_DATA   ,0x00	,DIGIT_1	,0x0C	,LONG_VAL	,DIV_100		,DISP_h						,DISP_V|DISP_T3|DISP_AL+DISP_k,DISP_BP	,0xBF	,54	,0x06,0x02},



// TOD H1 Zone 1 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC0	,47	,0x06,0x02},
// TOD H1 Zone 2 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC1	,48	,0x06,0x02},
// TOD H1 Zone 3 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC2	,49	,0x06,0x02},
// TOD H1 Zone 4 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC3	,50	,0x06,0x02},
// TOD H1 Zone 5 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC4	,51	,0x06,0x02},
// TOD H1 Zone 6 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC5	,52	,0x06,0x02},
// TOD H1 Zone 7 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC6	,53	,0x06,0x02},
// TOD H1 Zone 8 kVAh
{EEP_DATA   ,0x00	,DIGIT_1	,0x10	,LONG_VAL	,DIV_100		,DISP_AR|DISP_h				,DISP_k|DISP_V|DISP_AL,DISP_BP			,0xC7	,54	,0x06,0x02},

// dnld
{MSG_DNLD   	,0xFF  	,DIGIT_8  	,NONE  	,NONE 		,DIV_1     	,NONE  				,NONE                   ,NONE    			,0xD0	,0x5D	,0x04,0x08},
// ESD
{MSG_MAG 		,0xFF  	,DIGIT_2  	,NONE  	,SHORT_VAL 	,DIV_1     	,NONE  				,NONE                   ,NONE    			,0xD1	,0x5E	,0x0A,0x02},
// esd last OCC date
{EEP_DATA   	,0x00  	,DIGIT_6  	,0x00  	,DATE_VAL 	,DIV_1     	,NONE  				,DISP_DOTS 	            ,DISP_DATE        	,0xD2	,0x55	,0x0A,0x02},
// esd last OCC time
{EEP_DATA   	,0x00 	,DIGIT_6  	,0x00  	,TIME_VAL	,DIV_1     	,NONE  				,DISP_DOTS 	            ,DISP_TIME     	  	,0xD3	,0x55	,0x0A,0x02},	



};
#endif

const uint8_t digit[]={LCD_0,LCD_1,LCD_2,LCD_3,LCD_4,LCD_5,LCD_6,LCD_7,LCD_8,LCD_9};
static uint8_t paramIndex;

/************** Display data at LCD *******************************************/
void Display(uint8_t isCodeType)
{   
	uint8_t i,byteVal;
	const uint8_t *lcddReadptr;
	uint16_t loc;
	uint8_t *lcddWriteptr;
	uint32_t data;
	const struct lcdmap *dispIndex;
	uint8_t lcdbuff[10];

	dispIndex = &disp[paramIndex];	//paramID


	for(i=0;i<10;i++) 
		lcdbuff[i]=0x00;// clear buffer
		
    
	if((dispIndex->eep_addr) == 0xFF)// take data from ram
	{
		data=0;
		lcddWriteptr=(uint8_t*)&data;
		lcddReadptr=dispIndex->Data;
		switch(dispIndex->modifier)
		{
			case LONG_VAL:
			case TIME_VAL:
			case DATE_VAL:
				*lcddWriteptr|= *lcddReadptr;
				lcddReadptr++;
				lcddWriteptr++;
				*lcddWriteptr|= *lcddReadptr;
				lcddReadptr++;
				lcddWriteptr++;
			case SHORT_VAL:
				*lcddWriteptr|= *lcddReadptr;
				lcddReadptr++;
				lcddWriteptr++;
			case CHAR_VAL:
				*lcddWriteptr|= *lcddReadptr;
				break;
		}
		
		if(dispIndex->id==HIGH_RESO_KWH_INDEX)
		{
			data=data*100;
			data=data+100UL*InsSave.ZkWhCounter/METER_CONSTANT;
			
		}
		else if((dispIndex->id==HIGH_RESO_KWH_2DEC_INDEX)||(dispIndex->id==HIGH_RESO_KWH_1DEC_INDEX))
		{
			data=data+InsSave.ZkWhCounter/METER_CONSTANT;
		}
		else if((dispIndex->id>=TOD_KWH_START_INDEX)&&(dispIndex->id<=TOD_KWH_END_INDEX))
		{
			if(dispIndex->id==(TOD_KWH_START_INDEX+currentZoneID))
				data=data+InsSave.ZkWhCounter/METER_CONSTANT;
		}		
		else if(dispIndex->id==HIGH_RESO_KVAH_INDEX)
		{
			data=data*100;
			data=data+100UL*InsSave.ZkVAhCounter/METER_CONSTANT;
			
		}
		else if(dispIndex->id==HIGH_RESO_KVAH_2DEC_INDEX)
		{
			data=data+InsSave.ZkVAhCounter/METER_CONSTANT;
		}
		else if((dispIndex->id>=TOD_KVAH_START_INDEX)&&(dispIndex->id<=TOD_KVAH_END_INDEX))
		{
			if(dispIndex->id==(TOD_KVAH_START_INDEX+currentZoneID))
				data=data+InsSave.ZkVAhCounter/METER_CONSTANT;
		}
		else if(dispIndex->id==CUM_POWER_ON_INDEX)
		{
			data=data+InsSave.PowerOn30;
			
		}
		else if(isCodeType==BATTERY_MODE)
		{
			if(dispIndex->id==0x02)
					data=nmPhCurrent;
			else if(dispIndex->id==0x03)
					data=nmNuCurrent;
			else if(dispIndex->id==0x04)
					data=nmEffectiveI;
		}
		

	}
	else if((dispIndex->eep_addr) == 0xFE)// take data from function return value 
	{
		if(dispIndex->id==0xFA)
			data=BleIDCount();
		else if(dispIndex->id==0x6E)
			data=getTotalTamperCount();
		else if(dispIndex->id==0x6F)
			data=getAvgPf();
//		else if((dispIndex->id >0xB7)&&((dispIndex->id <0xC0)))
//		{
//			data=getTODConsumption(dispIndex->id-0xB8,ENERGY_TYPE_KWH);
//			#if (defined(LCD18D) && (LCD18D == 1))
//				lcdbuff[0] |= DISP_BP;
//			#endif
			
//		}
//		else if((dispIndex->id >0xBF)&&((dispIndex->id <0xC8)))
//		{
//			data=getTODConsumption(dispIndex->id-0xC0,ENERGY_TYPE_KVAH);
//			#if (defined(LCD18D) && (LCD18D == 1))
//				lcdbuff[0] |= DISP_BP;
//			#endif
//		}
		else if((dispIndex->id >0x73)&&((dispIndex->id <0x76)))
		{
			data=getlastEventID(dispIndex->id-0x74);
		}
		#if (defined(IS_PRODUCTION_MODE_ENABLED) && (IS_PRODUCTION_MODE_ENABLED == 1))
		else if(dispIndex->id ==0xFE)
		{
			data=getProdStatusDec();		
		}
		#endif
		
	}
	else // get value from eeprom
	{
		if((dispIndex->id >0x3F)&&((dispIndex->id <0x5E)))
		{
			loc=GetBillLoc((dispIndex->id-0x40)/5);	// as it starts from id 0x40
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}
		if(dispIndex->id ==0x79)
		{
			loc=GetBillLoc(0);// as it starts from id 0x80
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}		
		if((dispIndex->id >0x7F)&&((dispIndex->id <0x86)))
		{
			loc=GetBillLoc((dispIndex->id-0x80));// as it starts from id 0x80
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}
		else if(dispIndex->id ==0x3F)
		{
			getMeterSerial(lcdbuff,SR_TYPE_LCD);
		}
		else if((dispIndex->id >0x3C)&&((dispIndex->id <0x3F)))
		{
			loc=TAMPER_NON_ROLLOVER_LOC;
		}
		if((dispIndex->id >0x97)&&((dispIndex->id <0x9E)))
		{
			loc=GetBillLoc((dispIndex->id-0x98));	// Export kWh as it starts from id 0x98
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif
		}
		else if((dispIndex->id >0x6F)&&((dispIndex->id <0x72)))
			loc=TamperRecord.lastOccRes[0];
		else if((dispIndex->id >0x71)&&((dispIndex->id <0x74)))
			loc=TamperRecord.lastOccRes[1];
		else if(dispIndex->id ==0x27)
		{
			loc=RTC_BAT_STATUS_LOC;
		}
		else if((dispIndex->id >0xB7)&&((dispIndex->id <0xC0)))
		{
			loc=GetTODLatestBillZoneLoc(0,dispIndex->id-0xB8);
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif			
		}
		else if((dispIndex->id >0xBF)&&((dispIndex->id <0xC8)))
		{
			loc=GetTODLatestBillZoneLoc(0,dispIndex->id-0xC0);
			#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[0] |= DISP_BP;
			#endif			
		}	
		else if((dispIndex->id >0x93)&&((dispIndex->id <0x96)))
		{
			loc=TamperRecord.lastEventAddr[2];
		}
		else if((dispIndex->id >0x95)&&((dispIndex->id <0x98)))
			loc=TamperRecord.lastEventAddr[7];
		else if((dispIndex->id >0xD1)&&((dispIndex->id <0xD4)))
			loc=TamperRecord.lastEventAddr[10];
		
		
			
		
	}
	
		
	if(dispIndex->eep_addr ==0x00)// eeprom location
	{   
		loc += (dispIndex->offset & 0xFF);
		read_eeprom((uint8_t *)&data,loc,4);
		if(dispIndex->modifier == CHAR_VAL)
			data = data&0xFF;
		else if(dispIndex->modifier == SHORT_VAL)
			data = data&0xFFFF;
	}
	 
#if (defined(LCD18D) && (LCD18D == 1))
	if(dispIndex->symbol1&DISP_TIME)
	{   
		data = ConvertTimeCounterToTime(data,TIME_VAL,0);
	}
	else if(dispIndex->symbol1&DISP_DATE) 	
	{ 
		data = ConvertTimeCounterToTime(data,DATE_VAL,0);
	}
#endif
#if (defined(LCD18C) && (LCD18C == 1))
	if(dispIndex->symbol3&DISP_TIME)
	{   
		data = ConvertTimeCounterToTime(data,TIME_VAL,0);
		
	}
	else if(dispIndex->symbol3&DISP_DATE) 	
	{ 
		data = ConvertTimeCounterToTime(data,DATE_VAL,0);

	}
#endif

	
	if(dispIndex->divisor==DIV_3600)
		data=data/3600;
	else if(dispIndex->divisor==DIV_32)
		data=data/32;
	else if(dispIndex->divisor==DIV_10)
		data=data/10;
	else if(dispIndex->divisor==DIV_100)
		data=data/100;
	else if(dispIndex->divisor==DIV_1000)
		data=data/1000;
	else if(dispIndex->divisor==DIV_320)
		data=data/320;
  
		
	for(i=0; i<8; i++)
	{  
		byteVal=txtmsg[dispIndex->txtid][7-i];
		if((9==dispIndex->id)&&(0x00==Ins.EffectiveP))
			byteVal=txtmsg[71][7-i];
		else if((0x93==dispIndex->id)&&(get3TamperCount()))
			byteVal=txtmsg[87][7-i];
		else if((0x94==dispIndex->id)||(0x95==dispIndex->id))
		{
			if(TamperRecord.lastEventAddr[2]==0)
				byteVal=txtmsg[76][7-i];
		}
		else if((0x96==dispIndex->id)||(0x97==dispIndex->id))
		{
			if(TamperRecord.lastEventAddr[7]==0)
				byteVal=txtmsg[76][7-i];
		}
		else if((0xD2==dispIndex->id)||(0xD3==dispIndex->id))
		{
			if(TamperRecord.lastEventAddr[10]==0)
				byteVal=txtmsg[76][7-i];
		}			
		else if((dispIndex->id==0x74)||(dispIndex->id==0x75))
		{
			if(i<2)
			{
				if(data%2)
					byteVal=txtmsg[85][7-i];
				else
					byteVal=txtmsg[86][7-i];
			}
			else
			{
				
				if((data==207)||(data==208))	// NM
					byteVal=txtmsg[63][7-i];
				else if((data==201)||(data==202))	// MAG
					byteVal=txtmsg[66][7-i];
				else if((data==51)||(data==52))	// REV
					byteVal=txtmsg[65][7-i];
				else if((data==203)||(data==204))	// ND
					byteVal=txtmsg[64][7-i];
				else if((data==69)||(data==70))	// EARTH
					byteVal=txtmsg[67][7-i];
				else if((data==251))	// C-OPEN
					byteVal=txtmsg[88][7-i];
			}
					
		}
		else if((dispIndex->id==0x70)||(dispIndex->id==0x71))
		{
			if(TamperRecord.lastOccRes[0]==0)
				byteVal=txtmsg[81][7-i];
		}
		else if((dispIndex->id==0x72)||(dispIndex->id==0x73))
		{
			if(TamperRecord.lastOccRes[1]==0)
				byteVal=txtmsg[82][7-i];
		}
		else if(dispIndex->id==0x90)
		{
			if(power_fail_state&RTC_BAT_STATUS_FLAG)
				byteVal=txtmsg[90][7-i];
		}
		else if(dispIndex->id==0x27)
		{
			if((data==0x00)&&(isCodeType!=BATTERY_MODE))
				byteVal=txtmsg[84][7-i];
		}
		#if (defined(LCD18D) && (LCD18D == 1))
			#if (defined(LCD18A) && (LCD18A==1))
				byteVal|=((display_flag &(~DISP_MAG)) & (1<<i)?0x80:0)|(dispIndex->symbol3&(1<<i)?0x80:0);	
			#endif
			#if (defined(LCD18D) && (LCD18A==0))
				byteVal|=((display_flag &(~DISP_MAG)) & (1<<i)?0x01:0)|(dispIndex->symbol3&(1<<i)?0x01:0);	
			#endif
			
		#endif
		
		#if (defined(LCD18C) && (LCD18C == 1))
			byteVal|=((display_flag &(~DISP_MAG)) & (1<<i)?0x80:0)|(dispIndex->symbol3&(1<<i)?0x80:0);	
		#endif
		
		#if (defined(LCD18D) && (LCD18D == 1))
			#if (defined(LCD18A) && (LCD18A==1))
				if(((byteVal&0x7F)==0)&&(i>=dispIndex->startdigit))
			#endif
			#if (defined(LCD18D) && (LCD18A==0))
				if(((byteVal&0xFE)==0)&&(i>=dispIndex->startdigit))
			#endif
			
		#endif
		#if (defined(LCD18C) && (LCD18C == 1))
			if(((byteVal&0x7F)==0)&&(i>=dispIndex->startdigit))
		#endif
		{
			byteVal|=digit[data % 10];

			//--------------------------------------------------
			if(data==0)//remove 0 padding
			{
				if(i>=(dispIndex->digits+dispIndex->startdigit))
				{
					#if (defined(LCD18D) && (LCD18D == 1))
						#if (defined(LCD18A) && (LCD18A==1))
							byteVal &= ~0x7F;
						#endif
						#if (defined(LCD18D) && (LCD18A==0))
							byteVal &= ~0xFE;
						#endif
						
					#endif
					#if (defined(LCD18C) && (LCD18C == 1))
						byteVal &= ~0x7F;
					#endif
				}
			}
			//--------------------------------------------------
			data = data/10;
			
		}
		if(i==7)
		{
			if((dispIndex->id==0x02)&&(mcu_flag&PH_POWER_REV))
				byteVal|=CHR__;
			if((dispIndex->id==0x03)&&(mcu_flag&NU_POWER_REV))
				byteVal|=CHR__;
			if((dispIndex->id==0x14)&&((mcu_flag&NU_POWER_REV)||(mcu_flag&PH_POWER_REV)))	
				byteVal|=CHR__;
			if((dispIndex->id==0x09)&&(Ins.LeadLag==1)&&(Ins.EffectiveP>STARTING_POWER_THRESHOLD_L)&&(Ins.PowerFactor<98))
				byteVal|=CHR__;
		}		

		#if (defined(LCD18D) && (LCD18D == 1))
			if(i<2)
				lcdbuff[8+i]|=byteVal;
			else
				lcdbuff[i]|=byteVal;
		#else
				lcdbuff[2+i]|=byteVal;
		#endif
			
	}   
	
	if(TamperRecord.TamperCount[7]>0)
	{
		lcdbuff[1]|=DISP_SCO;
	}
	// symbol 1
	lcdbuff[1]|=dispIndex->symbol1;
	
	if(display_flag&DISP_MAG)
	{
		#if (defined(LCD18D) && (LCD18D == 1))
			lcdbuff[1]|=dispIndex->symbol1|DISP_SMAG;
		#endif
		#if (defined(LCD18C) && (LCD18C == 1))
			lcdbuff[3]|=0x80;
		#endif
		
	}
	
	
	if((isCodeType==USER_CODE)&&(Ins.Voltage>NM_THRESHOLD_VOLTAGE))
	{
		#if (defined(LCD18D) && (LCD18D == 1))
				lcdbuff[8]|=OK_BIT;
		#endif		
		#if (defined(LCD18C) && (LCD18C == 1))
			lcdbuff[9]|=OK_BIT;
		#endif
		
	}
	else if(isCodeType==BATTERY_MODE)
	{
		#if (defined(LCD18C) && (LCD18C == 1))
			lcdbuff[1]|=VBAT_BIT;
		#endif
		
	}

	// symbol 2

	lcdbuff[0]|=dispIndex->symbol2;

		
	for(i=0;i<10;i++)
	{
		#if (defined(LCD18D) && (LCD18D == 1))
			#if (defined(LCD18A) && (LCD18A==1))
				byteVal=lcdbuff[i];
			#endif
			#if (defined(LCD18A) && (LCD18A==0))
				if(lcdbuff[i]&0x80)
					byteVal=0x01;
				else
					byteVal=0x00;
					
				if(lcdbuff[i]&0x40)
					byteVal|=0x02;

				if(lcdbuff[i]&0x20)
					byteVal|=0x04;
				if(lcdbuff[i]&0x10)
					byteVal|=0x08;
				if(lcdbuff[i]&0x08)
					byteVal|=0x10;
				if(lcdbuff[i]&0x04)
					byteVal|=0x20;
				if(lcdbuff[i]&0x02)
					byteVal|=0x40;
				if(lcdbuff[i]&0x01)
					byteVal|=0x80;
			#endif
				
		#elif (defined(LCD18C) && (LCD18C == 1))
			byteVal=lcdbuff[i];
		#endif
			
		*(LCD_DATA_REG_PTR+pinConfig[i])=byteVal;


	}		
}

/*----------------------------------------------------------------------------*/

void Clear_Lcd(void)
{   
	uint8_t i;

	for(i=0;i<10; i++)
		*(LCD_DATA_REG_PTR+pinConfig[i])=0x00;
}

void PushButton(void)
{
	
	if(ModeID!=DISPLAY_MODE_PUSH)
	{
		ModeID=DISPLAY_MODE_PUSH;
		setParaBuff(DISPLAY_MODE_PUSH);
		setParamIndex(MSG_PUSH_INDEX,1);
		display_lock.lock=0;
	}
	else
	{
		refresh_time=0xFE;
	}
	pushModeTime=DISPLAY_PUSH_MODE2_HOLD_TIME;
	

}
void setParaBuff(uint8_t ioption)
{
	//getPdPointer(ioption);
	ModeID=ioption;
	if(ioption==DISPLAY_MODE_NORMAL)
	{
		if(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)
		{
			ModeID=DISPLAY_MODE_AUTO_COVER;
			//getPdPointer(ModeID);
			//displayParamPtr=idisplayModePtr[DISPLAY_MODE_AUTO_COVER];
		}
	}
	else if(ioption==DISPLAY_MODE_UOUT_NORMAL)
	{
		if(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)
		{
			ModeID=DISPLAY_MODE_UOUT_COVER;
			//displayParamPtr=idisplayModePtr[DISPLAY_MODE_UOUT_COVER];
		}		
	}
	getPdPointer(ModeID);
	displayParamPtr=idisplayModePtr;
	displayParamPtr++;
	setParamIndex(*displayParamPtr,0);
}

void setParamIndex(uint8_t paramID,uint8_t isMsg)
{
	uint8_t para_count=sizeof(disp)/sizeof(struct lcdmap);
	
	while(para_count--)
	{
		if(disp[para_count].id==paramID)
			break;
	}
	
	paramIndex=para_count;
	refresh_time=0;
	if(isMsg)
		mcu_flag|=DISPLAY_CHANGE_FLAG;
}

void CheckParaTimeOut(void)
{
	const struct lcdmap *dispIndex;


	dispIndex = &disp[paramIndex];	//paramID
	
	if(mcu_flag&PUSH_BUTTON_RELEASED)
	{
		PushButton();
		mcu_flag&=~PUSH_BUTTON_RELEASED;
		#if (defined(IS_DISPLAY_PUSH_LOCK) && (IS_DISPLAY_PUSH_LOCK == 1))
			display_lock.push_lock_hold_time=0;
		#endif

	}
#if (defined(IS_DISPLAY_PUSH_LOCK) && (IS_DISPLAY_PUSH_LOCK == 1))

	if(mcu_flag&PUSH_BUTTON_PRESSED_FLAG)
	{
		// only for high resolution parameters should be locked
		if((*displayParamPtr==0x10)||(*displayParamPtr==0x19))
		{
			if(display_lock.push_lock_hold_time++>2)
			{
				if(display_lock.lock==0)
				{
					movebackLock();
					setParamIndex(MSG_LOCK_INDEX,1);
				}
				else
				{
					
					setParamIndex(MSG_UNLOCK_INDEX,1);
				}
				display_lock.lock^=1;
				display_lock.push_lock_hold_time=0;
			}
		}
			
	}
	if((mcu_flag&DISPLAY_CHANGE_FLAG)==0)	
	{
		if(display_lock.lock==1)
		{
			refresh_time=0;
			pushModeTime=DISPLAY_PUSH_MODE2_HOLD_TIME;
		}
			
	}	
	
#endif	
	
	if(++refresh_time>=dispIndex->time)
	{
		setParamIndex(*displayParamPtr,0);
		setNextParaSeq();
		
		mcu_flag&=~DISPLAY_CHANGE_FLAG;
		
	}
	

	if((mcu_flag&DISPLAY_CHANGE_FLAG)==0)	
	{
		siwitch2Auto();

	}
	
}

void siwitch2Auto(void)
{
	if((ModeID==DISPLAY_MODE_PUSH) && (pushModeTime>0))
	{
		pushModeTime--;
		refresh_time=0;
		
	}
	else if(ModeID==DISPLAY_MODE_PUSH)
	{
		ModeID=DISPLAY_MODE_NORMAL;
		setParaBuff(DISPLAY_MODE_NORMAL);
		setParamIndex(MSG_AUTO_INDEX,1);
		display_lock.lock=0;
	}
}
void setNextParaSeq(void)
{
	displayParamPtr++;
	if(displayParamPtr>((uint8_t*)idisplayModePtr+idisplayModePtr[0]))
	{
		displayParamPtr=idisplayModePtr+1;
	}
	
	
	if((*displayParamPtr>=TOD_KWH_START_INDEX)&&(*displayParamPtr<=TOD_KWH_END_INDEX))
		getNextZoneReg(TOD_KWH_START_INDEX,TOD_KWH_END_INDEX);
	else if((*displayParamPtr>=TOD_KVAH_START_INDEX)&&(*displayParamPtr<=TOD_KVAH_END_INDEX))
		getNextZoneReg(TOD_KVAH_START_INDEX,TOD_KVAH_END_INDEX);
	else if((*displayParamPtr>=BILL_TOD_KWH_START_INDEX)&&(*displayParamPtr<=BILL_TOD_KWH_END_INDEX))
		getNextZoneReg(BILL_TOD_KWH_START_INDEX,BILL_TOD_KWH_END_INDEX);
	else if((*displayParamPtr>=BILL_TOD_KVAH_START_INDEX)&&(*displayParamPtr<=BILL_TOD_KVAH_END_INDEX))
		getNextZoneReg(BILL_TOD_KVAH_START_INDEX,BILL_TOD_KVAH_END_INDEX);
		
	if((*displayParamPtr==0x33)&&(TamperRecord.TamperStatusFlag&TAMPER_COVEROPEN)==0)
		displayParamPtr++;
	
		
	
	if(displayParamPtr>((uint8_t*)idisplayModePtr+idisplayModePtr[0]))
	{
		displayParamPtr=idisplayModePtr+1;
	}
	
}


uint8_t getParamTime(void)
{
	refresh_time=0xFE;
	return disp[paramIndex].time; 
}

uint8_t getParaCount(void)
{
	return idisplayModePtr[0];
}

uint8_t getMeterSerial(uint8_t* srBuff,uint8_t srType)
{
	uint8_t digitSR,srLength=0,srNumeric=0;
	uint8_t digitSRbuff[16];
	uint32_t srNumber=0;
		
	read_eeprom(digitSRbuff,SERIAL_NO_LOC,16);
	
	while(srLength<16)
	{
		digitSR=digitSRbuff[srLength];
		if(digitSR==':')
			break;

		if((SR_TYPE_LCD==srType)||(SR_TYPE_NUM==srType))
		{
			if(digitSR>='0'&&digitSR<='9')
			{
				digitSR=digitSR-0x30;
				if(SR_TYPE_LCD==srType)		
				{
					#if (defined(LCD18D) && (LCD18D == 1))
						if(srNumeric>5)
							srBuff[15-srNumeric]=digit[digitSR];
						else
							srBuff[7-srNumeric]=digit[digitSR];
					#endif
					#if (defined(LCD18C) && (LCD18C == 1))
						srBuff[9-srNumeric]=digit[digitSR];
					#endif
					
				}
				else
				{
					srBuff[srNumeric]=digitSR;
					
				}
				srNumeric=(srNumeric+1)%9;
				
			}
		}
		else
		{
			srBuff[srNumeric]=digitSRbuff[srNumeric];
			srNumeric++;
		}
			
		srLength++;
	}
	
	if(SR_TYPE_NUM==srType)
	{
		
		srNumber=srBuff[0];
		srType=1;
		while(srType<srNumeric)
		{
			srNumber=srNumber*10+srBuff[srType];
			srType++;
		}
		Data.long_data=srNumber;
	}
	
	
	
	
	return (srNumeric%16);
	
}

void getPdPointer(uint8_t imode)
{
	if(idisplayMode==0)
		idisplayModePtr=lcdParamIDBuff0[imode];
	else
		idisplayModePtr=lcdParamIDBuff1[imode];
}
void getNextZoneReg(uint8_t startAddr,uint8_t endAddr)
{
	uint8_t maxzones=getUiniqueMaxZones(currentSeasonID);
	if((*displayParamPtr>=startAddr+maxzones)&&(*displayParamPtr<=endAddr))
		displayParamPtr+=(8-maxzones);
}
#if (defined(IS_DISPLAY_PUSH_LOCK) && (IS_DISPLAY_PUSH_LOCK == 1))
void movebackLock(void)
{
	displayParamPtr--;
	#if (defined(IS_DISPLAY_CONFIGURABLE) && (IS_DISPLAY_CONFIGURABLE == 0))
	if(displayParamPtr<((uint8_t*)idisplayModePtr[ModeID]+1))
	{
		displayParamPtr=((uint8_t*)idisplayModePtr[ModeID]+idisplayModePtr[ModeID][0]);
	}
	#endif
	
}
#endif