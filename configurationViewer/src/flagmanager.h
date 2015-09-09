/*
 * manager.h
 *
 *  Created on: 26 янв. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MANAGER_H_
#define MANAGER_H_
#include "types.h"
#include <vector>

class IDispatchFlags;
class PerfomanceCounter;

class FlagManager
{
private:
	class Dispatcher
	{
	public:
		DISPATH_RESULT flagState;
		IDispatchFlags* Object;

		explicit Dispatcher(IDispatchFlags* obj):
			flagState(drNotDispatched), Object(obj) {}

		DISPATH_RESULT Process(Flags id);
		void ResetState() { flagState = drNotDispatched; }
	};

	bool flags[static_cast<int>(PRIORITY::Count)][static_cast<int>(Flags::Count)];
	void ResetFlag(const Flags id, PRIORITY priority) { flags[static_cast<int>(priority)][static_cast<int>(id)] = false; for (UINT i = 0; i < dispatchers[static_cast<int>(priority)][static_cast<int>(id)].size(); i++) dispatchers[static_cast<int>(priority)][static_cast<int>(id)][i]->ResetState(); }

	std::vector<Dispatcher*> dispatchers[static_cast<int>(PRIORITY::Count)][static_cast<int>(Flags::Count)];
	PerfomanceCounter* _pc;

public:
	explicit FlagManager(PerfomanceCounter* pc);
	virtual ~FlagManager();

	void SetFlag(Flags id) { for(auto i = 0; i < static_cast<int>(PRIORITY::Count); i++) flags[i][static_cast<int>(id)] = true; }
	// Для отладки, в работе не используется
	bool GetFlag(Flags id) 	{ return flags[0][static_cast<int>(id)] || flags[1][static_cast<int>(id)] || flags[2][static_cast<int>(id)]; }
	void RegFlag(Flags id, PRIORITY priority, IDispatchFlags* obj);

	void Process();
};

#endif /* MANAGER_H_ */
