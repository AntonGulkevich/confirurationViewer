
/*
 * ichannelrate.h
 *
 *  Created on: 14 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef ICHANNELRATE_H_
#define ICHANNELRATE_H_
#include "../types.h"

class IChannel;

class ChannelRate
{

private:
	IChannel* _channel;
	float* _rateBuff; // ������ �������� ��� �������
	UINT _rateBuffLength; // ����� �������
	UINT _valuesCount; // ���������� �������� � ������
	const float _secondBase = 20.0f / 1000.0f; // ������ ������� � ��������

	float LineOls();

public:
	explicit ChannelRate(IChannel* channel);
	virtual ~ChannelRate() {}
	void CreateRateRegisters();
	void CreateRateBuffer();
	void ResetRate();
	void UpdateValue();
	bool IsRateReady() const
	{ return _valuesCount == _rateBuffLength; }

};

#endif /* ICHANNELRATE_H_ */
