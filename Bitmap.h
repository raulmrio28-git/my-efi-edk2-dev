/*
** ===========================================================================
** File: Bitmap.h
** Description: UEFI graphics-related code module (bitmap (BMP) manipulation)
** ===========================================================================
*/

#ifndef __DRAWLOGO_H__
#define __DRAWLOGO_H__

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <IndustryStandard/Bmp.h>

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

enum
{
	BITMAP_CMP_RGB,
	BITMAP_CMP_RLE8,
	BITMAP_CMP_RLE4,
	BITMAP_CMP_BITFIELD,
	BITMAP_CMP_JPEG,
	BITMAP_CMP_PNG,
	BITMAP_CMP_RGBA,
	BITMAP_CMP_CMYK,
	BITMAP_CMP_CMYKRLE8,
	BITMAP_CMP_CMYKRLE4
};
enum
{
	BITMAP_BPP_1BPP,
	BITMAP_BPP_4BPP,
	BITMAP_BPP_8BPP,
	BITMAP_BPP_16BPP1555,
	BITMAP_BPP_16BPP565,
	BITMAP_BPP_16BPP4444,
	BITMAP_BPP_24BPP888,
	BITMAP_BPP_32BPP8888
};
#pragma pack(push,1)
typedef struct {
	UINT16				wType;
	UINT32				nSize;
	UINT16				wReserved1;
	UINT16				wReserved2;
	UINT32				nImgOffset;
	UINT32				nInfoHdrSize;
	INT32				nWidth;
	INT32				nHeight;
	UINT16				nPlanes;
	UINT16				nBPP;
	UINT32				nCompression;
	UINT32				nImgSize;
	INT32				nXPPM;
	INT32				nYPPM;
	UINT32				nUsedColors;
	UINT32				nImpColors;
} BMP_HEADER;
#pragma pack(pop)
#pragma pack(push,1)
typedef struct {
	UINT32				nBlueBits;
	UINT32				nGreenBits;
	UINT32				nRedBits;
	UINT32				nTranspBits;
} BMP_PIXEL_BITS;
#pragma pack(pop)
#pragma pack(push,1)
typedef struct {
	BMP_HEADER			tBmpHeader;
	UINT32				nBitPerPixel;
	BOOLEAN				bIsUpsideDown;
} BMP_PROCESS_HEADER;
#pragma pack(pop)

/*
**---------------------------------------------------------------------------
**  Function(external use only) Declarations
**---------------------------------------------------------------------------
*/

/*
** ===========================================================================
** Function: DrawBmpImage()
** Description: Outputs bitmap image to screen
** Input:
**		ptGraphicsOutput: GOP
**		pBitmap: Image itself
**		nBitmappSize: Image size
**		pBmpHeader: Bitmap info structure
** Output: Bitmap image output on the screen
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
DrawBmpImage(
	IN		EFI_GRAPHICS_OUTPUT_PROTOCOL *ptGraphicsOutput,
	IN		UINT8*		pBitmap,
	IN		UINTN		nBitmapSize,
	IN		RECT		*ptRect
);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif