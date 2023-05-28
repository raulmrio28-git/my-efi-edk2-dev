/*
** ===========================================================================
** File: Rectangle.c
** Description: UEFI graphics-related code module (rectangles)
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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "Rectangle.h"
#include "UefiDebug.h"

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#define ASSERT_ENSURE_FALSE(expr)    \
	ASSERT_DEBUG(ASSERTION_ENSURE, expr);\
	if (!(expr)) { return FALSE; }

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
** Function: SetRect()
** Description: Sets rectangle buffer data with specific values
** Input:
**		ptRect: Rectangle to be set
**		nLeft, nTop, nRight, nBottom = Values to fill the rectangle with
** Output: Filled rectangle
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
BOOLEAN
EFIAPI
SetRect(
	IN OUT RECT                                  *ptRect,
	IN     UINTN                                 nLeft,
	IN     UINTN                                 nTop,
	IN     UINTN                                 nRight,
	IN     UINTN                                 nBottom
)
{
	ASSERT_ENSURE_FALSE(ptRect != NULL);
	ptRect->nLeft = nLeft;
	ptRect->nTop = nTop;
	ptRect->nRight = nRight;
	ptRect->nBottom = nBottom;
	return TRUE;
}

/*
** ===========================================================================
** Function: CopyRect()
** Description: Copy rectangle to another
** Input:
**		ptDestRect: Rectangle copy destination
**		ptSrcRect: Rectangle copy source
** Output: Copied rectangle
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
BOOLEAN
EFIAPI
CopyRect(
	OUT      RECT                                *ptDestRect,
	IN CONST RECT                                *ptSrcRect
)
{
	ASSERT_ENSURE_FALSE(ptSrcRect != NULL);
	ASSERT_ENSURE_FALSE(ptDestRect != NULL);
	CopyMem(ptDestRect, ptSrcRect, sizeof(RECT));
	return TRUE;
}

/*
** ===========================================================================
** Function: SetRect()
** Description: Shrinks rectangle buffer data with specific values
** Input:
**		ptRect: Rectangle to modify
**		nDiffX, nDiffY = Values to shrink the rectangle with
** Output: Shrunk rectangle
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
BOOLEAN
EFIAPI
ShrinkRect(
	IN OUT RECT                                  *ptRect,
	IN UINTN                                     nDiffX,
	IN UINTN                                     nDiffY
)
{
	ASSERT_ENSURE_FALSE(ptRect != NULL);
	ptRect->nLeft += nDiffX;
	ptRect->nTop += nDiffY;
	ptRect->nRight -= nDiffX;
	ptRect->nBottom -= nDiffY;
	return TRUE;
}

/*
** ===========================================================================
** Function: SetRect()
** Description: Offsets rectangle buffer data with specific values
** Input:
**		ptRect: Rectangle to modify
**		nDiffX, nDiffY = Values to offset the rectangle with
** Output: Offset rectangle
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
BOOLEAN
EFIAPI
OffsetRect(
	IN OUT RECT                                  *ptRect,
	IN     UINTN                                 nDiffX,
	IN     UINTN                                 nDiffY
)
{
	ASSERT_ENSURE_FALSE(ptRect != NULL);
	ptRect->nLeft += nDiffX;
	ptRect->nRight += nDiffX;
	ptRect->nTop += nDiffY;
	ptRect->nBottom += nDiffY;
	return TRUE;
}
