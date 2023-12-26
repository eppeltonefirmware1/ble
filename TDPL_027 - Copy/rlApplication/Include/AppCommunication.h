#ifndef APP_COM_H
#define APP_COM_H
#include "..\\..\\rlDevice\\Include\\dTypedef.h"

void Communication(uint8_t isCodeType);
void Write_Command(uint8_t isCodeType);
uint8_t getSerialCrc(void);
void setMfgMonth(uint8_t month,uint8_t year);
void getDisplayMode(void);
#endif