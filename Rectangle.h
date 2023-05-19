/*
** ===========================================================================
** File: Rectangle.h
** Description: UEFI graphics-related code module (rectangles)
** ===========================================================================
*/

#ifndef _GRAPHICS_RECTANGLE_H_
#define _GRAPHICS_RECTANGLE_H_

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

#pragma pack(1)
typedef struct {
	UINTN                                        nLeft;
	UINTN                                        nTop;
	UINTN                                        nRight;
	UINTN                                        nBottom;
} RECT;
#pragma pack()
#define WidthRect(ptRect) (ptRect->nRight - ptRect->nLeft + 1)
#define HeightRect(ptRect) (ptRect->nBottom - ptRect->nTop + 1)

/*
**---------------------------------------------------------------------------
**  Variable Declarations
**---------------------------------------------------------------------------
*/

/*
**---------------------------------------------------------------------------
**  Function(external use only) Declarations
**---------------------------------------------------------------------------
*/

/*
** ===========================================================================
** Function: SetRect()
** Description: Sets rectangle buffer data with specific values
** Input:
**		ptRect: Rectangle to be set
**		nLeft, nTop, nRight, nBottom = values to fill the rectangle with
** Output: filled rectangle
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
);

/*
** ===========================================================================
** Function: CopyRect()
** Description: Copy rectangle to another
** Input:
**		ptDestRect: Rectangle copy destination
**		ptSrcRect: Rectangle copy source
** Output: copied rectangle
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
BOOLEAN
EFIAPI
CopyRect(
	OUT      RECT                                *ptDestRect,
	IN CONST RECT                                *ptSrcRect
);

/*
** ===========================================================================
** Function: SetRect()
** Description: Shrinks rectangle buffer data with specific values
** Input:
**		ptRect: Rectangle to modify
**		nDiffX, nDiffY = values to shrink the rectangle with
** Output: shrunk rectangle
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
BOOLEAN
EFIAPI
ShrinkRect(
	IN OUT RECT                                  *ptRect,
	IN UINTN                                     nDiffX,
	IN UINTN                                     nDiffY
);

/*
** ===========================================================================
** Function: SetRect()
** Description: Offsets rectangle buffer data with specific values
** Input:
**		ptRect: Rectangle to modify
**		nDiffX, nDiffY = values to offset the rectangle with
** Output: offset rectangle
** Return value: FALSE -> Failure, TRUE -> Success
** ===========================================================================
*/
BOOLEAN
EFIAPI
OffsetRect(
	IN OUT RECT                                  *ptRect,
	IN     UINTN                                 nDiffX,
	IN     UINTN                                 nDiffY
);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _GRAPHICS_RECTANGLE_H_ */
