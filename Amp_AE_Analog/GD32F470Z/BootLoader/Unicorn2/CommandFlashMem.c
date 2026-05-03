/*=============================================================================
2     Project: Amp_AE
3     Platform: BF533
4     Filename: CommandFlashMem.c
5     Description:
6     Version: 0.0
7     Created: 2022.11.19
8     Last modified: 2022.11.19
9============================================================================*/
#include <stdint.h>
//--------------------------------------------------------------------------//
#include "AccessFlash.h"
#include "PacketParser.h"
#include "CommandParser.h"
#include "CommandParserFunction.h"
#include "CommandFlashMem.h"
//--------------------------------------------------------------------------//
uint8_t FlashParam (uint8_t NumUART, uint8_t Command,
										uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
  int32_t flashNumMax = GetFlashCount ();
	int32_t pageNum, pageSize;
	int32_t flashNum;
  int32_t res, j;

	switch (Command)
	{	
	  case _WriteFlashMemPage:
	  {
	    if (*pSize < 2 + 1 + 4 + 4 + 1)
	    	{ErrorNum = _ErrorSize; break;}
			flashNum = B [0];

	    pageSize = GetFlashPageSize (flashNum);
	    if (*pSize != 2 + pageSize + 1 + 4 + 4 + 1)
	    	{ErrorNum = _ErrorSize; break;}

	    if ((flashNum < 0) || (flashNum >= GetFlashCount ())) 
	    	{ErrorNum = _ErrorUnCorrParam; break;}

	    pageNum = Pased_Buff_to_uint32_t (B + 1);
	    if ((pageNum < 0) || (pageNum >= GetFlashPageCount (flashNum))) 
	    	{ErrorNum = _ErrorUnCorrParam; break;}

	    if (Pased_Buff_to_uint32_t (B + 5) != 0xEB1C5A3F)
	    	{ErrorNum = _ErrorUnCorrParam; break;}

	    if (CalculateCheckSum (B, 1 + 4 + 4 + pageSize) !=
	        B [1 + 4 + 4 + pageSize])
	    	{ErrorNum = _ErrorKC; break;}

	    res = WriteFlashPage (flashNum, pageNum, B + 1 + 4 + 4);

	    if (res < 0) {
	      ErrorNum = _ErrorInternalError;
	      break;
	    }
	    *pSize = 2 + 1 + 4;
	    break;
	  }
  
	  case _ReadFlashMemPage:
	  {
	    if (*pSize != 2 + 1 + 4) {ErrorNum = _ErrorSize; break;}
			flashNum = B [0];
	    if ((flashNum < 0) || (flashNum >= GetFlashCount ()))
	    	{ErrorNum = _ErrorUnCorrParam; break;}
    
	    pageSize = GetFlashPageSize (flashNum);
	    if (pageSize * 2 + 30 > _SizeBuffRS485)
	    	{ErrorNum = _ErrorComandImposWork; break;}
    
	    pageNum = Pased_Buff_to_uint32_t (B + 1);
	    if ((pageNum < 0) || (pageNum >= GetFlashPageCount (flashNum)))
	    	{ErrorNum = _ErrorUnCorrParam; break;}
    
	    res = ReadFlashPage (flashNum, pageNum, B + 1 + 4);
    
	    if (res < 0)
	    	{ErrorNum = _ErrorInternalError; break;}

	    B [1 + 4 + pageSize] = CalculateCheckSum (B, 1 + 4 + pageSize);
	    
	    *pSize = 2 + 1 + 4 + pageSize + 1;
	    break;
	  }
 
	  case _ReadFlashMemParam:
	  {
	    if (*pSize != 2) {ErrorNum = _ErrorSize; break;}
	    for (flashNum = 0; flashNum < flashNumMax; flashNum++)
	    {
	      Pased_int32_t_to_Buff (B + 8 * flashNum, GetFlashPageCount (flashNum));
	      Pased_int32_t_to_Buff (B + 8 * flashNum + 4, GetFlashPageSize (flashNum));
	    }
	    *pSize = 2 + 8 * flashNumMax;
	    break;
		}
	
	  case _ReadDescrFlashMem:
	  {
			ErrorNum = _ErrorUnKnowComand;
			break;
			/*
	    if (*pSize != 2 + 4) {ErrorNum = _ErrorSize; break;}
			flashNum = Pased_Buff_to_uint32_t (B);
	    if ((flashNum < 0) || (flashNum >= GetFlashCount ()))
	    	{ErrorNum = _ErrorUnCorrParam; break;}
	    j = GetFlashDescription (flashNum, B + 4);
	    *pSize = 2 + 4 + j;
	    break;
			*/
	  }
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
