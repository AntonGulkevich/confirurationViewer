/*
 * menuchannelaction.h
 *
 *  Created on: 07 сент. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MENUCHANNELACTION_H_
#define MENUCHANNELACTION_H_
#include "menuitem.h"

class IChannel;

class MenuChannelAction : public MenuItem
{
public:
	enum class ACTIONS
	{
		ShowValue,
		ShowLinearValue,
		ResetTarirovka,
	};

private:
	IChannel* _channel;
	ACTIONS _action;

	char _valueBuff[5];
	bool _isAcception;
	bool _isFlashing;
	int _flashingState;

	void ValueToStr(float value);

public:

	MenuChannelAction(IChannel* channel, ACTIONS action, const char* name, const BYTE points = 0);
	virtual ~MenuChannelAction() {}

	virtual const char* GetElementName() override;
	void Enter() override;
	bool Exit() override;
	void LongEnter() override;
};

#endif /* MENUCHANNELACTION_H_ */
