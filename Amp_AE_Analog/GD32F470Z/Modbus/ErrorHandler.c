/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ErrorHandler.c
     Description: Unicorn to Modbus error code conversion
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
//--------------------------------------------------------------------------//
uint8_t ConvertUnicornErrorIntoModbusError(uint8_t UnicornError)
{
	switch (UnicornError)
	{
		case _NoError:              return _NoError;
		case _ErrorUnKnowComand:    return _IllegalFunction;
		case _ErrorUnCorrParam:     return _IllegalDataAddress;
		case _ErrorNoSuchParam:     return _IllegalDataAddress;
		case _ErrorSize:            return _IllegalDataValue;
		case _ErrorUnCorrComand:    return _IllegalDataValue;
		case _ErrorWriteROParam:    return _IllegalDataValue;
		case _ErrorDataNotReady:    return _SlaveDeviceBusy;
		case _ErrorTimeout:         return _SlaveDeviceBusy;
		case _ErrorComandImposWork: return _SlaveDeviceFailure;
		case _ErrorInternalError:   return _SlaveDeviceFailure;
		default:                    return _SlaveDeviceFailure;
	}
}
