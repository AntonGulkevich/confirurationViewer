#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: Установите здесь ссылки на дополнительные заголовки, требующиеся для программы
#include <windows.h>
#include <iostream>
#include <memory.h>
#include "../win32DLib/ucu_fw/src/dllapi/factory.h"
