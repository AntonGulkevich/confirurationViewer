/*
 * chanalogin.h
 *
 *  Created on: 01 ט‏םÿ 2015 ד.
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
		SCT_8, // ׁÊׂ 8ֲ
		SCT_20, // ׁÊׂ 20ֲ
		SCT_36, // ׁÊׂ 36ֲ
		Selsin, // ׁוכüסטם 36ֲ-60ֲ
		Logometr, // ֻמדמלוענ 27ֲ
		NPT, // ֽֿׂ
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
