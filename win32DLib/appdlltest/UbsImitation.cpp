#include "UbsImitation.h"
#include <tchar.h>

UbsImitation::UbsImitation(LPCWSTR libraryName)
{
	hInstDll =LoadLibrary(libraryName);
}

UbsImitation::~UbsImitation()
{
	FreeLibrary(hInstDll);
}
