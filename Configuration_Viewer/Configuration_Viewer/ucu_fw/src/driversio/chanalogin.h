/*
 * chanalogin.h
 *
 *  Created on: 01 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef CHANALOGIN_H_
#define CHANALOGIN_H_


#include "channelcalibration.h"
#include "ichannelin.h"

class AnalogIn;

class ChAnalogIn : public IChannelIn
{
private:
	AnalogIn* _channel;


public:
	enum class ANALOG_TYPE
	{
		SCT_8, // ��� 8�
		SCT_20, // ��� 20�
		SCT_36, // ��� 36�
		Selsin, // ������� 36�-60�
		Logometr, // �������� 27�
		NPT, // ���
	};

	ChAnalogIn(CPattern* const pattern, UINT number);

	virtual ~ChAnalogIn();

	void AssignChannel(IInputValue* ch) override	{ _channel = reinterpret_cast<AnalogIn*>(ch); }

	virtual void InitRegisters() override;
	virtual void UpdateDataToHW() override;
	virtual void UpdateHWToData() override;


	CheckConfigurationResult CheckConfiguration() override;
};

#endif /* CHANALOGIN_H_ */
