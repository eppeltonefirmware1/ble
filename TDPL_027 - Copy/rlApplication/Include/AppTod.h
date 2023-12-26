#ifndef _APP_TOD_H
#define _APP_TOD_H
#include "..\\..\\rlDevice\\Include\\dTypedef.h"

uint16_t GetZoneLoc(uint8_t ZoneNo);
void TodBilling(void);
uint8_t getCurrentZoneNo(uint32_t timeZoneCounter);
uint16_t GetTODBillLoc(unsigned char eventno);
uint8_t getMaxZoneNo(uint8_t seasonNO);
uint16_t GetTODLatestBillLoc(uint8_t eventno);
//uint32_t getTODConsumption(uint8_t zoneNo,uint8_t EnergyType);
uint8_t getCurrentSeason(uint32_t timeZoneCounter);
uint8_t getMaxSeasonNo(void);
//uint8_t isZoneChanged(uint32_t timeZoneCounter);
uint8_t getUiniqueMaxZones(uint8_t seasonNO);
uint16_t GetTODLatestBillZoneLoc(uint8_t eventno,uint8_t zoneno);
#endif