.SECTION DEFAULT_SET,	TEXT	
 DEFAULT_SET .CSEG		TEXT	
.ORG	0x400

$IFNDEF ASM_TATA_POWER_TYPE
ASM_TATA_POWER_TYPE_1		.EQU	1
$ENDIF

$IF (ASM_TATA_POWER_TYPE_1==1)
FLASH_FW_VERSION:				.DB "4.68.027"
$ENDIF
$IF (ASM_TATA_POWER_TYPE_1==0)
FLASH_FW_VERSION:				.DB "4.68.100"
$ENDIF

								.DS	0x08
FLASH_NO_OF_CATEGORY:			.DB 0x10
$IF (ASM_TATA_POWER_TYPE_1==1)
; parameters' catogery defined in following format:1- byte-no of bytes, values
; lower bytes first
;#1
FLASH_PRE_BILL_SINGLE_ACTION:	.DB	0x05,0x00,0x00,0x00,0x00,0xFF
;#2
FLASH_PASSIVE_SEA_ACT_TIME:		.DB	0x05,0x00,0x00,0x00,0x00,0xFF
;#3
FLASH_MD_PERIOD:				.DB	0x02,0x84,0x03	;	900
;#4
FLASH_LS_PERIOD:				.DB 0x02,0x02,0x00	;	1800
;#5
FLASH_PRE_BILL_DAY_TYPE:		.DB 0x02			;	size
								.DB	0x01			;   day of bill
								.DB	0x00			;	month of 0-Each,1-Odd,2-Even
;#6								
FLASH_ACTIVE_CAL_NAME:			.DB 0x36			;	size
								.DB	'A','C','A','L','1',0x00,0x00,0x00		;	Active Calander name
								.DB	'A','S','E','A','S','O','N',0x00		;	Active Season1 name
								.DB	'W','E','E','K','1',0x00,0x00,0x00		;	Active Season1 week name
								.DB	0x01,0x01,0x01,0x01,0x01,0x01,0x01		;	Active Season1 day#
								.DB	'B','S','E','A','S','O','N',0x00		;	Active Season2 name
								.DB	'W','E','E','K','2',0x00,0x00,0x00		;	Active Season2 week name
								.DB	0x01,0x01,0x01,0x01,0x01,0x01,0x01		;	Active Season2 day#
;#7
FLASH_ACTIVE_SEASON1_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ2-00:00
								.DB	0x68,0x01		;	TZ1-06:00
								.DB	0x48,0x03		;	TZ3-14:00
								.DB	0x38,0x04		;	TZ4-18:00
								.DB	0x00,0x00		;	TZ5-0
								.DB	0x00,0x00		;	TZ6-0
								.DB	0x00,0x00		;	TZ7-0
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x04			; 	Total zones-4
								.DB	0x01			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x03			; 	TZ3-zone no
								.DB	0x04			; 	TZ4-zone no
								.DB	0x00			; 	TZ5-zone no
								.DB	0x00			; 	TZ6-zone no
								.DB	0x00			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#8								
FLASH_ACTIVE_SEASON2_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ2-00:00
								.DB	0x68,0x01		;	TZ1-06:00
								.DB	0x48,0x03		;	TZ3-14:00
								.DB	0x38,0x04		;	TZ4-18:00
								.DB	0x00,0x00		;	TZ5-0
								.DB	0x00,0x00		;	TZ6-0
								.DB	0x00,0x00		;	TZ7-0
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x04			; 	Total zones-4
								.DB	0x01			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x03			; 	TZ3-zone no
								.DB	0x04			; 	TZ4-zone no
								.DB	0x00			; 	TZ5-zone no
								.DB	0x00			; 	TZ6-zone no
								.DB	0x00			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#9								
FLASH_ACTIVE_SEASON_TIME:		.DB 0x07			;	size
								.DB	0x01			; 	season1-start day
								.DB	0x04			; 	season1-start month
								.DB	0x01			; 	season2-start day
								.DB	0x0A			; 	season2-start month
								.DB	0x02			;	total seasons
								.DB	0x01			;	seasons seq-1
								.DB	0x02			;	seasons seq-2
;#10								
FLASH_PASSIVE_CAL_NAME:			.DB 0x36			;	size
								.DB	'B','C','A','L','1',0x00,0x00,0x00		;	Passive Calander name
								.DB	'A','S','E','A','S','O','N',0x00		;	Passive Season name
								.DB	'W','E','E','K','1',0x00,0x00,0x00		;	Passive week name
								.DB	0x01,0x01,0x01,0x01,0x01,0x01,0x01		;	Passive Season1 day#
								.DB	'B','S','E','A','S','O','N',0x00		;	Passive Season2 name
								.DB	'W','E','E','K','2',0x00,0x00,0x00		;	Passive Season2 week name
								.DB	0x02,0x02,0x02,0x02,0x02,0x02,0x02		;	Passive Season2 day#
;#11								
FLASH_PASSIVE_SEASON1_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ2-00:00
								.DB	0x68,0x01		;	TZ1-06:00
								.DB	0x48,0x03		;	TZ3-14:00
								.DB	0x38,0x04		;	TZ4-18:00
								.DB	0x00,0x00		;	TZ5-0
								.DB	0x00,0x00		;	TZ6-0
								.DB	0x00,0x00		;	TZ7-0
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x04			; 	Total zones-4
								.DB	0x01			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x03			; 	TZ3-zone no
								.DB	0x04			; 	TZ4-zone no
								.DB	0x00			; 	TZ5-zone no
								.DB	0x00			; 	TZ6-zone no
								.DB	0x00			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#12								
FLASH_PASSIVE_SEASON2_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ2-00:00
								.DB	0x68,0x01		;	TZ1-06:00
								.DB	0x48,0x03		;	TZ3-14:00
								.DB	0x38,0x04		;	TZ4-18:00
								.DB	0x00,0x00		;	TZ5-0
								.DB	0x00,0x00		;	TZ6-0
								.DB	0x00,0x00		;	TZ7-0
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x04			; 	Total zones-4
								.DB	0x01			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x03			; 	TZ3-zone no
								.DB	0x04			; 	TZ4-zone no
								.DB	0x00			; 	TZ5-zone no
								.DB	0x00			; 	TZ6-zone no
								.DB	0x00			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#13								
FLASH_PASSIVE_SEASON_TIME:		.DB 0x07			;	size
								.DB	0x01			; 	season1-start day
								.DB	0x04			; 	season1-start month
								.DB	0x01			; 	season2-start day
								.DB	0x0A			; 	season2-start month
								.DB	0x02			;	total seasons
								.DB	0x01			;	seasons seq-1
								.DB	0x02			;	seasons seq-2
;#14
FLASH_DISPLAY_MODE:				.DB	0x02			;size
								.DB	0x00
								.DB	0x00
;#15
FLASH_DEFAULT_LLS_KEY:			.DB	0x08			;size
								.DB	'A'
								.DB	'B'
								.DB	'C'
								.DB	'D'
								.DB	'E'
								.DB	'F'
								.DB	'G'
								.DB	'H'
;#16
FLASH_DEFAULT_HLS_KEY:			.DB	0x10			;size
								.DB	'R'
								.DB	'E'
								.DB	'N'
								.DB	'E'
								.DB	'S'
								.DB	'A'
								.DB	'S'
								.DB	'_'
								.DB	'P'
								.DB	'6'
								.DB	'w'
								.DB	'R'
								.DB	'J'
								.DB	'2'
								.DB	'1'
								.DB	'F'
$ENDIF	
$IF (ASM_TATA_POWER_TYPE_1==0)
; parameters' catogery defined in following format:1- byte-no of bytes, values
; lower bytes first
;#1
FLASH_PRE_BILL_SINGLE_ACTION:	.DB	0x05,0x00,0x00,0x00,0x00,0xFF
;#2
FLASH_PASSIVE_SEA_ACT_TIME:		.DB	0x05,0x00,0x00,0x00,0x00,0xFF
;#3
FLASH_MD_PERIOD:				.DB	0x02,0x08,0x07	;	1800
;#4
FLASH_LS_PERIOD:				.DB 0x02,0x02,0x00	;	1800
;#5
FLASH_PRE_BILL_DAY_TYPE:		.DB 0x02			;	size
								.DB	0x01			;   day of bill
								.DB	0x00			;	month of 0-Each,1-Odd,2-Even
;#6								
FLASH_ACTIVE_CAL_NAME:			.DB 0x36			;	size
								.DB	'A','C','A','L','1',0x00,0x00,0x00		;	Active Calander name
								.DB	'A','S','E','A','S','O','N',0x00		;	Active Season1 name
								.DB	'W','E','E','K','1',0x00,0x00,0x00		;	Active Season1 week name
								.DB	0x01,0x01,0x01,0x01,0x01,0x01,0x01		;	Active Season1 day#
								.DB	'B','S','E','A','S','O','N',0x00		;	Active Season2 name
								.DB	'W','E','E','K','2',0x00,0x00,0x00		;	Active Season2 week name
								.DB	0x01,0x01,0x01,0x01,0x01,0x01,0x01		;	Active Season2 day#
;#7
FLASH_ACTIVE_SEASON1_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ1-00:00
								.DB	0x3C,0x00		;	TZ2-01:00
								.DB	0xF0,0x00		;	TZ3-04:00
								.DB	0x58,0x02		;	TZ4-10:00
								.DB	0x48,0x03		;	TZ5-14:00
								.DB	0xFC,0x03		;	TZ6-17:00
								.DB	0x28,0x05		;	TZ7-22:00
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x07			; 	Total zones-7
								.DB	0x03			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x01			; 	TZ3-zone no
								.DB	0x02			; 	TZ4-zone no
								.DB	0x03			; 	TZ5-zone no
								.DB	0x02			; 	TZ6-zone no
								.DB	0x03			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#8								
FLASH_ACTIVE_SEASON2_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ1-00:00
								.DB	0x3C,0x00		;	TZ2-01:00
								.DB	0xF0,0x00		;	TZ3-04:00
								.DB	0x58,0x02		;	TZ4-10:00
								.DB	0x48,0x03		;	TZ5-14:00
								.DB	0xFC,0x03		;	TZ6-17:00
								.DB	0x28,0x05		;	TZ7-22:00
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x07			; 	Total zones-7
								.DB	0x03			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x01			; 	TZ3-zone no
								.DB	0x02			; 	TZ4-zone no
								.DB	0x03			; 	TZ5-zone no
								.DB	0x02			; 	TZ6-zone no
								.DB	0x03			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#9								
FLASH_ACTIVE_SEASON_TIME:		.DB 0x07			;	size
								.DB	0x01			; 	season1-start day
								.DB	0x01			; 	season1-start month
								.DB	0x01			; 	season2-start day
								.DB	0x01			; 	season2-start month
								.DB	0x02			;	total seasons
								.DB	0x01			;	seasons seq-1
								.DB	0x01			;	seasons seq-2
;#10								
FLASH_PASSIVE_CAL_NAME:			.DB 0x36			;	size
								.DB	'B','C','A','L','1',0x00,0x00,0x00		;	Passive Calander name
								.DB	'A','S','E','A','S','O','N',0x00		;	Passive Season name
								.DB	'W','E','E','K','1',0x00,0x00,0x00		;	Passive week name
								.DB	0x01,0x01,0x01,0x01,0x01,0x01,0x01		;	Passive Season1 day#
								.DB	'B','S','E','A','S','O','N',0x00		;	Passive Season2 name
								.DB	'W','E','E','K','2',0x00,0x00,0x00		;	Passive Season2 week name
								.DB	0x02,0x02,0x02,0x02,0x02,0x02,0x02		;	Passive Season2 day#
;#11								
FLASH_PASSIVE_SEASON1_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ1-00:00
								.DB	0x3C,0x00		;	TZ2-01:00
								.DB	0xF0,0x00		;	TZ3-04:00
								.DB	0x58,0x02		;	TZ4-10:00
								.DB	0x48,0x03		;	TZ5-14:00
								.DB	0xFC,0x03		;	TZ6-17:00
								.DB	0x28,0x05		;	TZ7-22:00
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x07			; 	Total zones-7
								.DB	0x03			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x01			; 	TZ3-zone no
								.DB	0x02			; 	TZ4-zone no
								.DB	0x03			; 	TZ5-zone no
								.DB	0x02			; 	TZ6-zone no
								.DB	0x03			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#12								
FLASH_PASSIVE_SEASON2_ZONE_TIME:	.DB 0x19			;	size
								.DB	0x00,0x00		;	TZ1-00:00
								.DB	0x3C,0x00		;	TZ2-01:00
								.DB	0xF0,0x00		;	TZ3-04:00
								.DB	0x58,0x02		;	TZ4-10:00
								.DB	0x48,0x03		;	TZ5-14:00
								.DB	0xFC,0x03		;	TZ6-17:00
								.DB	0x28,0x05		;	TZ7-22:00
								.DB	0x00,0x00		;	TZ8-0
								.DB	0x07			; 	Total zones-7
								.DB	0x03			; 	TZ1-zone no
								.DB	0x02			; 	TZ2-zone no
								.DB	0x01			; 	TZ3-zone no
								.DB	0x02			; 	TZ4-zone no
								.DB	0x03			; 	TZ5-zone no
								.DB	0x02			; 	TZ6-zone no
								.DB	0x03			; 	TZ7-zone no
								.DB	0x00			; 	TZ8-zone no
;#13								
FLASH_PASSIVE_SEASON_TIME:		.DB 0x07			;	size
								.DB	0x01			; 	season1-start day
								.DB	0x01			; 	season1-start month
								.DB	0x01			; 	season2-start day
								.DB	0x01			; 	season2-start month
								.DB	0x02			;	total seasons
								.DB	0x01			;	seasons seq-1
								.DB	0x01			;	seasons seq-2
;#14
FLASH_DISPLAY_MODE:				.DB	0x02			;size
								.DB	0x00
								.DB	0x00
;#15
FLASH_DEFAULT_LLS_KEY:			.DB	0x08			;size
								.DB	'A'
								.DB	'B'
								.DB	'C'
								.DB	'D'
								.DB	'E'
								.DB	'F'
								.DB	'G'
								.DB	'H'
;#16
FLASH_DEFAULT_HLS_KEY:			.DB	0x10			;size
								.DB	'R'
								.DB	'E'
								.DB	'N'
								.DB	'E'
								.DB	'S'
								.DB	'A'
								.DB	'S'
								.DB	'_'
								.DB	'P'
								.DB	'6'
								.DB	'w'
								.DB	'R'
								.DB	'J'
								.DB	'2'
								.DB	'1'
								.DB	'F'
$ENDIF						
															