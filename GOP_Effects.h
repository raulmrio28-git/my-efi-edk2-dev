/*
** ===========================================================================
** File: GOP_Effects.h
** Description: Display GOP BLT image with specific effects
** Info: The code is adapted from luobing4365's uefi-practical-programming re-
** pository on Gitee. (https://gitee.com/luobing4365/uefi-practical-programming)
** Original file: RobinPkg/Applications/ImageEffect/Window.c
** ===========================================================================
*/

#ifndef _GRAPHICS_GOP_EFFECTS_H_
#define _GRAPHICS_GOP_EFFECTS_H_

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#ifdef __cplusplus
extern "C" {
#endif
#ifndef _GRAPHICS_RECTANGLE_H_
#include "Rectangle.h"
#endif

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

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
** Function: DrawBlt_ImageFade()
** Description: Outputs graphical image to screen with a fade in/out effect
** Input:
**		ptGraphicsOutput: Output protocol
**		ptBlt: BLT pixel buffer
**		bReverse: TRUE = fade out
**		ptRect: Rectangle with info about position
** Output: BLT data output on the screen with respective effect
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
EFIAPI
DrawBlt_ImageFade(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL *ptGraphicsOutput,
	IN CONST	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ptBlt,
	IN BOOLEAN	bReverse,
	IN RECT*  ptRect
);

/*
** ===========================================================================
** Function: DrawBlt_ImageClockWipe()
** Description: Outputs graphical image to screen with a clock wipe effect
** Input:
**		ptGraphicsOutput: Output protocol
**		ptBlt: BLT pixel buffer
**		bIsCounterClockwise: TRUE = counter-clockwise order
**		ptRect: Rectangle with info about position
** Output: BLT data output on the screen with respecive effect
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
EFIAPI
DrawBlt_ImageClockWipe(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL *ptGraphicsOutput,
	IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ptBlt,
	IN BOOLEAN	bIsCounterClockwise,
	IN CONST RECT*  ptRect
);

/*
** ===========================================================================
** Function: DrawBlt_ImageRainFallShow()
** Description: Outputs graphical image to screen with a rainfall effect
** Input:
**		ptGraphicsOutput: Output protocol
**		ptBlt: BLT pixel buffer
**		bIsBottomToTop: TRUE = do the effect in reverse order
**		ptRect: Rectangle with info about position
** Output: BLT data output on the screen with respecive effect
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
EFIAPI
DrawBlt_ImageRainFallShow(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL *ptGraphicsOutput,
	IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ptBlt,
	IN BOOLEAN bIsBottomToTop,
	IN CONST RECT*	ptRect
);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _GRAPHICS_GOP_EFFECTS_H_ */
