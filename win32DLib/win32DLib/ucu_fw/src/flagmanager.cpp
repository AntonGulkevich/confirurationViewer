/*
 * manager.cpp
 *
 *  Created on: 26 џэт. 2015 у.
 *      Author: Youbrin_A
 */

#include "flagmanager.h"

#include <string.h>

#include "drivers/keys.h"
#include "drivers/perfomancecounter.h"
#include "interfaces/idispatchflags.h"
#include "interfaces/iusingflags.h"
#include "utilities/console.h"

DISPATH_RESULT FlagManager::Dispatcher::Process(Flags id)
{
	if (flagState == drNotDispatched)
		return flagState = Object->ProcessFlag(id);
	else
		return drAlreadyDispatched;
}

FlagManager::FlagManager(PerfomanceCounter* pc) : _pc(pc)
{
	IDispatchFlags::SetManager(this);
	IUsingFlags::SetManager(this);
	memset(flags, 0, sizeof(flags));

}

FlagManager::~FlagManager()
{

}

void FlagManager::Process()
{
	for (auto p = 0; p < static_cast<int>(PRIORITY::Count); p++)
		for (auto f = 0; f < static_cast<int>(Flags::Count); f++)
			if (flags[p][f])
			{
				_pc->ProcessStart();
				for (UINT i = 0; i < dispatchers[p][f].size(); i++)
				{
					if (dispatchers[p][f][i]->Process(static_cast<Flags>(f)) == drDispatched)
					{
						_pc->ProcessStop();
						return;
					}
				}
				ResetFlag(static_cast<Flags>(f), static_cast<PRIORITY>(p));
				_pc->ProcessStop();
			}
}


void FlagManager::RegFlag(Flags id, PRIORITY priority, IDispatchFlags* obj)
{
	dispatchers[static_cast<int>(priority)][static_cast<int>(id)].push_back(new Dispatcher(obj));
}

