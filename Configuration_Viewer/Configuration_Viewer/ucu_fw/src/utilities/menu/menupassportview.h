/*
 * menupassportview.h
 *
 *  Created on: 11 сент. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MENUPASSPORTVIEW_H_
#define MENUPASSPORTVIEW_H_
#include "menuitem.h"


class IChannel;

class MenuPassportView : public MenuItem
{
private:
	enum STAGES
	{
		Header,
		Start,
		ViewHeader,
		ViewVal

	};

	IChannel* _channel;
	STAGES _stage;
	int _pointNumber;
	char _headerBuffer[5];

public:
	MenuPassportView(IChannel* channel, const char* name, const BYTE points = 0);
	virtual ~MenuPassportView() {}

	virtual const char* GetElementName() override;
	void Enter() override;
	bool Exit() override;
	void Prev() override;
	void Next() override;

	bool IsActive() override { return _stage != STAGES::Header; }
};

#endif /* MENUPASSPORTVIEW_H_ */
