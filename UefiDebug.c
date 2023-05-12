/*
** ===========================================================================
** File: UefiDebug.c
** Description: UEFI debug addon
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
#include <Library/SerialPortLib.h>
#include "UefiDebug.h"

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
** Function: DebugPrtPrint()
** Description: Writes n characters of input buffer to output buffer
** Input:
**		pszInChar: input buffer
**		nChars: characters to write to output
** Output: function itself
** Return value: NULL -> Failure, buffer -> Success
** ===========================================================================
*/
EFI_STATUS
DebugPrtPrint(
	IN INTN nAssertLevel,
	IN CHAR8 *pszAssertType,
	IN CHAR8 *pszFunction,
	IN CHAR8 *pszExpression,
	IN CHAR8 *pszFile,
	IN INTN nLine
)
{
	UINTN   nReturn;
	CHAR8   pszBuffer[DBGMSG_MAX_CHARS];
	if (pszAssertType == NULL || pszExpression == NULL || pszFile == NULL)
		return ASSERTION_NEVER_GET_HERE;
	if (nAssertLevel == ASSERTION_NEVER_GET_HERE)
		AsciiSPrint(pszBuffer, sizeof(pszBuffer), "%a:%a->Cannot continue... (file %a, line %d)\n", pszAssertType, pszFunction, pszFile, nLine);
	if (nAssertLevel == ASSERTION_NOT_IMPLEMENTED_YET)
		AsciiSPrint(pszBuffer, sizeof(pszBuffer), "%a:%a->Function not implemented! (file %a, line %d)\n", pszAssertType, pszFunction, pszFile, nLine);
	else
		AsciiSPrint(pszBuffer, sizeof(pszBuffer), "%a:%a->%a (file %a, line %d)\n", pszAssertType, pszFunction, pszExpression, pszFile, nLine);
	SerialPortWrite((UINT8 *)pszBuffer, AsciiStrLen(pszBuffer));
	return nAssertLevel;
}