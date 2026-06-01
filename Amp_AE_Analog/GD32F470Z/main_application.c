
//--------------------------------------------------------------------------//
#include "gd32f4xx.h"
#include <stdbool.h>
//--------------------------------------------------------------------------//
#include "main_application.h"
#include "drv_LEDs.h"
#include "drv_TestPinKT.h"
#include "drv_time.h"//Unicorn2
#include "drv_RS485x.h"//Unicorn2
#include "Unicorn2Routine.h"//Unicorn2
#include "PacketParser.h"//Unicorn2
#include "unicorn_uart_speed.h"//Unicorn2
#include "ParamApp.h"
#include "ParamSystem.h"
#include "CommandParser.h"
#include "drv_MonADC0.h"
#include "drv_MonADC1.h"
#include "drv_MonADC2.h"
#include "drv_Monp.h"
#include "drv_DS18B20.h"
#include "drv_EEPROM.h"
#include "drv_SPI.h"
#include "drv_ExWDT.h"
#include "CheckEnviron.h"
#include "CheckBUE.h"
#include "drv_ICP.h"
#include "drv_Din.h"
#include "drv_KuOP.h"
#include "drv_PowerSync.h"
#include "DeviceUsageStatistics.h"
#include "DeviceTemperatureStatistics.h"
#include "ModbusRtuRoutine.h"
//--------------------------------------------------------------------------//
TMTDDevice SPmtdDev;
//--------------------------------------------------------------------------//
void Pre_Init (void)
{
	Disable_ExWDT ();
  Preinit_LEDs ();
	init_PowerSync ();
}
//--------------------------------------------------------------------------//
static void ResetRoutine (void)
{
  NVIC_SystemReset ();
}
//--------------------------------------------------------------------------//
int main (void)
{
  {
    uint32_t ahb_frequency = 0U;
    /* an interrupt every 10ms */
    ahb_frequency = rcu_clock_freq_get (CK_AHB);
    SysTick_Config (ahb_frequency / 100);
  }
  rcu_periph_clock_enable (RCU_SYSCFG);
  rcu_periph_clock_enable (RCU_PMU);
  
  InitTime ();//Unicorn2

  init_TestPinKT ();
  init_LEDs ();

  initMTD (&SPmtdDev, _SPI4, _SPI_CS_EE1, 5e6, _SPI_MODE_3, _STM_M95256);
  Delay_ms (100);// !!! EEPROM startup BAG !!!

  storage_init(&SPmtdDev);
	InitDeviceUsageStatistics (&main_storage);
	InitDeviceTempStatistics (&main_storage);

  LoadParamSystem (&main_storage);
  LoadParamApp (&main_storage);
  loadLockUnlockStatus();
  CommandParserInit();
	
	gParamApp.ProtocolMode = PROTOCOL_MODBUS;

  if (gParamApp.ProtocolMode == PROTOCOL_MODBUS)
  {
    ModbusRtuRoutine_Init();
  }
  else
  {
    PacketParser_Init();
  }
    
  InitMonADC0 ();
  InitMonADC1 ();
  InitMonADC2 ();
  
  MonpInit ();

  DS18B20_ini ();
	
	Init_KuOP ();
  Init_OPPower ();
	InitDIn ();
  
	Enable_ExWDT ();

  while(1)
  {
    static TTime TOModeWork = 0;

		Restart_ExWDT ();
		ToggleKT_PIN102();
    
		if (EndTime (TOModeWork))
		{
			SetModeHLED (_HLEDSBlink);
			TOModeWork = SetTime_ms (100);
			if (0 == GetStatusBUENoCLR ().ui32)
			{
//				SetModeHLED (_HLEDSBlink);
				if (0 == GetStatusEnvNoCLR ().ui32) SetModeErrorLED (_ENoError);
				else SetModeErrorLED (_EEnvError);
			}
			else
			{
//				SetModeHLED (_HLEDFBlink);
				SetModeErrorLED (_EFatalError);
			}
		}
		
    if (gParamApp.ProtocolMode == PROTOCOL_MODBUS)
      ModbusRtuRoutine();
    else
      Unicorn2Routine();
    RoutineMonADC0 ();
    RoutineMonADC1 ();
    RoutineMonADC2 ();
    DS18B20_Routine ();
    RoutineLEDs ();
    MonpRoutine ();
    KuOPRoutine ();
    OPPowerRoutine ();
    RoutineSaveParamSystem ();
    RoutineSaveParamApp ();

    CheckEnviron_Routine ();
		RoutineCheckBUE ();
		RoutineDin ();
		RoutineDeviceUsageStatistics ();
		RoutineDeviceTempStatistics ();
		
    if (ResetFlag) ResetRoutine ();
  }
}
//--------------------------------------------------------------------------//
