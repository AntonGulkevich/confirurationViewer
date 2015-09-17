#ifndef UBS_IMITATION_H
#define UBS_IMITATION_H

#include <windows.h>

class UbsImitation
{
	HINSTANCE hInstDll;

public:
	explicit UbsImitation(LPCWSTR libraryName);
	virtual ~UbsImitation();
};

#endif
