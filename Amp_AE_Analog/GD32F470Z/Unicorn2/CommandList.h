/*=============================================================================
2     Project: 
3     Platform:GD32F407
4     Filename: CommandList.h
5     Description:
6     Version: 0.0
7     Created: 2020.12.14
8     Last modified: 2023.06.28
9============================================================================*/
#ifndef CommandList_H
#define CommandList_H
	//--------------------------------------------------------------------------//
	typedef enum {
		_PAM_RO,
		_PAM_WO,
		_PAM_RW
	} ParamAccessMode_t;
	//--------------------------------------------------------------------------//
	typedef enum {
		_FPUnknown		= -1,

		_FP_CALMADCMonP_0,
		_FP_CALAADCMonP_0,
		_FP_CALMADCMonN_0,
		_FP_CALAADCMonN_0,
		_FP_CALMADCMonP_1,
		_FP_CALAADCMonP_1,
		_FP_CALMADCMonN_1,
		_FP_CALAADCMonN_1,
		_FP_CALMADCMonP_2,
		_FP_CALAADCMonP_2,
		_FP_CALMADCMonN_2,
		_FP_CALAADCMonN_2,
		_FP_CALMADCMonP_3,
		_FP_CALAADCMonP_3,
		_FP_CALMADCMonN_3,
		_FP_CALAADCMonN_3,
		_FP_CALMADCMonP_4,
		_FP_CALAADCMonP_4,
		_FP_CALMADCMonN_4,
		_FP_CALAADCMonN_4,
		_FP_CALMADCMonP_5,
		_FP_CALAADCMonP_5,
		_FP_CALMADCMonN_5,
		_FP_CALAADCMonN_5,
		_FP_CALMADCMonP_6,
		_FP_CALAADCMonP_6,
		_FP_CALMADCMonN_6,
		_FP_CALAADCMonN_6,
		_FP_CALMADCMonP_7,
		_FP_CALAADCMonP_7,
		_FP_CALMADCMonN_7,
		_FP_CALAADCMonN_7,

		_FP_CALMADCMon3P3VD,
		_FP_CALAADCMon3P3VD,
		_FP_CALMADCMonN5Va,
		_FP_CALAADCMonN5Va,
		_FP_CALMADCMonP5Va,
		_FP_CALAADCMonP5Va,
		_FP_CALMADCMonP24Va,
		_FP_CALAADCMonP24Va,
		_FP_CALMMSP1Uiin,
		_FP_CALAMSP1Uiin,
		_FP_CALMMSP1Up,
		_FP_CALAMSP1Up,
		_FP_CALMMSP1VCC,
		_FP_CALAMSP1VCC,
		_FP_CALMMSP2Uiin,
		_FP_CALAMSP2Uiin,
		_FP_CALMMSP2Up,
		_FP_CALAMSP2Up,
		_FP_CALMMSP2VCC,
		_FP_CALAMSP2VCC,
		_FP_ICPMinVoltage,

		_FPCount
	} FloatParam_t;
	//--------------------------------------------------------------------------//
	typedef enum {
		_IPpUnknown		= -1,

		_IP_DevAddr,
		_IP_KuOP0,
		_IP_KuOP1,
		_IP_KuOP2,
		_IP_KuOP3,
		_IP_KuOP4,
		_IP_KuOP5,
		_IP_KuOP6,
		_IP_KuOP7,
		_IP_PowerOP,
		_IP_LotwaferID,
		_IP_DXYpos,
		_IP_System,
		_IP_LotwaferIDMSP1,
		_IP_DXYposMSP1,
		_IP_SystemMSP1,
		_IP_BuildMSP1,
		_IP_LotwaferIDMSP2,
		_IP_DXYposMSP2,
		_IP_SystemMSP2,
		_IP_BuildMSP2,
		_IP_HWDevVer,
		_IP_HFXTOFFGCount,

		_IPMonStateDIn,
		_IPMonChangeDIn,

		_IPStatusBUE,
		_IPStatusEnv,
		_IPStatusInterVolt,

		_IP_DS0_Location,
		_IP_DS1_Location,

		ipTotalOperatingTime,
		ipNumberOfBootloaderLaunches,
		ipNumberOfApplicationLaunches,
		ipNumberErrorWriteDeviceUsStat,

		_IPCount
	} IntParam_t;
	//--------------------------------------------------------------------------//
	typedef enum {
		_TelPUnknown		= -1,

		_Tel_ADC_AvgP0,
		_Tel_ADC_AvgN0,
		_Tel_ADC_AvgP1,
		_Tel_ADC_AvgN1,
		_Tel_ADC_AvgP2,
		_Tel_ADC_AvgN2,
		_Tel_ADC_AvgP3,
		_Tel_ADC_AvgN3,
		_Tel_ADC_AvgP4,
		_Tel_ADC_AvgN4,
		_Tel_ADC_AvgP5,
		_Tel_ADC_AvgN5,
		_Tel_ADC_AvgP6,
		_Tel_ADC_AvgN6,
		_Tel_ADC_AvgP7,
		_Tel_ADC_AvgN7,
		_Tel_ADC_Max3P3,
		_Tel_ADC_Min3P3,
		_Tel_ADC_Avg3P3,
		_Tel_ADC_MaxN5Va,
		_Tel_ADC_MinN5Va,
		_Tel_ADC_AvgN5Va,
		_Tel_ADC_MaxP5Va,
		_Tel_ADC_MinP5Va,
		_Tel_ADC_AvgP5Va,
		_Tel_ADC_MaxP24Va,
		_Tel_ADC_MinP24Va,
		_Tel_ADC_AvgP24Va,
		_Tel_Temp_Avg,
		_Tel_Sync,
		_Tel_UpTime,
		_Tel_PG,
		_Tel_Power_OP_Log,
		_Tel_MSP1_UpTime,
		_Tel_MSP1_Max_Cur,
		_Tel_MSP1_Min_Cur,
		_Tel_MSP1_Avg_Cur,
		_Tel_MSP1_Up_Max,
		_Tel_MSP1_Up_Min,
		_Tel_MSP1_Up_AVG,
		_Tel_MSP1_VCC_Max,
		_Tel_MSP1_VCC_Min,
		_Tel_MSP1_VCC_AVG,
		_Tel_MSP1_Pow_Max,
		_Tel_MSP1_Pow_Min,
		_Tel_MSP1_Pow_AVG,
		_Tel_MSP1_Temp_AVG,
		_Tel_MSP2_UpTime,
		_Tel_MSP2_Max_Cur,
		_Tel_MSP2_Min_Cur,
		_Tel_MSP2_Avg_Cur,
		_Tel_MSP2_Up_Max,
		_Tel_MSP2_Up_Min,
		_Tel_MSP2_Up_AVG,
		_Tel_MSP2_VCC_Max,
		_Tel_MSP2_VCC_Min,
		_Tel_MSP2_VCC_AVG,
		_Tel_MSP2_Pow_Max,
		_Tel_MSP2_Pow_Min,
		_Tel_MSP2_Pow_AVG,
		_Tel_MSP2_Temp_AVG,

		_TelMonStateDIn,
		_TelMonChangeDIn,
		_Tel_Mona_FreqDIn1,
		_Tel_Mona_FreqDIn2,

		_TelStatusBUE,
		_TelStatusEnv,
		_TelStatusInterVolt,
		_Tel_Monp_ErrorPacketCount,
		_Tel_Monp_PassPacketCount,

		_Tel_DSCore_Temp,
		_Tel_DSIP_Temp,

		_Tel_DS0_Temp,
		_Tel_DS1_Temp,
			
		_Tel_ADC_Maxp7V,
		_Tel_ADC_Minp7V,
		_Tel_ADC_Avgp7V,
		_Tel_ADC_Maxp27V,
		_Tel_ADC_Minp27V,
		_Tel_ADC_Avgp27V,
		_Tel_ADC_MaxVocm,
		_Tel_ADC_MinVocm,
		_Tel_ADC_AvgVocm,

		_TelPCount
	} TelimParam_t;
	//--------------------------------------------------------------------------//
	typedef enum {
		_TabPUnknown		= -1,

		_TabPCount
	} TabParams;
	//--------------------------------------------------------------------------//
#endif //CommandList_H
