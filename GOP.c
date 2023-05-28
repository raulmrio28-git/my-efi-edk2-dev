/*
** ===========================================================================
** File: GOP.c
** Description: UEFI graphics-related code module (GOP)
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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "UefiDebug.h"
#include "GOP.h"
#include "Rectangle.h"

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
** Function: DrawBlt()
** Description: Outputs graphical image to screen
** Input:
**		ptGraphicsOutput: Output protocol
**		ptBlt: BLT pixel buffer
**		nMode: BLT opmode
**		ptRect: Rectangle with info about position
** Output: BLT data output on the screen
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
EFI_STATUS
DrawBlt(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL *ptGraphicsOutput,
	IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ptBlt,
	IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION nMode,
	IN RECT*  ptRect
)
{
	EFI_STATUS nStatus = EFI_SUCCESS;
	UINTN      nX = ptRect->nLeft;
	UINTN      nY = ptRect->nTop;
	UINTN      nWidth = WidthRect(ptRect);
	UINTN      nHeight = HeightRect(ptRect);

	ASSERT_DEBUG_MSGONLY("Pos-> X=%d, Y=%d; Size= %d\x78%d", nX, nY, nWidth, nHeight);
	switch (nMode) {
	case EfiBltVideoFill:
	case EfiBltBufferToVideo:
		nStatus = ptGraphicsOutput->Blt(
			ptGraphicsOutput,
			ptBlt,
			nMode,
			0,
			0,
			nX,
			nY,
			nWidth,
			nHeight,
			0);
		break;

	case EfiBltVideoToBltBuffer:
		nStatus = ptGraphicsOutput->Blt(
			ptGraphicsOutput,
			ptBlt,
			nMode,
			nX,
			nY,
			0,
			0,
			nWidth,
			nHeight,
			0
		);
		break;

	default:
		break;

	}

	ASSERT_DEBUG_MSGONLY("Err=%d, GOPMode=%d, BLTMode=%d, Pos-> X=%d, Y=%d; Size= %d\x78%d", nStatus, ptGraphicsOutput->Mode->Mode, nMode, nX, nY, nWidth, nHeight);

	return nStatus;
}