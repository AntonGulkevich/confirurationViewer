/*
 * charincout.h
 *
 *  Created on: 01 θώνÿ 2015 γ.
 *      Author: Youbrin_A
 */

#ifndef CHARINCOUT_H_
#define CHARINCOUT_H_
#include "ichannelout.h"
#include "../driversiohw/arinctypes.h"

class ArincOut;
class ChArincWordOut;

class ChArincOut : public IChannelOut
{
private:
	ArincOut* _channel;
	ChArincWordOut* _word[256];
	const Arinc::Rate _arincRates[4] = { Arinc::Rate::R12_5, Arinc::Rate::R12_5,  Arinc::Rate::R50,  Arinc::Rate::R100 };
	bool _isDataExits;

public:
	ChArincOut(CPattern* const pattern, UINT number);
	virtual ~ChArincOut() {}

	void AssignChannel(IOutputValue* ch) override	{ _channel = reinterpret_cast<ArincOut*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;

	bool IsLabelExist(BYTE label) const	{ return _word[label] != nullptr; }
	void AddLabel(BYTE label);
	ChArincWordOut* GetLabel(BYTE label) const	{ return _word[label]; }
	bool IsDataExists() const
	{ return _isDataExits; }
};

#endif /* CHARINCOUT_H_ */
