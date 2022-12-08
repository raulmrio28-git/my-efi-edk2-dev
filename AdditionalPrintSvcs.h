/*===================================================
	UEFI Environment
	Print services for additional addons to Print()
	1.0
===================================================*/
/***
  PrintCenter()

  Prints a string in a centered position.

  Example of usage:
  PrintCenter(L"Hi! I'm a centered string and it is printed centered thanks to my PrintCenter() function!\n");
  Returns:
  'Hi! I'm a centered string and it is printed centered thanks to my PrintCenter()'
  '                                   function!                                   '

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
UINTN
PrintCenter(
	IN CONST CHAR16 *inChar,
	...
);