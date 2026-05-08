/*=============================================================================
2     Project: MVTP
3     Platform: BF533
4     Filename: UARTRoutine.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.16
8     Last modified: 2022.09.16
9============================================================================*/
#include <stdint.h>
//#include <stdlib.h>
#include <stdio.h>
//------------------------------------------------------------------------------
#include "drv_time.h"
#include "drv_RS485x.h"
#include "CommandParserFunction.h"
#include "Unicorn2Routine.h"
//#include "unicorn2_tcp.h"
#include "unicorn_uart_speed.h"
#include "ParamSystem.h"
#include "PacketParser.h"

//--------------------------------------------------------------------------//
Unicorn2RoutineParam_t Unicorn2RoutineParam [_MaxUARTNumber + 1];
uint8_t ResetFlag = 0;
//--------------------------------------------------------------------------//
static uint8_t NeedResetFlag = 0;
static uint8_t NumUARTResetFlag = 0;
static uint8_t NeedSetSpeedFlag [_MaxUARTNumber] = {0, 0, 0};
static uint8_t PacketParser_NeedSetModbusAddress [_MaxUARTNumber] = {0, 0, 0};
static uint8_t UARTNumber = 0;
static TTime TransmitCompTO = 0;
//--------------------------------------------------------------------------//
static uint8_t BuffRS485 [_MaxUARTNumber + 1][_SizeBuffRS485];
//--------------------------------------------------------------------------//
uint32_t GetCurrUARTNumber (void) {return (UARTNumber);}
//--------------------------------------------------------------------------//
void NeedToReset (uint8_t NumUART)
{
	NeedResetFlag = 1;
	NumUARTResetFlag = NumUART;
	TransmitCompTO = SetTime_ms (500);
}
//--------------------------------------------------------------------------//
void SetUARTSpeed (uint8_t NumUART, uint8_t Speed)
{
  // change speed only on RS485, on TCP connections ignoring
  if (NumUART >= _RS485_MODBUS_COUNT)
    return;
  printf("RS485:%d speed %d\n", NumUART, Speed);
  gParamSystem.rs485Modbus[NumUART].UARTSpeed = Speed;
  SaveParamSystem ();
  NeedSetSpeedFlag [NumUART] = 1;
	TransmitCompTO = SetTime_ms (100);
}

void SetDeviceAddress(uint8_t NumUART, uint8_t address)
{
  // change speed only on RS485, on TCP connections ignoring
  if (NumUART >= _RS485_MODBUS_COUNT)
    return;
  printf("RS485:%d address %d\n", NumUART, address);
  gParamSystem.rs485Modbus[NumUART].ModbusAddress = address;
  SaveParamSystem ();
  PacketParser_NeedSetModbusAddress [NumUART] = 1;
}
//--------------------------------------------------------------------------//
static uint32_t GetNumberOfByteRxFIFO (uint8_t N)
{
	int32_t TCPNum;
	switch (N)
	{
		case 0: return ((uint32_t)(RS485_GetNumberOfByteRxFIFO (_RS485_1)));
		case 1: return ((uint32_t)(RS485_GetNumberOfByteRxFIFO (_RS485_2)));
		case 2: return ((uint32_t)(RS485_GetNumberOfByteRxFIFO (_RS485_3)));
		default:
//			TCPNum = N - _RS485_MODBUS_COUNT;
//			if (TCPNum >= 0 && TCPNum < _MAX_UNICORN2_TCP_SESSIONS_COUNT)
//				return (uint32_t)(unicorn2_tcp_recv_bytes_count (TCPNum));
//			else
			  return (0);
	}
}

//--------------------------------------------------------------------------//
static uint32_t GetNumberOfFreeByteTxFIFO (uint8_t N)
{
	int32_t TCPNum;
	switch (N)
	{
		case 0: return ((uint32_t)(RS485_GetNumberOfFreeByteTxFIFO (_RS485_1)));
		case 1: return ((uint32_t)(RS485_GetNumberOfFreeByteTxFIFO (_RS485_2)));
		case 2: return ((uint32_t)(RS485_GetNumberOfFreeByteTxFIFO (_RS485_3)));
		default:
//			TCPNum = N - _RS485_MODBUS_COUNT;
//			if (TCPNum >= 0 && TCPNum < _MAX_UNICORN2_TCP_SESSIONS_COUNT)
//			{
//				uint32_t ret = unicorn2_tcp_free_send_space (TCPNum);
//				if (ret > 1) ret--;
//				return (ret);
//			}
//			else
			  return (0);
	}
}
//--------------------------------------------------------------------------//
static uint32_t GetNumberOfByteTxFIFO (uint8_t N)
{
	int32_t TCPNum;
	switch (N)
	{
		case 0: return ((uint32_t)(RS485_GetNumberOfByteTxFIFO (_RS485_1)));
		case 1: return ((uint32_t)(RS485_GetNumberOfByteTxFIFO (_RS485_2)));
		case 2: return ((uint32_t)(RS485_GetNumberOfByteTxFIFO (_RS485_3)));
		default:
//			TCPNum = N - _RS485_COUNT;
//			if (TCPNum >= 0 && TCPNum < _MAX_TCP_SESSIONS_COUNT)
//				return (uint32_t)(tcp_GetNumberOfByteTxFIFO (TCPNum));
//			else
			  return (0);
	}
}
//--------------------------------------------------------------------------//
static uint8_t PopRxFIFO (uint8_t N)
{
	int32_t TCPNum;
	switch (N)
	{
		case 0: return (RS485_PopRxFIFO (_RS485_1).byte);
		case 1: return (RS485_PopRxFIFO (_RS485_2).byte);
		case 2: return (RS485_PopRxFIFO (_RS485_3).byte);
		default:
//			TCPNum = N - _RS485_MODBUS_COUNT;
//			if (TCPNum >= 0 && TCPNum < _MAX_UNICORN2_TCP_SESSIONS_COUNT)
//				return (uint32_t)(unicorn2_tcp_get_byte (TCPNum));
//			else
			  return (0);
	}
}

static uint32_t PushTxFIFOBuf (uint8_t N, const uint8_t *buf, uint32_t size)
{
	uint32_t TCPNum, ret = size;
	switch (N)
	{
		case 0: RS485_PushTxFIFOBuf (_RS485_1, buf, size); break;
		case 1: RS485_PushTxFIFOBuf (_RS485_2, buf, size); break;
		case 2: RS485_PushTxFIFOBuf (_RS485_3, buf, size); break;
		default:
//			TCPNum = N - _RS485_MODBUS_COUNT;
//			if (TCPNum >= 0 && TCPNum < _MAX_UNICORN2_TCP_SESSIONS_COUNT)
//				ret = unicorn2_tcp_put_buf (TCPNum, buf, size);
//			else ret = 0;
//			break;
			return (0);
	}
	return (ret);
}
//--------------------------------------------------------------------------//
typedef enum {Idle, Recieve, Transmite} TRS485State;

static void UARTSetSpeed (uint8_t N, uint8_t Speed)
{
  switch (N)
  {
    case 0: RS485_SetSpeed (GetUART (_RS485_1), unicorn_uart_speed_to_baudrate(Speed)); break;
    case 1: RS485_SetSpeed (GetUART (_RS485_2), unicorn_uart_speed_to_baudrate(Speed)); break;
    case 2: RS485_SetSpeed (GetUART (_RS485_3), unicorn_uart_speed_to_baudrate(Speed)); break;
    default: return;
  }
}
//--------------------------------------------------------------------------//
// ПП обслуживания UARTa
//--------------------------------------------------------------------------//
void Unicorn2Routine (void)
{
	static uint32_t Init = 0;
  static TRS485State Mode [_MaxUARTNumber + 1];		//режим работы приемопередатчика
  static uint32_t IndexBuff [_MaxUARTNumber + 1];	//позиция приема/передачи в буфере
  static uint32_t SizeBuff [_MaxUARTNumber + 1];	//кол-во байт в буфере
  static TTime TxTO [_MaxUARTNumber + 1];

  uint32_t t;
  uint8_t RxB;
  
  if (Init == 0)
  {
  	Init = 1;
  	for (t = 0; t <= _MaxUARTNumber; t++)
  	{
	  	Mode [t] = Idle;
	  	IndexBuff [t] = 0;
	  	SizeBuff [t] = 0;
	  	Unicorn2RoutineParam [t].TotalRxByte = 0;
	  	Unicorn2RoutineParam [t].TotalTxByte = 0;
		  Unicorn2RoutineParam [t].BeginFlags = 0;
	  	Unicorn2RoutineParam [t].EndFlags = 0;
  	}
  }

	for (UARTNumber = 0; UARTNumber <= _MaxUARTNumber; UARTNumber++)
	{
	  t = GetNumberOfByteRxFIFO (UARTNumber);
	  if (t)		
	  {
	  	if ((Mode [UARTNumber] != Transmite))
	  	{
		    do
		    {
		      RxB = PopRxFIFO (UARTNumber);
		 			Unicorn2RoutineParam [UARTNumber].TotalRxByte++;
		      switch (Mode [UARTNumber])
		      {
		        case Idle:
		          if (RxB == _MODBUS_REQUEST_PACKET_HEADER)
		          {
		          	Unicorn2RoutineParam [UARTNumber].BeginFlags++;
		            Mode [UARTNumber] = Recieve;
		            IndexBuff [UARTNumber] = 0;
		            SizeBuff [UARTNumber] = 1;
		            BuffRS485 [UARTNumber] [IndexBuff [UARTNumber]++] = RxB;
		          }
		          break;
		        case Recieve:
		          if (RxB == _MODBUS_DELIMITER)
		          {
						  	Unicorn2RoutineParam [UARTNumber].EndFlags++;
								//printf ("\n%04X", SizeBuff [UARTNumber]);
		            RecieveCompl (UARTNumber, BuffRS485 [UARTNumber], &(SizeBuff [UARTNumber]));
								//printf ("\n%04X", SizeBuff [UARTNumber]);
		            if (SizeBuff [UARTNumber] > 0)
		            {
		              Mode [UARTNumber] = Transmite;
		              IndexBuff [UARTNumber] = 0;
									TxTO [UARTNumber] = SetTime_ms (0);
		            }
		            else
		              Mode [UARTNumber] = Idle;
		            break;
		          }
		          if (RxB == _MODBUS_REQUEST_PACKET_HEADER)
		          {
		          	Unicorn2RoutineParam [UARTNumber].BeginFlags++;
		            Mode [UARTNumber] = Recieve;
		            IndexBuff [UARTNumber] = 0;
		            SizeBuff [UARTNumber] = 1;
		            BuffRS485 [UARTNumber] [IndexBuff [UARTNumber]++] = RxB;
			          break;
		          }
		          if (SizeBuff [UARTNumber] < _SizeBuffRS485)
		          {
		            BuffRS485 [UARTNumber] [IndexBuff [UARTNumber]++] = RxB;
		            SizeBuff [UARTNumber]++;
		          }
		          break;
		        case Transmite:
		          break;
//		        default:
//		          break;
		      }
		    } while (--t);
	  	}
	  }

	  if ((Mode [UARTNumber] == Transmite) && EndTime (TxTO [UARTNumber]))
	  { // пытаемся передать если необходимо
			t=GetNumberOfFreeByteTxFIFO (UARTNumber);
			if (t>0)t--;
	    t = min_uint32 (t, SizeBuff [UARTNumber]);
	    if (t > 0)
	    {
				//TxTO [UARTNumber] = SetTime_ms (50);
	      t = PushTxFIFOBuf (UARTNumber, &(BuffRS485 [UARTNumber][IndexBuff [UARTNumber]]), t);
	      IndexBuff [UARTNumber] += t;
	      Unicorn2RoutineParam [UARTNumber].TotalTxByte += t;
	      SizeBuff [UARTNumber] -= t;
//				printf ("%04X", t);
	      if (SizeBuff [UARTNumber] == 0)
				{
	        Mode [UARTNumber] = Idle;
//					printf ("Idly");
				}
//	      StartTx (UARTNumber);
	    }
	  }
    if (UARTNumber < _RS485_MODBUS_COUNT)
    {
      if ((NeedSetSpeedFlag [UARTNumber] != 0)
        && (Mode [UARTNumber] == Idle)
        && GetNumberOfByteTxFIFO (UARTNumber) == 0
				&& EndTime (TransmitCompTO))
      {
        NeedSetSpeedFlag [UARTNumber] = 0;
        UARTSetSpeed (UARTNumber, gParamSystem.rs485Modbus[UARTNumber].UARTSpeed);
      }
      if ((PacketParser_NeedSetModbusAddress [UARTNumber] != 0)
        && (Mode [UARTNumber] == Idle)
        && GetNumberOfByteTxFIFO (UARTNumber) == 0)
      {
        PacketParser_NeedSetModbusAddress [UARTNumber] = 0;
        PacketParser_SetDeviceAddress(UARTNumber, gParamSystem.rs485Modbus[UARTNumber].ModbusAddress);
      }
    }
	}
  if ((NeedResetFlag != 0)
    && (Mode [NumUARTResetFlag] == Idle)
    && GetNumberOfByteTxFIFO (NumUARTResetFlag) == 0
		&& EndTime (TransmitCompTO))
  {
    NeedResetFlag = 0;
    ResetFlag = 1;
  }
}
