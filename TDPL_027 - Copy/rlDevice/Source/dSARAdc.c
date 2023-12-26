#include "..\\include\\dIODefine.h"
#include "..\\include\\dUser_includes.h"
#include "..\\include\\dSARAdc.h"
#include "..\\include\\dMisc.h"

#include "..\\..\\rlApplication\\Include\\AppMacros.h"
#include "..\\..\\rlApplication\\Include\\AppVariables.h"
#include "..\\..\\rlApplication\\Include\\AppMisc.h"
#include "..\\..\\rlApplication\\Include\\AppEeprom.h"
#include "..\\..\\rlApplication\\Include\\AppCalibration.h"
#include "..\\..\\rlApplication\\Include\\AppTampers.h"
#include "..\\..\\rlApplication\\Include\\AppConfig.h"

/***********************************************************************************************************************
* Function Name: R_SAR_ADC_Create
* Description  : This function initializes the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
uint16_t buffTMP[5];
//uint16_t buffClock[5];
uint8_t buff_sample_tmp_count=0;
void R_SAR_ADC_Create(void)
{
    volatile uint16_t w_count;
	buffTMP[0]=0;
	buffTMP[1]=0;
	buffTMP[2]=0;
	buffTMP[3]=0;
	buffTMP[4]=0;
	buff_sample_tmp_count=0;
    ADCRES = 1U;    /* reset A/D converter */
    ADCRES = 0U;    /* reset release of A/D converter */
    ADCEN = 1U;     /* enables input clock supply */
	TMPCTL=1;
	TMPCTL|=0x80;
	
    ADM0 = 0x00U;  /* disable AD conversion and clear ADM0 register */
    ADMK = 1U;      /* disable INTAD interrupt */
    ADIF = 0U;      /* clear INTAD interrupt flag */
    /* Set INTAD low priority */
    ADPR1 = 1U;
    ADPR0 = 1U;
    /* The reset status of ADPC is analog input, so it's unnecessary to set. */
    /* Set ANI0 - ANI1 pin,ANI3 */
	ADM2 = _80_AD_POSITIVE_INTERVOLT | _00_AD_AREA_MODE_1 | _00_AD_RESOLUTION_10BIT;
	MCU_Delay(1000);
    
    ADM0 = _00_AD_OPERMODE_SELECT | _18_AD_CONVERSION_CLOCK_8 | _04_AD_TIME_MODE_LOWVOLTAGE_1;
    ADM1 = _00_AD_TRIGGER_SOFTWARE | _20_AD_CONVMODE_ONESELECT;
    //ADM2 = _80_AD_POSITIVE_INTERVOLT | _00_AD_NEGATIVE_VSS | _00_AD_AREA_MODE_1 | _00_AD_RESOLUTION_10BIT;
	
    ADUL = _FF_AD_ADUL_VALUE;
    ADLL = _00_AD_ADLL_VALUE;
    ADS = _9D_AD_INPUT_TEMPERSENSOR;
	

    /* Change the waitting time according to the system */
    for (w_count = 0U; w_count < AD_WAITTIME; w_count++)
    {
        NOP();  
    }
    ADCE=0;
}
/***********************************************************************************************************************
* Function Name: R_ADC_Start
* Description  : This function starts the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAR_ADC_Start(void)
{
	
    ADIF = 0U;      /* clear INTAD interrupt flag */
    ADMK = 1U;      /* enable INTAD interrupt */
    ADCS = 1U;      /* enables conversion operation */
	
}
void StopTemperature(void)
{
	TMPCTL=0;
	ADCRES=1;
	ADCS=0;
	ADCE=0;
	
}
/***********************************************************************************************************************
* Function Name: R_ADC_Stop
* Description  : This function stops the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAR_ADC_Stop(void)
{
	ADCRES=1;
    ADCS = 0U;      /* stops conversion operation */
	NOP();
	NOP();
	ADCEN= 0U;
    ADMK = 1U;      /* disable INTAD interrupt */
    ADIF = 0U;      /* clear INTAD interrupt flag */
	
	
	
	
}
/***********************************************************************************************************************
* Function Name: R_SAR_ADC_Set_OperationOn
* Description  : This function enables comparator operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAR_ADC_Set_OperationOn(void)
{
	ADCRES=0;
    ADCE = 1U;      /* enables A/D voltage comparator operation */
	R_SAR_ADC_Start();
}
/***********************************************************************************************************************
* Function Name: R_SAR_ADC_Set_OperationOff
* Description  : This function stops comparator operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAR_ADC_Set_OperationOff(void)
{
    ADCE = 0U;      /* stops A/D voltage comparator operation */
}
/***********************************************************************************************************************
* Function Name: R_SAR_ADC_Get_Result
* Description  : This function returns the conversion result in the buffer.
* Arguments    : buffer -
*                    the address where to write the conversion result
* Return Value : None
***********************************************************************************************************************/
void R_SAR_ADC_Get_Result(uint16_t * const buffer)
{
    *buffer = (uint16_t) (ADCR >> 6U);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#pragma interrupt r_SAR_ADC_interrupt(vect=INTAD)
static void __near r_SAR_ADC_interrupt(void)
{
	
}


void doSARMeasurement(void)
{
	
	uint8_t itr_nos=3;
	uint16_t iTemp=2000;
	uint32_t u32_current=0;

	while(itr_nos--)
	{
		ADMK=1;
		ADIF=0;
		ADS=_9D_AD_INPUT_TEMPERSENSOR;
			
		R_SAR_ADC_Set_OperationOn();
		while(ADIF==0)
		if(iTemp--==0)
			break;
		
		u32_current=(ADCR >> 6);
		
		
	}
	//R_SAR_ADC_Stop();
	if(u32_current==0)
		u32_current=1;
		
	buffTMP[4]=buffTMP[3];
	buffTMP[3]=buffTMP[2];
	buffTMP[2]=buffTMP[1];
	buffTMP[1]=buffTMP[0];	
	buffTMP[0]=2830UL*691/u32_current;
	
	u32_current=buffTMP[0];
	u32_current+=buffTMP[1];
	u32_current+=buffTMP[2];
	u32_current+=buffTMP[3];
	u32_current+=buffTMP[4];
	
	InsTemp= u32_current/5;
	
	if(InsTemp>=3000)
	{
		InsTemp=3000+(42UL*(InsTemp-3000))/10;
	}
	
	
}

