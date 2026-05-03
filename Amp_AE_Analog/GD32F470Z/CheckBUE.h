/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: ChecBOCv.h
  Description:
  Programmer: 
  Version: 0.0
  Created: 2024.06.28
  Last modified: 2024.06.28
=============================================================================*/
#ifndef __ChecBOCv_H__
#define __ChecBOCv_H__
	//--------------------------------------------------------------------------//
	#include <stdint.h>
//	#include <stdbool.h>
	//--------------------------------------------------------------------------//
//	#include "drv_Mona.h"
//	#include "ParamSaved.h"
	//--------------------------------------------------------------------------//
	typedef union {
		struct {
			uint32_t Reserv0:							7; // 0...6
			uint32_t MonpRxError:					1; // 7
			uint32_t Reserv1:							3; // 8...10
			uint32_t IntVoltError:				1; // 11
			uint32_t Reserv2:							2; // 12...13
			uint32_t OneWireSensorError:	1; // 14
			uint32_t Reserv3:							17; // 15...31
		} st;
		uint32_t ui32;
	} StatusBUE_t;
	//--------------------------------------------------------------------------//
	typedef union {
		struct {
			uint32_t ADC2_p3_3Vd_Min:	1; //0
			uint32_t ADC2_p3_3Vd_AVG:	1; //1
			uint32_t ADC2_p3_3Vd_Max:	1; //2
			uint32_t ADC2_p5Va_Min:		1; //3
			uint32_t ADC2_p5Va_AVG:		1; //4
			uint32_t ADC2_p5Va_Max:		1; //5
			uint32_t ADC2_p7V_Min:		1; //6
			uint32_t ADC2_p7V_AVG:		1; //7
			uint32_t ADC2_p7V_Max:		1; //8
			uint32_t ADC2_p24V_Min:		1; //9
			uint32_t ADC2_p24V_AVG:		1; //10
			uint32_t ADC2_p24V_Max:		1; //11
			uint32_t ADC2_p27V_Min:		1; //12
			uint32_t ADC2_p27V_AVG:		1; //13
			uint32_t ADC2_p27V_Max:		1; //14
			uint32_t ADC2_Vocm_Min:		1; //15
			uint32_t ADC2_Vocm_AVG:		1; //16
			uint32_t ADC2_Vocm_Max:		1; //17
			uint32_t Monp_Vcc_Min:		1; //18
			uint32_t Monp_Vcc_AVG:		1; //19
			uint32_t Monp_Vcc_Max:		1; //20
			uint32_t Monp_3_3Vp_Min:	1; //21
			uint32_t Monp_3_3Vp_AVG:	1; //22
			uint32_t Monp_3_3Vp_Max:	1; //23
			uint32_t Reserv0:					8; //24...31
		} st;
		uint32_t ui32;
	} ErrorInterVolt_t;
	//--------------------------------------------------------------------------//
	ErrorInterVolt_t GetStatusInternalVoltage (void);
	ErrorInterVolt_t GetStatusInternalVoltageNoCLR (void);
	StatusBUE_t GetStatusBUE (void);
	StatusBUE_t GetStatusBUENoCLR (void);

	void RoutineCheckBUE (void);
	//--------------------------------------------------------------------------//
#endif /*__ChecBOCv_H__*/
