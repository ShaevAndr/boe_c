/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: drv_ICP.h
  Description:
  Programmer: 
  Version: 0.0
  Created: 2017.11.23
  Last modified: 2024.12.10
=============================================================================*/
#ifndef __drv_ICP_H__
#define __drv_ICP_H__
	//--------------------------------------------------------------------------//
	#include <stdint.h>
	//--------------------------------------------------------------------------//
	uint8_t GetICPDisablePower (void);
	void SetICPDisablePower (uint8_t DisablePower);

	void Init_OPPower (void);
	void OPPowerRoutine (void);
	uint8_t GetOPPowerState (void);
	//void ICPPowerDisable (uint8_t d);
	//--------------------------------------------------------------------------//
#endif /*__drv_ICP_H__*/