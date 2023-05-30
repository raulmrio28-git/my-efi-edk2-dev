/*
** ===========================================================================
** File: Image_Qoi.c
** Description: UEFI graphics-related code module (Quite OK Image (QOI) mani-
** pulation)
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
#include "Rectangle.h"
#include "GOP.h"
#include "Image_Qoi.h"
/*
** ===========================================================================
** QOI stuff goes here.
** This package does not contain the necessary qoi.h header. You can find it
** on the official QOI repository on Github:
** https://github.com/phoboslab/qoi
** ===========================================================================
*/
/* Necessary for QOI code to compile */
#define QOI_IMPLEMENTATION
/* Until then, we have to make our own equivalent of qoi_read and qoi_write to use EDK file IO-specific stuff */
#define QOI_NO_STDIO
/* Define QOI memory-related functions as EDK pool and memory functions */
#define QOI_MALLOC(size) AllocateZeroPool(size)
#define QOI_FREE(buf) FreePool(buf)
#define QOI_ZEROARR(buf) SetMem(buf, sizeof(buf), 0) 
/* Header inclusion */
#include "qoi.h"

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
** Function: ConvertQOIToGopBlt()
** Description: Converts QOI image to GOP BLT data
** Input:
**		pImage: Image itself
**		nImageSize: Image size
**		ptGopBlt: GOP BLT buffer
**		pnGopBltSize: output GOP BLT size
**		ptQoiDescription: QOI description structure
** Output: converted QOI image to valid GOP BLT data
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
ConvertQoiToGopBlt(
	IN		UINT8*	pImage,
	IN		UINTN	nImageSize,
	IN OUT	VOID**	ptGopBlt,
	IN OUT	UINTN*	nGopBltSize,
	IN OUT	qoi_desc*	ptQoiDescription
)
{
	BOOLEAN		bIsAllocated;
	UINT32		nWidth;
	UINT32		nHeight;
	UINT64	nBltBufferSize;
	ASSERT_ENSURE(pImage != NULL || nImageSize != 0);
	CopyMem(ptQoiDescription, pImage+4, sizeof(qoi_desc));
	ASSERT_CHECK(*(unsigned int *)pImage == 'fioq');
	bIsAllocated = FALSE;
	ASSERT_CHECK((*ptGopBlt = qoi_decode(pImage, nImageSize, ptQoiDescription, 3)) != NULL);
	bIsAllocated = TRUE;
	ptQoiDescription->width = SwapBytes32(ptQoiDescription->width);
	ptQoiDescription->height = SwapBytes32(ptQoiDescription->height);
	nBltBufferSize = MultU64x32((UINT64)ptQoiDescription->width, ptQoiDescription->height);
	ASSERT_CHECK((nBltBufferSize > DivU64x32((UINTN)~0, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) == 0);
	nBltBufferSize = MultU64x32(nBltBufferSize, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	*nGopBltSize = (UINTN)nBltBufferSize;
	return EFI_SUCCESS;
}

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
)
{
	UINTN			nGopBltSize;
	qoi_desc		tQoiDesc;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL* ptGopBlt;
	ASSERT_ENSURE(ptGraphicsOutput != NULL || pBitmap != NULL || nBitmapSize != 0 || ptRect != NULL);
	ASSERT_CHECK_EFISTATUS(ConvertQoiToGopBlt(pBitmap, nBitmapSize, &ptGopBlt, &nGopBltSize, &tQoiDesc));
	ASSERT_DEBUG_MSGONLY("tQoiDesc->Width=%d, Height=%d, Channels=%d, Colorspace=%d", tQoiDesc.width, tQoiDesc.height, tQoiDesc.channels, tQoiDesc.colorspace);
	ptRect->nRight = ptRect->nLeft + tQoiDesc.width - 1;
	ptRect->nBottom = ptRect->nTop + tQoiDesc.height - 1;
	DrawBlt(ptGraphicsOutput, ptGopBlt, EfiBltBufferToVideo, ptRect);
	return EFI_SUCCESS;
}