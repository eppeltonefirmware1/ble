#include "..\\include\\dIODefine.h"
#include "..\\Include\\dUser_includes.h"
#include "..\\include\\dTypedef.h"
#include "..\\include\\dSerial.h"
#include "..\\include\\dMisc.h"
#include "..\\include\\dIOCtrl.h"
#include "..\\include\\dUart.h"
#include "..\\Include\\dWatchDog.h"
#include "..\\Include\\dI2c.h"

//-------- include app specific files-------------------
#include "..\\..\\rlApplication\\Include\\AppMacros.h"
#include "..\\..\\rlApplication\\Include\\AppVariables.h"
#include "..\\..\\rlApplication\\Include\\AppLcd.h"
#include "..\\..\\rlApplication\\Include\\Appeeprom.h"
//------------------------------------------------------

//-------- include dlms specific files----------------
#include "..\\..\\rlDlms\\DLMS_User.h"


void R_UART0_Create(void);
void Uart0_Start(void);
void Uart0_Stop(void);

void R_UART2_Create(void);
void Uart2_Start(void);
void Uart2_Stop(void);
void sendBytes2BLE(const uint8_t* bptr,uint16_t no_ofBytes);
void isSetBleID(void);
uint8_t readBLESerial(void);
uint16_t BleIDCount(void);
uint8_t ble_stable_time=0;

UartBuf dUartBuf;
const uint8_t bleSr[]={0x41,0x54,0x2B ,0x41 ,0x44 ,0x52 ,0x0D};

void init_Uart0(void)	// used for optical communication
{
	SAU0EN = 1U;	/* supply SAU0 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS0 = _0020_SAU_CK01_fCLK_2 | _0002_SAU_CK00_fCLK_2;
	R_UART0_Create();
	Uart0_Start();
	
}
void R_UART0_Create(void)
{
	ST0 |= _0002_SAUm_CH1_STOP_TRG_ON | _0001_SAUm_CH0_STOP_TRG_ON;	/* disable UART0 receive and transmit */
	STMK0 = 1U;	/* disable INTST0 interrupt */
	STIF0 = 0U;	/* clear INTST0 interrupt flag */
	SRMK0 = 1U;	/* disable INTSR0 interrupt */
	SRIF0 = 0U;	/* clear INTSR0 interrupt flag */
	SREMK0 = 1U;	/* disable INTSRE0 interrupt */
	SREIF0 = 0U;	/* clear INTSRE0 interrupt flag */
	
	/* Set INTST0 low priority */
	STPR10 = 1U;
	STPR00 = 1U;
	/* Set INTSR0 low priority */
	SRPR10 = 1U;
	SRPR00 = 1U;
	/* Set INTSRE0 low priority */
	SREPR10 = 1U;
	SREPR00 = 1U;
	
	SMR00 =  _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
	SDR00 = _9A00_SAU0_CH1_BAUDRATE_DIVISOR;
	NFEN0 |= _04_SAU_RXD1_FILTER_ON;
	SIR01 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR01 =  _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR01 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
	SDR01 = _9A00_SAU0_CH1_BAUDRATE_DIVISOR;
	SO0 |= _0001_SAUm_CH0_DATA_OUTPUT_1;
	SOL0 |= _0000_SAUm_CHANNEL2_NORMAL;	/* output level normal */
	SOE0 |= _0001_SAUm_CH0_OUTPUT_ENABLE;	/* enable UART0 output */
	// RxD0
	OPTICL_RX_DIR|=OPTICL_RX_BIT;
	
	
	/* Set TxD0 pin */
	OPTICL_TX|=OPTICL_TX_BIT;
	OPTICL_TX_DIR&=~OPTICL_TX_BIT;
	
	Uart0_Start();
	
}
/***********************************************************************************************************************
* Function Name: R_UART0_Start
* Description  : This function starts the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Uart0_Start(void)
{
	STIF0 = 0U;	/* clear INTST0 interrupt flag */
	STMK0 = 1U;	/* disable INTST0 interrupt */
	SRIF0 = 0U;	/* clear INTSR0 interrupt flag */
	SRMK0 = 0U;	/* enable INTSR0 interrupt */
	SREIF0 = 0U;	/* clear INTSRE0 interrupt flag */
	SREMK0 = 1U;	/* enable INTSRE0 interrupt */
	SO0 |= _0001_SAUm_CH0_DATA_OUTPUT_1;	/* output level normal */
	SOE0 |= _0001_SAUm_CH0_OUTPUT_ENABLE;	/* enable UART0 output */
	SS0 = _0002_SAUm_CH1_START_TRG_ON | _0001_SAUm_CH0_START_TRG_ON;	/* enable UART0 receive and transmit */
	
}
/***********************************************************************************************************************
* Function Name: R_UART0_Stop
* Description  : This function stops the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Uart0_Stop(void)
{
	ST0 |= _0002_SAUm_CH1_STOP_TRG_ON | _0001_SAUm_CH0_STOP_TRG_ON;	/* disable UART2 receive and transmit */
	SOE0 &= ~_0001_SAUm_CH0_OUTPUT_ENABLE;	/* disable UART2 output */
	STMK0 = 1U;	/* disable INTST0 interrupt */
	STIF0 = 0U;	/* clear INTST0 interrupt flag */
	SRMK0 = 1U;	/* disable INTSR0 interrupt */
	SRIF0 = 0U;	/* clear INTSR0 interrupt flag */
	SREMK0 = 1U;	/* disable INTSRE0 interrupt */
	SREIF0 = 0U;	/* clear INTSRE0 interrupt flag */
	SWITCH_OFF_COMM_VCC;
	//OPTICL_TX&=~OPTICL_TX_BIT;
	//OPTICL_TX_DIR&=~OPTICL_TX_BIT;
	
	//OPTICL_RX_DIR&=~OPTICL_RX_BIT;
	//OPTICL_RX&=~OPTICL_RX_BIT;
	
}


#pragma interrupt UART0_Interrupt_Receive(vect=INTSR0,bank=RB2)

static void UART0_Interrupt_Receive(void)
{
	volatile uint8_t rxChr;
	
	rxChr = RXD0;
	
	
	PhysicalReceiveCallback(0, rxChr);
	
	if((mcu_flag & COMM_RECEIVE_ENABLE)>0)
	{  
		if((mcu_flag & COMM_RECEIVE_START)==0)
		{
			dUartBuf.RxCount =0;
			if(rxChr==START_OF_TRANSMISSION)
				mcu_flag |= COMM_RECEIVE_START;
		}

		if(dUartBuf.RxCount >= 32)
		{
			dUartBuf.RxCount=0;
			mcu_flag&= ~COMM_RECEIVE_START;
		}

		RxTxBuffer[dUartBuf.RxCount++]=rxChr;
		RxTxBuffer[dUartBuf.RxCount]='\0';

		if((dUartBuf.RxCount>=8)&&(RxTxBuffer[dUartBuf.RxCount-1]==0x0A)&&(RxTxBuffer[dUartBuf.RxCount-2]==0x0D))
		{
			mcu_flag &= ~(COMM_RECEIVE_ENABLE+COMM_RECEIVE_START);
			mcu_flag|=COMM_RECEIVE_OVER;
			received_uart=0;
		}
	}
	else
		mcu_flag &= ~COMM_RECEIVE_START;	
	
}

#pragma interrupt UART0_Interrupt_Send(vect=INTST0,bank=RB2)

static void UART0_Interrupt_Send(void)
{

	
	TXD0=0xAB;
	
}

void Meter_Response(const uint8_t* bptr,uint16_t noofBytes)
{
    //--------------------------------------------------------------------------
	SRMK0=1;
	while(noofBytes--)
	{
		STIF0=0x00;
        TXD0 =*bptr++;
        
		while(STIF0==0);
		
    }
	SRIF0=0;
	SRMK0=0;
	mcu_flag|=COMM_TRANSMIT_FLAG;
	if(mcu_flag &COMM_RECEIVE_ENABLE)
		setParamIndex(MSG_DNLD_INDEX,1);
	
}	

void init_Uart2(void)	// used rs232
{
	SAU1EN = 1U;	/* supply SAU1 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS1 = _0020_SAU_CK01_fCLK_2 | _0002_SAU_CK00_fCLK_2;
	R_UART2_Create();
	Uart2_Start();
	
}
void R_UART2_Create(void)
{
	
	ST1 |= _0020_SAU_CK01_fCLK_2 | _0002_SAU_CK00_fCLK_2;	/* disable UART0 receive and transmit */
	STMK2 = 1U;	/* disable INTST0 interrupt */
	STIF2 = 0U;	/* clear INTST0 interrupt flag */
	SRMK2 = 1U;	/* disable INTSR0 interrupt */
	SRIF2 = 0U;	/* clear INTSR0 interrupt flag */
	SREMK2 = 1U;	/* disable INTSRE0 interrupt */
	SREIF2 = 0U;	/* clear INTSRE0 interrupt flag */
	
	/* Set INTST0 low priority */
	STPR11 = 1U;
	STPR01 = 1U;
	/* Set INTSR0 low priority */
	SRPR11 = 1U;
	SRPR01 = 1U;
	/* Set INTSRE0 low priority */
	SREPR11 = 1U;
	SREPR01 = 1U;
	
	SMR10 =  _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR10 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
	SDR10 = _9A00_SAU0_CH1_BAUDRATE_DIVISOR;
	NFEN0 |= _04_SAU_RXD1_FILTER_ON;
	SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR11 = _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR11 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
	SDR11 = _9A00_SAU0_CH1_BAUDRATE_DIVISOR;
	SO1 |= _0001_SAUm_CH0_DATA_OUTPUT_1;
	SOL1 |= _0000_SAUm_CHANNEL0_NORMAL;	/* output level normal */
	SOE1 |= _0001_SAUm_CH0_OUTPUT_ENABLE;	/* enable UART2 output */
	PFSEG3=0xFC;
	// RxD2
	RJ11_RX_DIR|=RJ11_RX_BIT;
	
	
	/* Set TxD2 pin */
	RJ11_TX|=RJ11_TX_BIT;
	RJ11_TX_DIR&=~RJ11_TX_BIT;
	
	
	Uart2_Start();
	
}
/***********************************************************************************************************************
* Function Name: R_UART2_Start
* Description  : This function starts the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Uart2_Start(void)
{
	SO1 |= _0001_SAUm_CH0_DATA_OUTPUT_1;	/* output level normal */
	SOE1 |= _0001_SAUm_CH0_OUTPUT_ENABLE;	/* enable UART2 output */
	SS1 = _0002_SAUm_CH1_START_TRG_ON | _0001_SAUm_CH0_START_TRG_ON;	/* enable UART2 receive and transmit */
	STIF2 = 0U;	/* clear INTST0 interrupt flag */
	STMK2 = 1U;	/* disable INTST0 interrupt */
	SRIF2 = 0U;	/* clear INTSR0 interrupt flag */
	SRMK2 = 0U;	/* enable INTSR0 interrupt */
	SREIF2 = 0U;	/* clear INTSRE0 interrupt flag */
	
}
/***********************************************************************************************************************
* Function Name: R_UART2_Stop
* Description  : This function stops the UART0 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Uart2_Stop(void)
{
	
	ST1 |= _0002_SAUm_CH1_STOP_TRG_ON | _0001_SAUm_CH0_STOP_TRG_ON;	/* disable UART2 receive and transmit */
	SOE1 &= ~_0001_SAUm_CH0_OUTPUT_ENABLE;	/* disable UART2 output */
	STMK2 = 1U;	/* disable INTST2 interrupt */
	STIF2 = 0U;	/* clear INTST2 interrupt flag */
	SRMK2 = 1U;	/* disable INTSR2 interrupt */
	SRIF2 = 0U;	/* clear INTSR2 interrupt flag */
	SREMK2 = 1U;	/* disable INTSRE2 interrupt */
	SREIF2 = 0U;	/* clear INTSRE2 interrupt flag */
	SWITCH_OFF_COMM_VCC;
	
	
	RJ11_TX_DIR&=~RJ11_TX_BIT;
	RJ11_TX&=~RJ11_TX_BIT;
	
	RJ11_RX_DIR&=~RJ11_RX_BIT;
	RJ11_RX&=~RJ11_RX_BIT;


	
}


#pragma interrupt UART2_Interrupt_Receive(vect=INTSR2,bank=RB2)

static void UART2_Interrupt_Receive(void)
{
	volatile uint8_t rxChr;
	
	rxChr = RXD2;
	PhysicalReceiveCallback(1, rxChr);
	
//	if((mcu_flag & COMM_RECEIVE_ENABLE)>0)
//	{  
//		if((mcu_flag & COMM_RECEIVE_START)==0)
//		{
//			dUartBuf.RxCount =0;
//			if(rxChr==START_OF_TRANSMISSION)
//				mcu_flag |= COMM_RECEIVE_START;
//		}

//		if(dUartBuf.RxCount >= 32)
//		{
//			dUartBuf.RxCount=0;
//			mcu_flag&= ~COMM_RECEIVE_START;
//		}

//		RxTxBuffer[dUartBuf.RxCount++]=rxChr;
//		RxTxBuffer[dUartBuf.RxCount]='\0';

//		if((dUartBuf.RxCount>=8)&&(RxTxBuffer[dUartBuf.RxCount-1]==0x0A)&&(RxTxBuffer[dUartBuf.RxCount-2]==0x0D))
//		{
//			mcu_flag &= ~(COMM_RECEIVE_ENABLE+COMM_RECEIVE_START);
//			mcu_flag|=COMM_RECEIVE_OVER;
//			received_uart=1;
//		}
//	}
//	else
//		mcu_flag &= ~COMM_RECEIVE_START;	
	
	
}

#pragma interrupt UART2_Interrupt_Send(vect=INTST2,bank=RB2)

static void UART2_Interrupt_Send(void)
{

	
	TXD2=0xAB;
	
}
void Meter2_Response(const uint8_t* bptr,uint16_t noofBytes)
{
	uint8_t buff[]="AT+QBLEGATTSNTFY=0,FFF3,";
	uint8_t buff2[]={0x0D,0x0A};
    //--------------------------------------------------------------------------
	SRMK2 = 1U;	/* disable INTSR2 interrupt */
	sendBytes2BLE(buff,24);
	sendBytes2BLE(bptr,noofBytes);
	sendBytes2BLE(buff2,2);
	mcu_flag|=COMM_TRANSMIT_FLAG;
	SRIF2 = 0U;	
	SRMK2 = 0U;	
	if(mcu_flag &COMM_RECEIVE_ENABLE)
		setParamIndex(MSG_DNLD_INDEX,1);

}

void sendBytes2BLE(const uint8_t* bptr,uint16_t no_ofBytes)
{
	while(no_ofBytes--)
	{

		STIF2=0x00;
        TXD2 =*bptr++;
		while(STIF2==0);
		
    }
	
}

void isSetBleID(void)
{
	uint16_t ble_count;
	if(GET_BLE_STATUS==0)
		++ble_stable_time;
		
	if(ble_stable_time>4)
	{
		if(ble_stable_time==5)
		{
			ble_count=BleIDCount()+1;
			write_eeprom((uint8_t*)&ble_count,BLE_ID_WCOUNT_LOC,2);
		}
		if(readBLESerial()==1)
			ble_stable_time=0;
	}
	
}
uint16_t BleIDCount(void)
{
	uint16_t ble_count=0;
	read_eeprom((uint8_t*)&ble_count,BLE_ID_WCOUNT_LOC,2);
	return ble_count;
}
uint8_t readBLESerial(void)
{
	uint32_t waittime=50000;
	uint8_t Readsrlen=0,srLen,status=0;
	uint8_t mSr[24]="AT+ADR=0";
	uint8_t rxChar;
	SRMK2=1;
	mSr[8]=0x0D;
	sendBytes2BLE(mSr,9);
	WDT_Restart(0xAC);
	MCU_Delay(50000);	
	srLen=getMeterSerial(mSr+7,SR_TYPE_ASCII);
	mSr[srLen+7]=0x0D;
	sendBytes2BLE(mSr,srLen+8);
	SRIF2=0;
	sendBytes2BLE(bleSr,7);
	do
	{
		WDT_Restart(0xAC);
		if(SRIF2==1)
		{
			SRIF2=0;
			rxChar=RXD2;
			RxTxBuffer[Readsrlen]=rxChar;
			
			if(Readsrlen>6)
			if(RxTxBuffer[Readsrlen-0]==mSr[14])
				if(RxTxBuffer[Readsrlen-1]==mSr[13])
					if(RxTxBuffer[Readsrlen-2]==mSr[12])
						if(RxTxBuffer[Readsrlen-3]==mSr[11])
							if(RxTxBuffer[Readsrlen-4]==mSr[10])
								if(RxTxBuffer[Readsrlen-5]==mSr[9])
									if(RxTxBuffer[Readsrlen-6]==mSr[8])
										if(RxTxBuffer[Readsrlen-7]==mSr[7])
										{
											status=1;
											break;
										}
			if(Readsrlen>2)
			if(RxTxBuffer[Readsrlen-0]==0x0A)
				if(RxTxBuffer[Readsrlen-1]==0x0D)
					if(RxTxBuffer[Readsrlen-2]=='K')
						if(RxTxBuffer[Readsrlen-3]=='O')
							break;
			Readsrlen++;
		}
		
	}while(waittime-->0);
	
	SRMK2=0;

	return status;
	
}