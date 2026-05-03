/*=============================================================================
2	  Project: SmartBox PeriferialUnit
3	  Platform: GD32F470
4	  Filename: drv_DIn.h
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.23
8   Last modified: 2025.04.14
9	=============================================================================*/
#ifndef drv_DIn_H
#define drv_DIn_H
	//------------------------------------------------------------------------//
	#include <stdbool.h>
	#include <stdint.h>
	//------------------------------------------------------------------------//
	typedef enum 
	{
		_DIn1 = 0,
		_DIn2 = 1,
		_DInNum = 2
	} TNumDIn;
	//------------------------------------------------------------------------//
	void InitDIn (void);
	void RoutineDin (void);
	bool GetStateDIn (TNumDIn Num);
	uint32_t GetStateDInsU32 (void);
	//float GetStateDInsf (void);
	bool GetChangeDIn (TNumDIn Num);
	uint32_t GetChangeDInsU32 (void);
	//float GetChangeDInsf (void);
	float GetFrequencyDIn (TNumDIn Num);
	//------------------------------------------------------------------------//
#endif 
