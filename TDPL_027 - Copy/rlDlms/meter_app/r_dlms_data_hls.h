/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : r_dlms_data.h
* Version      : 1.00
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 21.02.2013
******************************************************************************/

#ifndef _R_DLMS_DATA_HLS_H
#define _R_DLMS_DATA_HLS_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "..\\r_dlms_typedef.h"				/* DLMS Type Definitions       */

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
Unsigned16 R_OBIS_Aes_Ecbenc(Unsigned8 *in_ptext, Unsigned8 *out_ctext, Unsigned8 *in_key);
//void AES128_ECB_encrypt((Unsigned8 near*)in_ptext, (Unsigned8 near*)in_key,(Unsigned8 near*)out_ctext);

#endif /* _R_DLMS_DATA_HLS_H */
