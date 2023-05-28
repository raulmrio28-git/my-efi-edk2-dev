/*
** ===========================================================================
** File: GOP.h
** Description: UEFI graphics-related code module (GOP)
** ===========================================================================
*/

#ifndef _GRAPHICS_GOP_H_
#define _GRAPHICS_GOP_H_

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

enum
{
	FRONT_STYLE_LEFT_TOP,
	FRONT_STYLE_LEFT_CENTER,
	FRONT_STYLE_LEFT_BOTTOM,
	FRONT_STYLE_MIDDLE_TOP,
	FRONT_STYLE_MIDDLE_CENTER,
	FRONT_STYLE_MIDDLE_BOTTOM,
	FRONT_STYLE_RIGHT_TOP,
	FRONT_STYLE_RIGHT_CENTER,
	FRONT_STYLE_RIGHT_BOTTOM
};

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
);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _GRAPHICS_GOP_H_ */