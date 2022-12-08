/*===================================================
	UEFI Environment
	Print services for additional addons to Print()
	1.0
===================================================*/

//
// Basic UEFI Libraries
//
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/Dxe/Print/Print.h>
//
// Defines
//
#define CMD_LINE_LINES 80
/***
  DivApproxNext()

  Alternate for ceil().
  Return rounded division toward -inf to integral value when diving 2 numbers.
***/
INTN
DivApproxNext(
	INTN a,
	INTN b
)
{
	return (a / b) + ((a % b) != 0);
}

/***
  UnicodeStrSect()

  Alternate for strndup().
  Returns a pointer to a copy of input with at most n characters
  The result is always NUL terminated.
***/
CHAR16 
*UnicodeStrSect(
	IN CONST CHAR16 *inChar,
	IN INT32 noOfChars
)
{
	CHAR16 *buffer;
	buffer = AllocatePool((noOfChars + 1) * sizeof(CHAR16));
	if (buffer == NULL)
		return NULL;
	buffer[noOfChars] = 0;
	return (CHAR16*)CopyMem(buffer, inChar, noOfChars * sizeof(CHAR16));
}

/***
  UnicodeSplit()

  Splits a string to n strings, using a string length variable.
***/
CHAR16**
UnicodeSplit(
	IN CONST CHAR16 *inChar,
	IN INTN noOfChars,
	OUT INTN *cntStrs,
	CHAR16 **outArray
)
{
	INTN strLen = wcslen(inChar);
	INTN sectCnt = DivApproxNext(wcslen(inChar), noOfChars);
	INTN t = 0;
	INTN myCnt = 0;
	*cntStrs = sectCnt;
	outArray = AllocateZeroPool((sectCnt) * sizeof(CHAR16*));
	if (outArray == NULL)
		return NULL;
	for (myCnt = 0, t = 0; strLen > 0; t++ )
	{
		if (strLen > sectCnt)
			myCnt = noOfChars;
		else
			myCnt = strLen;
		outArray[t] = UnicodeStrSect(inChar + (noOfChars * t), myCnt);
		if (strLen > sectCnt)
			strLen-= noOfChars;
		else
			strLen = 0;
	}
	return outArray;
}

/***
  PrintCenter()

  Prints a string in a centered position.

  Example of usage:
  PrintCenter(L"Hi! I'm a centered string and it is printed centered thanks to my PrintCenter() function!\n");
  Returns:
  'Hi! I'm a centered string and it is printed centered thanks to my PrintCenter()'
  '                                   function!                                   '

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
UINTN
PrintCenter(
	IN CHAR16 *inChar,
	...
)
{
	UINTN   Return;
	VA_LIST Marker;
	CHAR16  Buffer[CMD_LINE_LINES + 1];
	INT32	centerHalfWidth;
	CHAR16  inTmp[EFI_DRIVER_LIB_MAX_PRINT_BUFFER];
	CHAR16** strArray = NULL;
	INTN	cntStrs = 0;
	INT32	t;
	Buffer[CMD_LINE_LINES] = 0;
	//If there is a newline, remove it from input buffer.
	if (inChar[wcslen(inChar) - 1] == '\n')
		inChar[wcslen(inChar) - 1] = 0;
	VA_START(Marker, inChar);
	Return = UnicodeVSPrint(inTmp, sizeof(inTmp), inChar, Marker);
	VA_END(Marker);
	strArray = UnicodeSplit(inTmp, CMD_LINE_LINES, &cntStrs, strArray);
	for (t = 0; t < cntStrs; t++)
	{
		// There is no solution for printing indentation with SPrint, so we SetMem with
		// spaces our temp buffer and write our string!
		SetMem16(Buffer, CMD_LINE_LINES*2, (CHAR16)' ');
		centerHalfWidth = (INT32)(CMD_LINE_LINES - wcslen(strArray[t])) / 2;
		CopyMem(Buffer + centerHalfWidth, strArray[t], wcslen(strArray[t]) * sizeof(CHAR16));
		Print(Buffer);
	}
	//Print(Buffer);
	/*Causes a crash, so comment it
	while (t > 0)
	{
		FreePool(strArray[t--]);
	}
	*/
	FreePool(strArray);
	return Return;
}