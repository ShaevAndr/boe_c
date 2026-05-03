/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: LockUnlock.h
  Description:
  Programmer: 
  Version: 0.0
  Created: 2024.06.28
  Last modified: 2024.06.28
=============================================================================*/
#ifndef __LockUnlock_H__
#define __LockUnlock_H__
	//--------------------------------------------------------------------------//
	#include <stdint.h>
	#include <stdbool.h>
	//--------------------------------------------------------------------------//

  typedef enum {
    luktFactoryCalib = 0,
    luktUserParam,
    luktFactorySettings,
    
    luktCount
  } LockUnlockKeyType;
  
  void loadLockUnlockStatus(void);
	bool isKeyUnlocked (LockUnlockKeyType keyType);
	int32_t GetKey (LockUnlockKeyType keyType);
	void SetKey (LockUnlockKeyType keyType, int32_t key);
	uint32_t CalcKey (LockUnlockKeyType keyType);

	//--------------------------------------------------------------------------//
#endif /*__LockUnlock_H__*/
