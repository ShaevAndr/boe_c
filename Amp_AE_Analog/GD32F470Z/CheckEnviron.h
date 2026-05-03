/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: ChecEnviron.h
  Description:
  Programmer: 
  Version: 0.0
  Created: 2024.06.28
  Last modified: 2024.06.28
=============================================================================*/
#ifndef __ChecEnviron_H__
#define __ChecEnviron_H__
	//--------------------------------------------------------------------------//
	#include <stdint.h>
	#include <stdbool.h>
	//--------------------------------------------------------------------------//
	typedef union {
		struct {
			uint32_t ErrorICP:		1;	//0
			uint32_t Reserv0:			1;	//1
			uint32_t ErrorPowUin:	1;	//2
			uint32_t Reserv1:			2;	//3...4
			uint32_t ErrorTemp:		1;	//5
			uint32_t Reserv2:			26;	//6...31
		} st;
		uint32_t ui32;
	} Env_Status_t;
	//--------------------------------------------------------------------------//
//	void LockUnlock_ini (void);
	Env_Status_t GetStatusEnvNoCLR (void);
	Env_Status_t GetStatusEnv (void);

	void CheckEnviron_Routine (void);
	//--------------------------------------------------------------------------//
#endif /*__ChecEnviron_H__*/
