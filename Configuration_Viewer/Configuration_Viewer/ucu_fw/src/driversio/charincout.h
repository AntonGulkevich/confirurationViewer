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
	ChArincWordOut* _wordId[1024];
	const Arinc::Rate _arincRates[4] = { Arinc::Rate::R12_5, Arinc::Rate::R12_5,  Arinc::Rate::R50,  Arinc::Rate::R100 };
	bool _isDataExits;

public:
	ChArincOut(CPattern* const pattern, UINT number);
	virtual ~ChArincOut() {}

	void AssignChannel(IOutputValue* ch) override	{ _channel = reinterpret_cast<ArincOut*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;

	bool IsLabelExist(UINT label) const	{ return GetLabel(label) != nullptr; }
	void AddLabel(BYTE label);
	void LabelToId(BYTE label, BYTE id);
	ChArincWordOut* GetLabel(UINT label) const	{ return  ((label & 0x400) == 0x400) ? _wordId[label&0x3FF] : _word[label & 0xFF]; }
	bool IsDataExists() const
	{ return _isDataExits; }
};

#endif /* CHARINCOUT_H_ */
