/*
 * charincin.h
 *
 *  Created on: 03 июня 2015 г.
 *      Author: Youbrin_A
 */

#ifndef CHARINCIN_H_
#define CHARINCIN_H_

#include "ichannelin.h"

class ArincIn;
class ChArincWordIn;

class ChArincIn : public IChannelIn
{
private:
	ArincIn* _channel;
	ChArincWordIn* _word[256];
	bool _isDataExist; // Есть сконфигурированные слова

public:
	ChArincIn(CPattern* const pattern, UINT number);
	virtual ~ChArincIn() {}

	void AssignChannel(IInputValue* ch) override	{ _channel = reinterpret_cast<ArincIn*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;

	bool IsLabelExist(BYTE label) const	{ return _word[label] != nullptr; }
	void AddLabel(BYTE label);
	ChArincWordIn* GetLabel(BYTE label) const	{ return _word[label]; }
	bool IsDataExists() const	{ return _isDataExist; }
};

#endif /* CHARINCIN_H_ */
