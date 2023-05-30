/*
** ===========================================================================
** File: Image_Qoi.h
** Description: UEFI graphics-related code module (Quite OK Image (QOI) mani-
** pulation)
** ===========================================================================
*/

#ifndef _GRAPHICS_IMAGE_QOI_H_
#define _GRAPHICS_IMAGE_QOI_H_

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
** Function: DrawQoiImage()
** Description: Outputs QOI image to screen
** Input:
**		ptGraphicsOutput: GOP
**		pBitmap: Image itself
**		nBitmappSize: Image size
**		ptRect: Rectangle to modify
** Output: QOI image output on the screen
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
DrawQoiImage(
	IN		EFI_GRAPHICS_OUTPUT_PROTOCOL *ptGraphicsOutput,
	IN		UINT8*		pBitmap,
	IN		UINTN		nBitmapSize,
	IN		RECT		*ptRect
);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _GRAPHICS_IMAGE_QOI_H_ */
