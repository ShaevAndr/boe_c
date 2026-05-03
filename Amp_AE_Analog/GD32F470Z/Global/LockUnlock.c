/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: LockUnlock.c
  Description:
  Programmer: 
  Version: 0.0
  Created: 2024.06.28
  Last modified: 2024.06.28
=============================================================================*/
//#include <stdint.h>
//------------------------------------------------------------------------------
#include "../Unicorn2/crc32Ethernet.h"
#include "../Unicorn2/drv_UUID.h"
#include "ParamSystem.h"
#include "LockUnlock.h"
//--------------------------------------------------------------------------//
static bool UserParamUnlock = true;
static bool FactoryCalibrUnlock = true;
//--------------------------------------------------------------------------//
#define _SizeStr (16)
static const uint8_t UserStr [_SizeStr] = 
{
	0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 
	0x10, 0x02, 0x30, 0x04, 0x50, 0x06, 0x70, 0x08
};
static const uint8_t UserFactory [_SizeStr] = 
{
	0xAB, 0x48, 0x90, 0xC4, 0x55, 0xA1, 0x5C, 0xF1, 
	0x11, 0x2F, 0x03, 0x43, 0x15, 0x6B, 0xD7, 0xF8
};
static const uint8_t FactorySettingsKey [_SizeStr] = 
{
	0xAB, 0x48, 0x90, 0xC4, 0x55, 0xA1, 0x5C, 0xF1, 
	0x11, 0x2F, 0x03, 0x43, 0x15, 0x6B, 0xD7, 0xF9
};

static const uint8_t *UnlockKeys[luktCount] = {
  UserFactory, UserStr, FactorySettingsKey
};

static TUUID UUID;
//--------------------------------------------------------------------------//
static uint32_t CalcKeyInt (const uint8_t * P)
{
	uint32_t r = 0xffffffff;
	
	UUID = GetUUID ();
	
	r = update_crc32_Ethernet (r, UUID.byte, 16);
	r = update_crc32_Ethernet (r, P, _SizeStr);
	
	return (r ^ 0xffffffff);
}
//--------------------------------------------------------------------------//
/*
//--------------------------------------------------------------------------//
//uint32_t CalcKeyUser (void)
//{
//	return (CalcKey (UserStr));
////	UserParamUnlock = true;
//}
//--------------------------------------------------------------------------//
uint32_t CalcKeyFactory (void)
{
	return (CalcKey (UserFactory));
//	UserParamUnlock = true;
//	FactoryCalibrUnlock = true;
}
//--------------------------------------------------------------------------//
//static bool ChackKeyUser (uint32_t Key)
//{
//	if (Key == CalcKey (UserStr))	return (true);
//	else return (false);
//}
//--------------------------------------------------------------------------//
static bool CheckKeyFactory (uint32_t Key)
{
	if (Key == CalcKey (UserFactory))	return (true);
	else return (false);
}
//--------------------------------------------------------------------------//
//bool GetUserParamUnlock (void)
//{
//	return (UserParamUnlock);
//}
//--------------------------------------------------------------------------//
//int32_t GetUserParamKey (void)
//{
//	return (gParamSystem.UserParamKey);
//}
//--------------------------------------------------------------------------//
//void SetUserParamKey (int32_t key)
//{
//	if (ChackKeyUser (key))	UserParamUnlock = true;
//	else UserParamUnlock = false;
//	gParamSystem.UserParamKey = key;
//	SaveParamSystem ();
//}
//--------------------------------------------------------------------------//
bool GetFactoryCalibrUnlock (void)
{
	return (FactoryCalibrUnlock);
}
//--------------------------------------------------------------------------//
int32_t GetFactoryCalibrKey (void)
{
	if (CheckKeyFactory (gParamSystem.FactoryCalibrKey))
		FactoryCalibrUnlock = true;
	return (gParamSystem.FactoryCalibrKey);
}
//--------------------------------------------------------------------------//
void SetFactoryCalibrKey (int32_t key)
{
	if (CheckKeyFactory (key))
	{
		FactoryCalibrUnlock = true;
//		gParamSystem.UserParamKey = CalcKeyUser ();
	}
	else FactoryCalibrUnlock = false;
	gParamSystem.FactoryCalibrKey = key;
	SaveParamSystem ();
}*/
//--------------------------------------------------------------------------//

static bool KeyUnlock[luktCount] = {false};

static bool CheckKey (LockUnlockKeyType keyType, uint32_t Key)
{
	if (Key == CalcKeyInt (UnlockKeys[keyType]))
    return (true);
	else
    return (false);
}

void loadLockUnlockStatus(void)
{
  for (int keyType = 0; keyType < luktCount; keyType++)
    KeyUnlock[keyType] = CheckKey (keyType, gParamSystem.UnlockKey[keyType]);
}

bool isKeyUnlocked (LockUnlockKeyType keyType)
{
  assert((uint32_t)keyType < luktCount);
  return KeyUnlock[keyType];
}

int32_t GetKey (LockUnlockKeyType keyType)
{
  assert((uint32_t)keyType < luktCount);
	return (gParamSystem.UnlockKey[keyType]);
}

void SetKey (LockUnlockKeyType keyType, int32_t key)
{
  assert((uint32_t)keyType < luktCount);
  if (CheckKey (keyType, key))
		KeyUnlock[keyType] = true;
	else
    KeyUnlock[keyType] = false;
	gParamSystem.UnlockKey[keyType] = key;
	SaveParamSystem ();
}

uint32_t CalcKey (LockUnlockKeyType keyType)
{
  assert((uint32_t)keyType < luktCount);
  return (CalcKeyInt (UnlockKeys[keyType]));
}
//--------------------------------------------------------------------------//
