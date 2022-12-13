/*
** ===========================================================================
** File: AdditionalPrintSvcs.c
** Description: UEFI print addons (center print, border print etc.)
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/Dxe/Print/Print.h>
#include "AdditionalPrintSvcs.h"

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Global variables
**---------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Internal variables
**---------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Function(internal use only) Declarations
**---------------------------------------------------------------------------
*/

/*
** ===========================================================================
** Function: UnicodeStrSect()
** Description: Writes n characters of input buffer to output buffer
** Input: 
**		pszInChar: input buffer
**		nChars: characters to write to output
** Output: function itself
** Return value: NULL -> Failure, buffer -> Success
** ===========================================================================
*/
CHAR16 
*UnicodeStrSect(
	IN CONST CHAR16 *pszInChar,
	IN INT32 nChars
)
{
	CHAR16 *pszBuffer = AllocatePool((nChars + 1) * sizeof(CHAR16));
	if (pszBuffer == NULL)
		return NULL;
	pszBuffer[nChars] = 0;
	return (CHAR16*)CopyMem(pszBuffer, pszInChar, nChars * sizeof(CHAR16));
}

/*
** ===========================================================================
** Function: UnicodeSplit()
** Description: Splits a string to n strings, using a string length variable
** Input:
**		pszInChar: input buffer
**		nChars: characters to write to output
**		nStrCnt: string count
**		poutArray: out array (it will return itself after function's done)
** Output: poutArray
** Return value: NULL -> Failure, poutArray -> Success
** ===========================================================================
*/

CHAR16
**UnicodeSplit(
	IN CONST CHAR16 *pszInChar,
	IN INTN nChars,
	OUT INTN *nStrCnt,
	CHAR16 **poutArray
)
{
	INTN nStrLen = wcslen(pszInChar);
	INTN nSections = DivApproxNext(wcslen(pszInChar), nChars);
	INTN nCurrSect = 0;
	INTN nTempCnt = 0;
	*nStrCnt = nSections;
	poutArray = AllocateZeroPool((nSections) * sizeof(CHAR16*));
	if (poutArray == NULL)
		return NULL;
	for (nTempCnt = 0, nCurrSect = 0; nStrLen > 0; nCurrSect++)
	{
		if (nStrLen > nSections)
			nTempCnt = nChars;
		else
			nTempCnt = nStrLen;
		poutArray[nCurrSect] = UnicodeStrSect(pszInChar + (nChars * nCurrSect), nTempCnt);
		if (nStrLen > nSections)
			nStrLen -= nChars;
		else
			nStrLen = 0;
	}
	return poutArray;
}

/*
** ===========================================================================
** Function: PrintHBorder()
** Description: Prints a horizontal border
** Input:
**		nCnt: border size
** Output: Printed buffer
** Return value: Other -> Failure, 0 -> Success
** ===========================================================================
*/
EFI_STATUS
PrintHBorder(
	IN INTN nCnt
)
{
	CHAR16 *pBuffer = AllocatePool((nCnt + 1) * sizeof(CHAR16));
	if (pBuffer == NULL)
		return EFI_OUT_OF_RESOURCES;
	pBuffer[nCnt] = 0;
	SetMem16(pBuffer, nCnt * 2, (CHAR16)BORDER_CHARACTER);
	return Print(pBuffer);
}

/*
**---------------------------------------------------------------------------
**  Function(external use only) Declarations
**---------------------------------------------------------------------------
*/

/*
** ===========================================================================
** Function: PrintAlign()
** Description: Prints a string in whatever orientation the user wants
** Input:
**		pszInChar: Input string
**		nAlignment: Alignment value
**		...: blablabla
** Output: Printed buffer in aligned position
** Return value: Other -> Failure, 0 -> Success
** ===========================================================================
*/
EFI_STATUS
PrintAlign(
	IN CHAR16 *pszInChar,
	IN INTN nAlignment OPTIONAL,
	...
)
{
	UINTN   nReturn;
	VA_LIST vMarker;
	CHAR16  pszBuffer[CMD_LINE_LINES + 1];
	INT32	nNoOfSpaces;
	CHAR16  pszInTmp[EFI_DRIVER_LIB_MAX_PRINT_BUFFER];
	CHAR16** pStrArray = NULL;
	INTN	nStrCnt = 0;
	INT32	nTempCnt;
	pszBuffer[CMD_LINE_LINES] = 0;
	//If there is a newline, remove it from input buffer.
	if (pszInChar[wcslen(pszInChar) - 1] == '\n')
		pszInChar[wcslen(pszInChar) - 1] = 0;
	VA_START(vMarker, nAlignment);
	nReturn = UnicodeVSPrint(pszInTmp, sizeof(pszInTmp), pszInChar, vMarker);
	VA_END(vMarker);
	pStrArray = UnicodeSplit(pszInTmp, CMD_LINE_LINES, &nStrCnt, pStrArray);
	for (nTempCnt = 0; nTempCnt < nStrCnt; nTempCnt++)
	{
		// There is no solution for printing indentation with SPrint, so we SetMem with
		// spaces our temp buffer and write our string!
		SetMem16(pszBuffer, CMD_LINE_LINES * 2, (CHAR16)' ');
		nNoOfSpaces = (INT32)(CMD_LINE_LINES - wcslen(pStrArray[nTempCnt]));
		if (nAlignment == PR_CENTER)
			nNoOfSpaces /= 2;
		if (nAlignment != PR_LEFT)
			CopyMem(pszBuffer + nNoOfSpaces, pStrArray[nTempCnt], wcslen(pStrArray[nTempCnt]) * sizeof(CHAR16));
		else
			CopyMem(pszBuffer, pStrArray[nTempCnt], wcslen(pStrArray[nTempCnt]) * sizeof(CHAR16));
		nReturn = Print(pszBuffer);
	}
	FreePool(pStrArray);
	return nReturn;
}


/*
** ===========================================================================
** Function: PrintBorder()
** Description: Prints a string in a border and in whatever orientation the
** user wants
** Input:
**		pszInChar: Input string
**		nAlignment: Alignment value
**		...: blablabla
** Output: Printed buffer in a border and aligned position
** Return value: Other -> Failure, 0 -> Success
** ===========================================================================
*/
EFI_STATUS
PrintBorder(
	IN CHAR16 *pszInChar,
	IN INTN nAlignment OPTIONAL,
	...
)
{
	UINTN   nReturn;
	VA_LIST vMarker;
	CHAR16  pszBuffer[CMD_LINE_LINES + 1];
	INT32	nNoOfSpaces;
	CHAR16  szInTmp[EFI_DRIVER_LIB_MAX_PRINT_BUFFER];
	CHAR16** pStrArray = NULL;
	INTN	nStrCnt = 0;
	INT32	nTempCnt;
	pszBuffer[CMD_LINE_LINES] = 0;
	//If there is a newline, remove it from input buffer.
	if (pszInChar[wcslen(pszInChar) - 1] == '\n')
		pszInChar[wcslen(pszInChar) - 1] = 0;
	VA_START(vMarker, nAlignment);
	nReturn = UnicodeVSPrint(szInTmp, sizeof(szInTmp), pszInChar, vMarker);
	VA_END(vMarker);
	pStrArray = UnicodeSplit(szInTmp, CMD_LINE_LINES - 4, &nStrCnt, pStrArray);
	PrintHBorder(CMD_LINE_LINES);
	for (nTempCnt = 0; nTempCnt < nStrCnt; nTempCnt++)
	{
		// There is no solution for printing indentation with SPrint, so we SetMem with
		// spaces our temp buffer and write our string!
		SetMem16(pszBuffer, CMD_LINE_LINES * 2, (CHAR16)' ');
		nNoOfSpaces = (INT32)(CMD_LINE_LINES - 4 - wcslen(pStrArray[nTempCnt]));
		pszBuffer[0] = BORDER_CHARACTER;
		pszBuffer[CMD_LINE_LINES - 1] = BORDER_CHARACTER;
		if (nAlignment == PR_CENTER)
			nNoOfSpaces /= 2;
		if (nAlignment != PR_LEFT)
			CopyMem(pszBuffer + nNoOfSpaces + 2, pStrArray[nTempCnt], wcslen(pStrArray[nTempCnt]) * sizeof(CHAR16));
		else
			CopyMem(pszBuffer + 2, pStrArray[nTempCnt], wcslen(pStrArray[nTempCnt]) * sizeof(CHAR16));
		nReturn = Print(pszBuffer);
	}
	PrintHBorder(CMD_LINE_LINES);
	FreePool(pStrArray);
	return nReturn;
}
