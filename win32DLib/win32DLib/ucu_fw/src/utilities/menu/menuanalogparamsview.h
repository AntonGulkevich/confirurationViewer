/*
 * MenuAnalogParamsView.h
 *
 *  Created on: 11 сент. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MENUANALOGPARAMSVIEW_H_
#define MENUANALOGPARAMSVIEW_H_
#include "menuitem.h"


class IChannel;

class MenuAnalogParamsView : public MenuItem
{
private:
	enum STAGES
	{
		Header,
		Sign,
		Delta,
		Sync

	};

	IChannel* _channel;
	STAGES _stage;
	char _headerBuffer[5];

public:
	MenuAnalogParamsView(IChannel* channel, const char* name, const BYTE points = 0);
	virtual ~MenuAnalogParamsView() {}

	virtual const char* GetElementName() override;
	void Enter() override;
	bool Exit() override;

	bool IsActive() override { return _stage != STAGES::Header; }
};

#endif /* MENUANALOGPARAMSVIEW_H_ */
