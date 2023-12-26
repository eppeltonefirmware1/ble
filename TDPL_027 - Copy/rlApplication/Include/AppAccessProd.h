#ifndef _APP_ACCESS_PROD_H
#define _APP_ACCESS_PROD_H
#include "..\\..\\rlDevice\\Include\\dTypedef.h"


#define PROD_RTC_SET_BIT		0x01
#define PROD_CLEAR_SET_BIT	 	0x02
#define PROD_SHUPF_SET_BIT	    0x04
#define PROD_SHPF_SET_BIT	    0x08
#define PROD_CTUPF_SET_BIT	    0x10
#define PROD_CTPF_SET_BIT	    0x20
#define PROD_RERTC_SET_BIT	    0x40



#define PROD_ACCESS_PARA_MCLR		(PROD_RTC_SET_BIT|PROD_RERTC_SET_BIT)
#define PROD_ACCESS_PARA_SHUPF		(PROD_ACCESS_PARA_MCLR|PROD_CLEAR_SET_BIT)
#define PROD_ACCESS_PARA_SHPF		(PROD_ACCESS_PARA_SHUPF|PROD_SHUPF_SET_BIT)
#define PROD_ACCESS_PARA_CTUPF		(PROD_ACCESS_PARA_SHPF|PROD_SHPF_SET_BIT)
#define PROD_ACCESS_PARA_CTPF		(PROD_ACCESS_PARA_CTUPF|PROD_CTUPF_SET_BIT)
#define PROD_ACCESS_PARA_ALL		(PROD_ACCESS_PARA_CTPF|PROD_CTPF_SET_BIT)



uint8_t getProdStatusBits(void);
void setProdStatus(uint8_t access_prod_para);
uint8_t getACCESSProd(uint8_t access_prod_para);
uint32_t getProdStatusDec(void);
void ClearProdStatusBits(uint8_t access_prod_para);


#endif
