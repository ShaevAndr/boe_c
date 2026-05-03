//--------------------------------------------------------------------------//
#ifndef __drv_LEDs_H__
#define __drv_LEDs_H__
	//--------------------------------------------------------------------------//
	#include <stdint.h>
	//--------------------------------------------------------------------------//
	//--------------------------------------------------------------------------//
	typedef enum {_HLEDOff = 0, _HLEDOn, _HLEDABlink, _HLEDFBlink, _HLEDSBlink, _HLEDUpdeteApp} TModeHLED;
	typedef enum {_ENoError = 0, _EEnvError, _EIntError, _EFatalError, _EAllOn} TModeErrorLED;
	typedef enum {_PILED_Hear, _PILED_Act, _PILED_Error} PreinitLED_t;
	typedef enum {_PILED_Off, _PILED_On} PreinitLEDState_t;
	//--------------------------------------------------------------------------//
	void Preinit_LEDs (void);
	void Preinit_SetLEDState (PreinitLED_t LED, PreinitLEDState_t State);

	void init_LEDs (void);
	void RoutineLEDs (void);
	void PingActivitiLED (void);
	void SetModeHLED (TModeHLED ModeLED);
	void SetModeErrorLED (TModeErrorLED ModeLED);
	//--------------------------------------------------------------------------//
#endif /*__drv_LEDs_H__ */
