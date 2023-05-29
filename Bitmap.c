/*
** ===========================================================================
** File: Bitmap.c
** Description: UEFI graphics-related code module (bitmap (BMP) manipulation)
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include <Uefi.h>
#include <Pi/PiFirmwareFile.h>
#include <Protocol/GraphicsOutput.h>
#include <IndustryStandard/Bmp.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "Rectangle.h"
#include "GOP.h"
#include "UefiDebug.h"
#include "Bitmap.h"

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
** Function: FlipBmpRows()
** Description: Flips bitmap image
** Input:
**		pImage: Image itself
**		nWidth: Width
**		nHeight: Height
**		nBPP: Bits per pixels
** Output: Flipped image
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
FlipBmpRows(
	IN OUT	UINT8* pImage,
	IN		UINTN nWidth,
	IN		UINTN nHeight,
	IN		UINTN nBPP
)
{
	UINT8* pTemp;
	INTN   nCurrentLine;
	ASSERT_ENSURE(pImage != NULL || nWidth != 0 && nHeight != 0 || nBPP != 0);
	ASSERT_CHECK((pTemp = (UINT8*)AllocateZeroPool(nWidth*nHeight*(nBPP / 8))) != NULL);
	CopyMem(pTemp, pImage, (nWidth*nHeight*(nBPP / 8)));
	SetMem(pImage, (nWidth*nHeight*(nBPP / 8)), 0);
	for (nCurrentLine = 0; nCurrentLine < nHeight; nCurrentLine++)
	{
		CopyMem(pImage + (nCurrentLine*nWidth*(nBPP / 8)), pTemp + (nHeight - 1 - nCurrentLine)*nWidth*(nBPP / 8), (nWidth*(nBPP / 8)));
	}
	FreePool(pTemp);
	return EFI_SUCCESS;
}

/*
** ===========================================================================
** Function: ReadBmpHdr()
** Description: Initializes bitmap info structure from a bitmap file
** Input:
**		pImage: Image itself
**		nImageSize: Image size
**		ptBmpHeader: Bitmap info structure
** Output: init-ed bitmap info structure
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
ReadBmpHdr(
	IN		UINT8*		pImage,
	IN		INTN		nImageSize,
	IN OUT	BMP_PROCESS_HEADER*	ptBmpHeader
)
{
	BMP_PIXEL_BITS tBmpBitFields; /* used for BI_BITFIELDS (0x3) compression */
	ASSERT_ENSURE(pImage != NULL || nImageSize > 0);
	CopyMem(&ptBmpHeader->tBmpHeader, pImage, sizeof(BMP_HEADER));
	ASSERT_CHECK(ptBmpHeader->tBmpHeader.wType == 'MB');
	if (ptBmpHeader->tBmpHeader.nHeight < 0)
	{
		ptBmpHeader->bIsUpsideDown = TRUE;
		ptBmpHeader->tBmpHeader.nHeight *= (-1);
	}
	else
		ptBmpHeader->bIsUpsideDown = FALSE;
	ptBmpHeader->tBmpHeader.nImgSize = ptBmpHeader->tBmpHeader.nWidth*ptBmpHeader->tBmpHeader.nHeight*(ptBmpHeader->tBmpHeader.nBPP / 8);
	if (ptBmpHeader->tBmpHeader.nWidth % 2)
	{
		ptBmpHeader->tBmpHeader.nImgSize += ptBmpHeader->tBmpHeader.nHeight*(ptBmpHeader->tBmpHeader.nBPP / 8);
	}
	if (ptBmpHeader->tBmpHeader.nCompression == BITMAP_CMP_BITFIELD)
	{
		CopyMem(&tBmpBitFields, pImage + sizeof(BMP_HEADER), sizeof(BMP_PIXEL_BITS));
		ASSERT_DEBUG_MSGONLY("redbits : 0x%08x, greenbits : 0x%08x, bluebits : 0x%08x, transpbits : 0x%08x", tBmpBitFields.nRedBits, tBmpBitFields.nGreenBits, tBmpBitFields.nBlueBits, tBmpBitFields.nTranspBits);
	}
	if (ptBmpHeader->tBmpHeader.nBPP == 1)
		ptBmpHeader->nBitPerPixel = BITMAP_BPP_1BPP;
	if (ptBmpHeader->tBmpHeader.nBPP == 4)
		ptBmpHeader->nBitPerPixel = BITMAP_BPP_4BPP;
	if (ptBmpHeader->tBmpHeader.nBPP == 8)
		ptBmpHeader->nBitPerPixel = BITMAP_BPP_8BPP;
	if (ptBmpHeader->tBmpHeader.nBPP == 16)
	{
		if (ptBmpHeader->tBmpHeader.nCompression == BITMAP_CMP_BITFIELD)
		{
			if (tBmpBitFields.nBlueBits == 0xf00 && tBmpBitFields.nGreenBits == 0xf0 && tBmpBitFields.nRedBits == 0xf)
				ptBmpHeader->nBitPerPixel = BITMAP_BPP_16BPP4444;
			if (tBmpBitFields.nBlueBits == 0xf800 && tBmpBitFields.nGreenBits == 0x7e0 && tBmpBitFields.nRedBits == 0x1f)
				ptBmpHeader->nBitPerPixel = BITMAP_BPP_16BPP565;
		}
		else
			ptBmpHeader->nBitPerPixel = BITMAP_BPP_16BPP1555;
	}
	if (ptBmpHeader->tBmpHeader.nBPP == 24)
		ptBmpHeader->nBitPerPixel = BITMAP_BPP_24BPP888;
	if (ptBmpHeader->tBmpHeader.nBPP == 32)
		ptBmpHeader->nBitPerPixel = BITMAP_BPP_32BPP8888;
	return EFI_SUCCESS;
}

/*
** ===========================================================================
** Function: ConvertBmpToGopBlt()
** Description: Converts bitmap to GOP BLT data
** Input:
**		pImage: Image itself
**		nImageSize: Image size
**		ptBmpHeader: Bitmap info structure
** Output: init-ed bitmap info structure
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
ConvertBmpToGopBlt(
	IN		UINT8*	pImage,
	IN		UINTN	nImageSize,
	IN OUT	VOID**	ptGopBlt,
	IN OUT	UINTN*	nGopBltSize,
	IN		BMP_PROCESS_HEADER*	ptBmpHeader
)
{
	UINT8* pImageHeader;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL*	ptBltBuffer;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL*	Blt;
	BMP_COLOR_MAP*		ptBmpColorMap;
	BOOLEAN	bIsAllocated;
	UINT64	nBltBufferSize;
	UINT32	nDataSizePerLine;
	UINT32	nColorMapNum;
	UINTN	nImageIndex;
	UINTN	Height;
	UINTN	nIndex;
	UINTN	Width;
	ASSERT_DEBUG_MSGONLY("tBmpHeader->Width=%d, Height=%d, BPP=%d, Compression=%d, Size=%d, UpsideDown?=%a", ptBmpHeader->tBmpHeader.nWidth, ptBmpHeader->tBmpHeader.nHeight, ptBmpHeader->tBmpHeader.nBPP, ptBmpHeader->tBmpHeader.nCompression, ptBmpHeader->tBmpHeader.nImgSize, (ptBmpHeader->bIsUpsideDown == TRUE) ? "TRUE" : "FALSE");
	ASSERT_CHECK(sizeof(BMP_HEADER) < nImageSize);
	nDataSizePerLine = ((ptBmpHeader->tBmpHeader.nWidth * ptBmpHeader->tBmpHeader.nBPP + 31) >> 3) & (~0x3);
	nBltBufferSize = MultU64x32(nDataSizePerLine, ptBmpHeader->tBmpHeader.nHeight);
	ASSERT_CHECK((nBltBufferSize > (UINT32)~0) == 0);
	ASSERT_CHECK(ptBmpHeader->tBmpHeader.nSize > ptBmpHeader->tBmpHeader.nImgOffset);
	ASSERT_CHECK(ptBmpHeader->tBmpHeader.nImgOffset >= sizeof(BMP_HEADER));
	if (ptBmpHeader->tBmpHeader.nBPP <= 8)
	{
		nColorMapNum = (ptBmpHeader->tBmpHeader.nImgOffset - sizeof(BMP_HEADER) - ptBmpHeader->tBmpHeader.nInfoHdrSize)/4;
		ptBmpColorMap = (BMP_COLOR_MAP*)AllocateZeroPool(nColorMapNum * 4);
		CopyMem(ptBmpColorMap, pImage + sizeof(BMP_HEADER), nColorMapNum * 4);
	}
	else
		nColorMapNum = 0;
	pImage += ptBmpHeader->tBmpHeader.nImgOffset;
	pImageHeader = pImage;
	// Flip our image if it is upside down (i.e. height is negative)
	if (ptBmpHeader->bIsUpsideDown == TRUE)
	{
		FlipBmpRows(pImage, ptBmpHeader->tBmpHeader.nWidth, ptBmpHeader->tBmpHeader.nHeight, ptBmpHeader->tBmpHeader.nBPP);
		ptBmpHeader->bIsUpsideDown = FALSE;
	}
	nBltBufferSize = MultU64x32((UINT64)ptBmpHeader->tBmpHeader.nWidth, ptBmpHeader->tBmpHeader.nHeight);
	ASSERT_CHECK((nBltBufferSize > DivU64x32((UINTN)~0, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) == 0);
	nBltBufferSize = MultU64x32(nBltBufferSize, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	bIsAllocated = FALSE;
	*nGopBltSize = (UINTN)nBltBufferSize;
	*ptGopBlt = AllocatePool(*nGopBltSize);
	bIsAllocated = TRUE;
	ASSERT_CHECK(*ptGopBlt != NULL);
	ptBltBuffer = *ptGopBlt;
	for (Height = 0; Height < ptBmpHeader->tBmpHeader.nHeight; Height++) {
		Blt = &ptBltBuffer[(ptBmpHeader->tBmpHeader.nHeight - Height - 1) * ptBmpHeader->tBmpHeader.nWidth];
		for (Width = 0; Width < ptBmpHeader->tBmpHeader.nWidth; Width++, pImage++, Blt++) {
			switch (ptBmpHeader->nBitPerPixel) {
			case BITMAP_BPP_1BPP:
				for (nIndex = 0; nIndex < 8 && Width < ptBmpHeader->tBmpHeader.nWidth; nIndex++) {
					Blt->Red = ptBmpColorMap[((*pImage) >> (7 - nIndex)) & 0x1].Red;
					Blt->Green = ptBmpColorMap[((*pImage) >> (7 - nIndex)) & 0x1].Green;
					Blt->Blue = ptBmpColorMap[((*pImage) >> (7 - nIndex)) & 0x1].Blue;
					Width++;
					Blt++;
				}
				Width--;
				Blt--;
				break;

			case BITMAP_BPP_4BPP:
				nIndex = (*pImage) >> 4;
				Blt->Red = ptBmpColorMap[nIndex].Red;
				Blt->Green = ptBmpColorMap[nIndex].Green;
				Blt->Blue = ptBmpColorMap[nIndex].Blue;
				if (Width < (ptBmpHeader->tBmpHeader.nWidth - 1)) {
					Width++;
					Blt++;
					nIndex = (*pImage) & 0x0f;
					Blt->Red = ptBmpColorMap[nIndex].Red;
					Blt->Green = ptBmpColorMap[nIndex].Green;
					Blt->Blue = ptBmpColorMap[nIndex].Blue;
				}
				break;

			case BITMAP_BPP_8BPP:
				Blt->Red = ptBmpColorMap[*pImage].Red;
				Blt->Green = ptBmpColorMap[*pImage].Green;
				Blt->Blue = ptBmpColorMap[*pImage].Blue;
				break;

			case BITMAP_BPP_16BPP1555:
				Blt->Blue = (((*((UINT16 *)pImage))) & 0x1F) << 3;
				Blt->Green = (((*((UINT16 *)pImage)) >> 5) & 0x1F) << 3;
				Blt->Red = (((*((UINT16 *)pImage)) >> 10) & 0x1F) << 3;
				pImage++;
				break;

			case BITMAP_BPP_16BPP565:
				Blt->Blue = (((*((UINT16 *)pImage))) & 0x1F) << 3;
				Blt->Green = (((*((UINT16 *)pImage)) >> 5) & 0x3F) << 2;
				Blt->Red = (((*((UINT16 *)pImage)) >> 11) & 0x1F) << 3;
				pImage++;
				break;

			case BITMAP_BPP_16BPP4444:
				Blt->Blue = (((*((UINT16 *)pImage))) & 0xF) << 4;
				Blt->Green = (((*((UINT16 *)pImage)) >> 4) & 0xF) << 4;
				Blt->Red = (((*((UINT16 *)pImage)) >> 8) & 0xF) << 4;
				pImage++;
				break;

			case BITMAP_BPP_24BPP888:
				Blt->Blue = *pImage++;
				Blt->Green = *pImage++;
				Blt->Red = *pImage;
				break;

			case BITMAP_BPP_32BPP8888:
				Blt->Blue = *pImage++;
				Blt->Green = *pImage++;
				Blt->Red = *pImage++;
				break;

			default:
				ASSERT_DEBUG_MSGONLY("Fail, BPP: %d", ptBmpHeader->nBitPerPixel);
				if (bIsAllocated) {
					FreePool(*ptGopBlt);
					*ptGopBlt = NULL;
				}
				return EFI_LOAD_ERROR;
				break;
			};
		}
		nImageIndex = (UINTN)(pImage - pImageHeader);
		if ((nImageIndex % 4) != 0) {
			pImage = pImage + (4 - (nImageIndex % 4));
		}
	}
	if (nColorMapNum != 0)
		FreePool(ptBmpColorMap);
	return EFI_SUCCESS;
};

/*
** ===========================================================================
** Function: DrawBmpImage()
** Description: Outputs bitmap image to screen
** Input:
**		ptGraphicsOutput: GOP
**		pBitmap: Image itself
**		nBitmappSize: Image size
**		ptBmpHeader: Bitmap info structure
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
)
{
	UINTN			nGopBltSize;
	BMP_PROCESS_HEADER tBmpProcess;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL* ptGopBlt;
	ASSERT_ENSURE(ptGraphicsOutput != NULL || pBitmap != NULL || nBitmapSize != 0 || ptRect != NULL);
	ASSERT_CHECK_EFISTATUS(ReadBmpHdr(pBitmap, nBitmapSize, &tBmpProcess));
	ASSERT_DEBUG_MSGONLY("tBmpHeader->Width=%d, Height=%d, BPP=%d, Compression=%d, Size=%d, UpsideDown?=%a", tBmpProcess.tBmpHeader.nWidth, tBmpProcess.tBmpHeader.nHeight, tBmpProcess.tBmpHeader.nBPP, tBmpProcess.tBmpHeader.nCompression, tBmpProcess.tBmpHeader.nImgSize, (tBmpProcess.bIsUpsideDown == TRUE) ? "TRUE" : "FALSE");
	ASSERT_CHECK_EFISTATUS(ConvertBmpToGopBlt(pBitmap, nBitmapSize, &ptGopBlt, &nGopBltSize, &tBmpProcess));
	ptRect->nRight = ptRect->nLeft + tBmpProcess.tBmpHeader.nWidth - 1;
	ptRect->nBottom = ptRect->nTop + tBmpProcess.tBmpHeader.nHeight - 1;
	DrawBlt(ptGraphicsOutput, ptGopBlt, EfiBltBufferToVideo, ptRect);
	return EFI_SUCCESS;
}