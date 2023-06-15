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