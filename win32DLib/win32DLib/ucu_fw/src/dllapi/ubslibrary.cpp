#ifdef EMULATION
#include "UbsLibrary.h"
// Factory function that creates instances if the UBS_ object.
EXTERN_C UBS_API  UBS_HANDLE returnFactory()
{
	DllWrapperFactory * pObj = static_cast<DllWrapperFactory*>(&FactorySingleton::Instance());
	return pObj;
}

EXTERN_C UBS_API VOID writeSharedMem(LPBYTE lpbBuf, DWORD address, DWORD size)
{
	LPVOID lpvMem = MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);
	auto lpbTemp = static_cast<LPBYTE>(lpvMem);
	*lpbTemp += address;
	while (size--)
		*lpbTemp++ = *lpbBuf++;
	UnmapViewOfFile(lpvMem);
}

EXTERN_C UBS_API VOID readSharedMem(LPBYTE lpbBuf, DWORD address, DWORD size)
{
	LPVOID lpvMem = MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);
	auto lpbTemp = static_cast<LPBYTE>(lpvMem);
	*lpbTemp += address;
	memcpy(lpbBuf, lpbTemp, size*sizeof(lpbBuf));
	UnmapViewOfFile(lpvMem);
}

BOOL createFileMap()
{	
	BOOL fInit;
	hMapObject = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, UBS_MEM_SIZE, TEXT("dllmemfilemap")); 
	if (hMapObject == nullptr)
		return FALSE;
	////fInit = (GetLastError() != ERROR_ALREADY_EXISTS);
	//lpvMem = MapViewOfFile(hMapObject, FILE_MAP_WRITE, 0, 0, 0);  
	////std::cout <<"MapViewOfFile: "<< reinterpret_cast<int>(lpvMem) << std::endl;
	//if (lpvMem == nullptr)
	//	return FALSE;
	////at the firts time initialize mapped file and fill it with 0(zero)
	///*if (fInit)
	//	memset(lpvMem, 0, 0xffffffff);*/
	//return TRUE;
}

BOOL detProcess()
{	
	return CloseHandle(hMapObject);
}
#endif