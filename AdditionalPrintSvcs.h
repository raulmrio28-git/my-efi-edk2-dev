/* 
** ===========================================================================
** File: AdditionalPrintSvcs.h
** Description: UEFI print addons (center print, border print etc.)
** ===========================================================================
*/

#ifndef _ADDITIONALPRINTSVCS_H_
#define _ADDITIONALPRINTSVCS_H_

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

// PrintAlign() and PrintBorder() use these
enum {
	PR_LEFT = 0, //String is placed at the very beginning
	PR_RIGHT = 1, //String is placed after (length - buffer size) chars 
	PR_CENTER = 2 //String is placed after [(length - buffer size) /2] chars (center)
};
#define CMD_LINE_LINES 80
#define BORDER_CHARACTER '#'
#define DivApproxNext(a, b) (a/b)+((a%b)!=0)

/*
**----------------------------------------------------------------------------
**  Type Definitions
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
** Function: PrintBorder()
** Description: Prints a string in a border and in whatever orientation the
** user wants
** Input:
**		pszInChar: Input string
**		nAlignment: Alignment value
**		...: blablabla
** Output: Printed buffer in a border and aligned position
** Return value: Other -> Failure, 0 -> Success
** ===========================================================================
*/
EFI_STATUS
PrintBorder(
	IN CHAR16 *pszInChar,
	IN INTN nAlignment OPTIONAL,
	...
);

/*
** ===========================================================================
** Function: PrintBorder()
** Description: Prints a string in a border and in whatever orientation the
** user wants
** Input:
**		pszInChar: Input string
**		nAlignment: Alignment value
**		...: blablabla
** Output: Printed buffer in a border and aligned position
** Return value: Other -> Failure, 0 -> Success
** ===========================================================================
*/
EFI_STATUS
PrintBorder(
	IN CHAR16 *pszInChar,
	IN INTN nAlignment OPTIONAL,
	...
);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _ADDITIONALPRINTSVCS_H_ */
