/*=============================================================================
2     Project: 
3     Platform: STM32G743
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
		
		_FPCount
	} FloatParam_t;
	//--------------------------------------------------------------------------//
	typedef enum {
		_IPpUnknown		= -1,

//		ipModbusAddress,
		
		ipEnableLoadApp,
		
		ipMACAddrUpperThreeBytes,
		ipMACAddrLowThreeBytes,
		
		ipTotalOperatingTime,
		ipNumberOfBootloaderLaunches,
		ipNumberOfApplicationLaunches,
		ipNumberErrorWriteDeviceUsStat,
		
		_IPFactorySettingsLock,
		_IPFactorySettingsKey,
    ipDeviceDateOfManufacture,
    ipDeviceSerialNumber,
    
		_IPCount
	} IntParam_t;
	//--------------------------------------------------------------------------//
	typedef enum {
		_TelPUnknown		= -1,

//		tpLatticeUptime,
		tpCoreUptime,
		
		_TelPCount
	} TelemParam_t;
	//--------------------------------------------------------------------------//
	typedef enum {
		_TablePUnknown		= -1,
		_tblpRS485Params,
		
		_TblPCount
	} TableParam_t;
	//--------------------------------------------------------------------------//
#endif //CommandList_H
