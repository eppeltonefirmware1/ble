#include "..\\..\\rlDevice\\Include\\dI2c.h"

#include "..\\include\\AppAccessProd.h"
#include "..\\include\\AppEeprom.h"
#include "..\\include\\AppConfig.h"

uint8_t getProdStatusBits(void)
{
	uint8_t status=0;
	read_eeprom((uint8_t*)&status,PROD_STATUS_LOC,1);
	if(status==0xFF)
		status=0;
		
	
	return status;
}
void setProdStatus(uint8_t access_prod_para)
{
	uint8_t status=(getProdStatusBits()|access_prod_para);
	write_eeprom((uint8_t*)&status,PROD_STATUS_LOC,1);
	
}

uint8_t getACCESSProd(uint8_t access_prod_para)
{
	
	uint8_t accessVal=getProdStatusBits();
	uint8_t access=0x00;
	if((accessVal&access_prod_para)==access_prod_para)
		access=1;
	
	return access;
}
#if (defined(IS_PRODUCTION_MODE_ENABLED) && (IS_PRODUCTION_MODE_ENABLED == 1))
void ClearProdStatusBits(uint8_t access_prod_para)
{
	uint8_t status=(getProdStatusBits()&(~access_prod_para));
	write_eeprom((uint8_t*)&status,PROD_STATUS_LOC,1);
	
}

uint32_t getProdStatusDec(void)
{
	uint8_t status=getProdStatusBits(),length=1;
	uint32_t decStatus=0;
	while(length<8)
	{
		decStatus=decStatus*10;
		if(status&0x01)
			decStatus+=length;
		status>>=1;
		length++;
		
	}
	return decStatus;
}
#endif