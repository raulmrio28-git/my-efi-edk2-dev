/* 
** ===========================================================================
** File: UefiDebug.h
** Description: UEFI debug addon
** ===========================================================================
*/

#ifndef _UEFIDEBUG_H_
#define _UEFIDEBUG_H_

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

// DebugPrtPrint() uses these
enum
{
	ASSERTION_ASSERT, //Assert 
	ASSERTION_REQUIRE, //Requie that...
	ASSERTION_ENSURE, //Ensure that...
	ASSERTION_CHECK, //Check if...
	ASSERTION_NEVER_GET_HERE, //Action cannot complete
	ASSERTION_NOT_IMPLEMENTED_YET //Not worked on!
};
#define DBGMSG_MAX_CHARS 256
#ifndef EXIT_FAILURE
#define EXIT_FAILURE -1
#endif
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
** Function: DebugPrtPrint()
** Description: Prints assertion debug info to COM port for debug purposes
** Input:
**		nAssertLevel: assert level
**		pszAssertType: #nAssertLevel
**		pszExpression: input expression
**		pszFile: fileanme
**		nLine: code line
** Output: Assert info in COM port
** Return value: Other -> Failure, 0 -> Success
** ===========================================================================
*/
EFI_STATUS
DebugPrtPrint(
	IN INTN nAssertLevel,
	IN CHAR8 *pszAssertType,
	IN CHAR8 *pszFunction,
	IN CHAR8 *pszExpression,
	IN CHAR8 *pszFile,
	IN INTN nLine
);

/*
**----------------------------------------------------------------------------
**  Definitions (macros for assertion)
**----------------------------------------------------------------------------
*/

#define ASSERT_DEBUG(type, expression) DebugPrtPrint(type, #type, __func__, #expression, __FILE__, __LINE__)

#define ASSERT_ASSERT(expr)    \
	ASSERT_DEBUG(ASSERTION_ASSERT, expr);\
	if (!(expr)) { return EXIT_FAILURE; }
#define ASSERT_REQUIRE(expr)    \
	ASSERT_DEBUG(ASSERTION_REQUIRE, expr);\
	if (!(expr)) { return EXIT_FAILURE; }
#define ASSERT_ENSURE(expr)    \
	ASSERT_DEBUG(ASSERTION_ENSURE, expr);\
	if (!(expr)) { return EXIT_FAILURE; }
#define ASSERT_CHECK(expr)        \
	ASSERT_DEBUG(ASSERTION_CHECK, expr);\
	if (!(expr)) { return EXIT_FAILURE; }
#define ASSERT_NEVER_GET_HERE()    ASSERT_DEBUG(ASSERTION_NEVER_GET_HERE, NULL);
#define ASSERT_NOT_IMPLEMENTED_YET()    ASSERT_DEBUG(ASSERTION_NOT_IMPLEMENTED_YET, NULL);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _UEFIDEBUG_H_ */
