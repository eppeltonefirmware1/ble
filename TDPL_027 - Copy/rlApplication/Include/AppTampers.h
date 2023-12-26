#ifndef _APP_TAMPER_H
#define _APP_TAMPER_H

#include "..\\..\\rlDevice\\Include\\dTypedef.h"


uint8_t Check_Tamper(void);
void Tamperlog(void);
uint16_t getTotalTamperCount(void);
void getEventLog(uint8_t*ptr, uint8_t tampertype,uint8_t eventno);
uint8_t getCurrentLogCount(uint8_t tamperid);
void logSWTamper(uint8_t eventid);
void getPfailLog(uint8_t*ptr,uint8_t eventno);
void getSWLog(uint8_t*ptr,uint8_t eventno);
uint8_t getEventMaxEntries(uint8_t tamperid);
void ClearCopen(void);
uint8_t getlastEventID(uint8_t eventtype);
uint8_t isEarth(uint16_t earthLimit);
uint16_t get3TamperCount(void);
void makeTamperSnapDataNew(uint8_t index,uint8_t eventtype);
uint16_t getIMaxPulseTime(void);
uint8_t isMagAvailable(uint8_t isDiaply);
void setIMaxPhasePowers(void);
void ClearIMaxPhasePowers(void);
void getIMaxElapsedPulseCount(void);

#endif

