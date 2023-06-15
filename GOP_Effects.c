/*
** ===========================================================================
** File: GOP_Effects.c
** Description: Display GOP BLT image with specific effects
** Info: The code is adapted from luobing4365's uefi-practical-programming re-
** pository on Gitee. (https://gitee.com/luobing4365/uefi-practical-programming)
** Some functions will be fixed for proprer output (e.g. fade output code).
** Original file: RobinPkg/Applications/ImageEffect/Window.c
** ===========================================================================
*/

//
// Basic UEFI Libraries
//
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>

//
// Boot and Runtime Services
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

//
// Shell Library
//
#include <Library/ShellLib.h>

//
// Our Libraries
#include "AdditionalPrintSvcs.h"
#include "UefiDebug.h"
#include "Rectangle.h"
#include "GOP.h"

/*
** ===========================================================================
** Function: GopEffects_BresenhamDrawLine()
** Description: Outputs a line on the screen using Bresenham's line drawing
** style.
** Input:
**		ptGraphicsOutput: Output protocol
**		ptGopBlt: BLT pixel buffer
**		nX: X coordonate
**		nY: Y coordonate
**		nWidth: Image width
**		nHeight: Image height
**		nImgX1, nImgX2: Image line X coords
**		nImgy1, nImgy2: Image line y coords
** Output: Line from BLT data output on the screen
** Return value: EFI_LOAD_ERROR -> Failure, EFI_SUCCESS -> Success
** ===========================================================================
*/
EFI_STATUS
EFIAPI
GopEffects_BresenhamDrawLine(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL *ptGraphicsOutput,
	IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ptGopBlt,
	IN UINTN nX,
	IN UINTN nY,
	IN UINTN nWidth,
	IN UINTN nHeight,
	IN UINTN nImgX1,
	IN UINTN nImgY1,
	IN UINTN nImgX2,
	IN UINTN nImgY2
)
{
	INT16 nD;
	INT16 nDx;
	INT16 nDy;
	INT16 nDx2;
	INT16 nDy2;
	INT16 nDxy;
	INT16 nXinc;
	INT16 nYinc;
	INT16 nX1;
	INT16 nX2;
	INT16 nY1;
	INT16 nY2;
	ASSERT_ENSURE(ptGraphicsOutput != NULL || ptGopBlt != NULL | nWidth != 0 || nHeight != 0);
	nX1 = (INT16)(nImgX1);
	nY1 = (INT16)(nImgY1);
	nX2 = (INT16)(nImgX2);
	nY2 = (INT16)(nImgY2);
	if (nX2 > nX1)
		nDx = nX2 - nX1;
	else
		nDx = nX1 - nX2;
	if (nY2 > nY1)
		nDy = nY2 - nY1;
	else
		nDy = nY1 - nY2;
	if (nX2 > nX1)
		nXinc = 1;
	else
		nXinc = (nX2 == nX1) ? 0 : (-1);
	if (nY2 > nY1)
		nYinc = 1;
	else
		nYinc = (nY2 == nY1) ? 0 : (-1);
	nDx2 = (INT16)(nDx << 1);
	nDy2 = (INT16)(nDy << 1);
	ASSERT_CHECK_EFISTATUS(ptGraphicsOutput->Blt(ptGraphicsOutput, &ptGopBlt[nY * nWidth + nX], EfiBltVideoFill, 0, 0, nX + nX1, nY + nY1, 1, 1, 0));
	if (nDx >= nDy) {
		nD = nDy2 - nDx;
		nDxy = nDy2 - nDx2;
		while (nDx--) {
			if (nD <= 0)nD += nDy2;
			else {
				nD += nDxy;
				nY1 += nYinc;
			}
			nX1 += nXinc;
			ASSERT_CHECK_EFISTATUS(ptGraphicsOutput->Blt(ptGraphicsOutput, &ptGopBlt[nY1 * nWidth + nX1], EfiBltVideoFill, 0, 0, nX + nX1, nY + nY1, 1, 1, 0));
		}
	}
	else {
		nD = nDx2 - nDy;
		nDxy = nDx2 - nDy2;
		while (nDy--) {
			if (nD <= 0)nD += nDx2;
			else {
				nD += nDxy;
				nX1 += nXinc;
			}
			nY1 += nYinc;
			ASSERT_CHECK_EFISTATUS(ptGraphicsOutput->Blt(ptGraphicsOutput, &ptGopBlt[nY1* nWidth + nX1], EfiBltVideoFill, 0, 0, nX + nX1, nY + nY1, 1, 1, 0));
		}
	}
	gBS->Stall(2500); /* 2.5ms pause */
	return EFI_SUCCESS;
}

/*
** ===========================================================================
** Function: DrawBlt_ImageFade()
** Description: Outputs graphical image to screen with a fade in/out effect
** Input:
**		ptGraphicsOutput: Output protocol
**		ptBlt: BLT pixel buffer
**		bReverse: TRUE = fade out
**		ptRect: Rectangle with info about position
** Output: BLT data output on the screen with respecive effect
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
)
{
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *ptTempBltBuffer;
	UINT32 nBltBufferSize;
	UINT16 nFadeLevel;
	UINT32 nCurrPixel;
	UINT32 nWidth;
	UINT32 nHeight;
	ASSERT_ENSURE(ptGraphicsOutput != NULL || ptBlt != NULL || ptRect != NULL);
	nWidth = (UINT32)WidthRect(ptRect);
	nHeight = (UINT32)HeightRect(ptRect);
	nBltBufferSize = (nWidth * nHeight * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	ASSERT_CHECK((ptTempBltBuffer = AllocateZeroPool(nBltBufferSize)) != NULL);
	/* The original implementation had a big oopsie. Which?
	It checks if one value the out buffer has less color value than input's.
	A proper fade does a percentage-like operation for every color value. 
	Same goes with fade out, but in reverse order. */
	if (bReverse == FALSE)
	{
		nFadeLevel = 0;
		while (nFadeLevel < 256)
		{
			nCurrPixel = 0;
			while (nCurrPixel < (nWidth * nHeight))
			{
				ptTempBltBuffer[nCurrPixel].Red = (UINT8)((ptBlt[nCurrPixel].Red * nFadeLevel) / 255);
				ptTempBltBuffer[nCurrPixel].Green = (UINT8)((ptBlt[nCurrPixel].Green * nFadeLevel) / 255);
				ptTempBltBuffer[nCurrPixel].Blue = (UINT8)((ptBlt[nCurrPixel].Blue * nFadeLevel) / 255);
				nCurrPixel++;
			}
			DrawBlt(ptGraphicsOutput, ptTempBltBuffer, EfiBltBufferToVideo, ptRect);
			gBS->Stall(2000);
			nFadeLevel++;
		}
		
	}
	else
	{
		nFadeLevel = 255;
		while (nFadeLevel > 0)
		{
			nCurrPixel = 0;
			while (nCurrPixel < (nWidth * nHeight))
			{
				ptTempBltBuffer[nCurrPixel].Red = (UINT8)((ptBlt[nCurrPixel].Red * nFadeLevel) / 255);
				ptTempBltBuffer[nCurrPixel].Green = (UINT8)((ptBlt[nCurrPixel].Green * nFadeLevel) / 255);
				ptTempBltBuffer[nCurrPixel].Blue = (UINT8)((ptBlt[nCurrPixel].Blue * nFadeLevel) / 255);
				nCurrPixel++;
			}
			DrawBlt(ptGraphicsOutput, ptTempBltBuffer, EfiBltBufferToVideo, ptRect);
			gBS->Stall(2000);
			nFadeLevel--;
		}
	}
	FreePool(ptTempBltBuffer);
	return EFI_SUCCESS;
}

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
)
{
	UINT32 Width;
	UINT32 Height;
	ASSERT_ENSURE(ptGraphicsOutput != NULL || ptBlt != NULL || ptRect != NULL);
	Width = (UINT32)WidthRect(ptRect);
	Height = (UINT32)HeightRect(ptRect);
	INTN i;
	if (bIsCounterClockwise == FALSE)
	{
		for (i = (INTN)((Width - 1) >> 1); i < (INTN)(Width - 1 - 1); i++)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = 0; i < (INTN)(Height - 1 - 1); i++)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, Width - 1 - 1, i, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = (INTN)(Width - 1); i > 0; i--)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, i, Height - 1 - 1, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = (INTN)(Height - 1); i > 0; i--)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, 0, i, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = 0; i < (INTN)((Width - 1) >> 1); i++)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1);
	}
	else
	{
		for (i = (INTN)((Width - 1) >> 1); i > 0; i--)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = 0; i < (INTN)(Height - 1); i++)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, 0, i, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = 0; i < (INTN)(Width - 1); i++)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, i, Height - 1 - 1, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = (INTN)(Height - 1 - 1); i > 0; i--)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, Width - 1 - 1, i, (Width - 1) >> 1, (Height - 1) >> 1);
		for (i = (INTN)(Width - 1 - 1); i > (INTN)((Width - 1) >> 1); i--)
			GopEffects_BresenhamDrawLine(ptGraphicsOutput, ptBlt, ptRect->nLeft, ptRect->nTop, Width, Height, i, 0, (Width - 1) >> 1, (Height - 1) >> 1);

	}
		
}

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
)
{
	UINT32 nCurrRow;
	UINT32 nCurrRowToDrawTo;
	UINT32 nWidth;
	UINT32 nHeight;
	ASSERT_ENSURE(ptGraphicsOutput != NULL || ptBlt != NULL || ptRect != NULL);
	nWidth = WidthRect(ptRect);
	nHeight =HeightRect(ptRect);
	if (bIsBottomToTop == FALSE)
	{
		for (nCurrRow = 0; nCurrRow <= (nHeight - 1); nCurrRow++)
		{
			for (nCurrRowToDrawTo = (nHeight - 1); nCurrRowToDrawTo >= nCurrRow; nCurrRowToDrawTo--)
			{
				ASSERT_CHECK_EFISTATUS(ptGraphicsOutput->Blt(ptGraphicsOutput, &(ptBlt[nCurrRow*nWidth]), EfiBltBufferToVideo, 0, 0, ptRect->nLeft, ptRect->nTop + nCurrRowToDrawTo, nWidth, 1, 0));
			}
			gBS->Stall(2500); /* 2.5ms pause */
		}
	}
	else
	{
		for (nCurrRow = (nHeight - 1); nCurrRow >= 0; nCurrRow--) 
		{
			for (nCurrRowToDrawTo = 0; nCurrRowToDrawTo <= nCurrRow; nCurrRowToDrawTo++)
			{
				ASSERT_CHECK_EFISTATUS(ptGraphicsOutput->Blt(ptGraphicsOutput, &(ptBlt[nCurrRow*nWidth]), EfiBltBufferToVideo, 0, 0, ptRect->nLeft, ptRect->nTop + nCurrRowToDrawTo, nWidth, 1, 0));
			}
			gBS->Stall(2500); /* 2.5ms pause */
		}
	}
	return EFI_SUCCESS;
}