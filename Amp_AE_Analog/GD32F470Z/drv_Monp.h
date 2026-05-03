/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: drv_Monp.h
5     Description:
6     Version: 0.0
7     Created: 2024.04.14
8     Last modified: 2024.04.14
9===========================================================================*/
#ifndef __drv_Monp_H__
#define __drv_Monp_H__
	//-----------------------------------------------------------------------------
	#include <stdint.h>
	//-----------------------------------------------------------------------------
	typedef enum
	{
		_Monp_RefInt_Min = 0,
		_Monp_RefInt_AVG,
		_Monp_RefInt_Max,
		_Monp_Up_Min,
		_Monp_Up_AVG,
		_Monp_Up_Max,
		_Monp_Iin_Min,
		_Monp_Iin_AVG,
		_Monp_Iin_Max,
		_Monp_Pow_Min,
		_Monp_Pow_AVG,
		_Monp_Pow_Max,
		_Monp_Vcc_Min,
		_Monp_Vcc_AVG,
		_Monp_Vcc_Max,
		_Monp_3_3Vp_Min,
		_Monp_3_3Vp_AVG,
		_Monp_3_3Vp_Max,
		_Monp_TempSensor
	} TMonpParamEnum;
	//-----------------------------------------------------------------------------
	float GetMonpParam (TMonpParamEnum Param);
	uint32_t GetMonpBuildTime (void);
	uint32_t GetMonpUpTime (void);
	void MonpInit (void);
	void MonpRoutine (void);
	//-----------------------------------------------------------------------------
	uint32_t MonpPassPacketCount (void);
	uint32_t MonpErrorPacketCount (void);
	//-----------------------------------------------------------------------------

	//--------------------------------------------------------------------------//
#endif /* __drv_Monp_H__ */

