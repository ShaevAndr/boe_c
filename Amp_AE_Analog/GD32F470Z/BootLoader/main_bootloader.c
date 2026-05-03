

//--------------------------------------------------------------------------//
#include "gd32f4xx.h"
#include <stdbool.h>
#include <stdio.h>
//--------------------------------------------------------------------------//
#include "Global.h"
#include "drv_LEDs.h"
#include "drv_EEPROM.h"
#include "drv_SPI.h"
#include "drv_ExWDT.h"
#include "drv_time.h"//Unicorn2
#include "drv_RS485x.h"//Unicorn2
#include "Unicorn2Routine.h"//Unicorn2
#include "PacketParser.h"//Unicorn2
#include "CommandParser.h"
#include "unicorn_uart_speed.h"//Unicorn2
#include "lfs_storage.h"
#include "DeviceUsageStatistics.h"
#include "ParamSystem.h"
#include "dbg_console.h"

#include "main_bootloader.h"
//#include "drv_ExWDT.h"

//--------------------------------------------------------------------------//
void Pre_Init (void)
{
	Disable_ExWDT ();
  Preinit_LEDs ();
//	init_PowerSync ();
}
//--------------------------------------------------------------------------//
static void ResetRoutine (void)
{
  NVIC_SystemReset ();
}
//--------------------------------------------------------------------------//
static bool EnableLoadApp = true;
static TTime TOLoadApp, TOLoadAppLong;
//--------------------------------------------------------------------------//
bool GetEnableLoadApp (void) {return (EnableLoadApp);}
//--------------------------------------------------------------------------//
void SetEnableLoadApp (int Enable)
{
	if (Enable == 0) EnableLoadApp = false;
	else
	{
		EnableLoadApp = true;
		TOLoadApp = TOLoadAppLong = 0ULL;
	}
}
//--------------------------------------------------------------------------//
uint64_t GetMACAddr (void) {return (0ULL);}
//--------------------------------------------------------------------------//
__attribute__( ( naked, noreturn ) ) void BootJump( unsigned long Address )
{
  __asm("LDR SP, [R0]"); //load new stack pointer address
  __asm("LDR PC, [R0, #4]"); //load new program counter address
}
//--------------------------------------------------------------------------//
static void LoadApplication (void)
{
//	return;
	if( CONTROL_nPRIV_Msk & __get_CONTROL ())
  {  /* not in privileged mode */
    EnablePrivilegedMode( ) ;
  }

	NVIC->ICER [0] = NVIC->ICER [1] = NVIC->ICER [2] = NVIC->ICER [3] = 0xFFFFFFFF;
	NVIC->ICER [4] = NVIC->ICER [5] = NVIC->ICER [6] = NVIC->ICER [7] = 0xFFFFFFFF;

	NVIC->ICPR [0] = NVIC->ICPR [1] = NVIC->ICPR [2] = NVIC->ICPR [3] = 0xFFFFFFFF;
	NVIC->ICPR [4] = NVIC->ICPR [5] = NVIC->ICPR [6] = NVIC->ICPR [7] = 0xFFFFFFFF;

	SysTick->CTRL = 0;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

	SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk |
									SCB_SHCSR_BUSFAULTENA_Msk | 
									SCB_SHCSR_MEMFAULTENA_Msk);

	if( CONTROL_SPSEL_Msk & __get_CONTROL ())
	{  /* MSP is not active */
		__set_MSP (__get_PSP ());
		__set_CONTROL (__get_CONTROL () & ~CONTROL_SPSEL_Msk);
	}

	SCB->VTOR = (uint32_t) _StartApplicationAddress;

  BootJump (_StartApplicationAddress);
}
//--------------------------------------------------------------------------//
TMTDDevice SPmtdDev;
//--------------------------------------------------------------------------//
#define _TOLoadAppShort (10000)
#define _TOLoadAppLong (_TOLoadAppShort + 10000)
//--------------------------------------------------------------------------//
int main (void)
{
  
	Preinit_SetLEDState (_PILED_Hear, _PILED_Off);
	Preinit_SetLEDState (_PILED_Act, _PILED_Off);
	Preinit_SetLEDState (_PILED_Error, _PILED_On);
  {
    uint32_t ahb_frequency = 0U;
    /* an interrupt every 10ms */
    ahb_frequency = rcu_clock_freq_get (CK_AHB);
    SysTick_Config (ahb_frequency / 100);
  }
  rcu_periph_clock_enable (RCU_SYSCFG);
  rcu_periph_clock_enable (RCU_PMU);
  
  InitTime ();//Unicorn2
	TOLoadApp = SetTime_ms (_TOLoadAppShort);
	TOLoadAppLong = SetTime_ms (_TOLoadAppLong);

	dbg_console_init ();
//  init_TestPinKT ();
  init_LEDs ();

  initMTD (&SPmtdDev, _SPI4, _SPI_CS_EE1, 5e6, _SPI_MODE_3, _STM_M95256);
  Delay_ms (100);// !!! EEPROM startup BAG !!!

  storage_init (&SPmtdDev);
	
	InitDeviceUsageStatistics (&main_storage);
  LoadParamSystem (&main_storage);
  loadLockUnlockStatus();
  CommandParserInit();
	
  SetCorrTimeInPPM (0);

  PacketParser_Init ();

	SetModeHLED (_HLEDFBlink);
	SetModeErrorLED (_ENoError);
	Enable_ExWDT ();
  while(1)
  {
    static TTime TOModeWork = 0;

		Restart_ExWDT ();
    
		if (GetReadDeviceTypeVersion ()) TOLoadApp = TOLoadAppLong;
		
		if (EnableLoadApp /*&& GetSDRAMTestComplete ()*/ && EndTime (TOLoadApp))
			LoadApplication ();
		
		if (EnableLoadApp) SetModeHLED (_HLEDFBlink);
		else SetModeHLED (_HLEDUpdeteApp);
		
    Unicorn2Routine ();
    RoutineSaveParamSystem ();
    RoutineLEDs ();
    RoutineDeviceUsageStatistics ();
    if (ResetFlag) ResetRoutine ();
  }
}
//--------------------------------------------------------------------------//

