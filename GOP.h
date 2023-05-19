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
**		ptRect: regtangle with info about position
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