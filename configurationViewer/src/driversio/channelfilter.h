
/*
 * ichannelfilter.h
 *
 *  Created on: 14 июня 2015 г.
 *      Author: Youbrin_A
 */

#ifndef ICHANNELFILTER_H_
#define ICHANNELFILTER_H_
#include "../types.h"

class IChannel;

class ChannelFilter
{
public:

	struct BESSCOEFF
	{

	    float b01;
	    float b11;
	    float b21;
	    float a11;
	    float a21;
	    float b02;
	    float b12;
	    float b22;
	    float a12;
	    float a22;
	};


private:


	IChannel* _channel;
	float* filterBuff; // Массив значений для фильтра
	UINT filterBuffLength; // Длина фильтра
	FILTR_TYPE filterType; // Тип фильтра
	UINT filterSubType; // Подтип для bessel параметры
	UINT valuesCount; // Количество значений в буфере

	float Bessel(float value);
	float Window(float value);

public:
	explicit ChannelFilter(IChannel* channel);
	virtual ~ChannelFilter() {}
	void CreateFilterRegisters() const;
	void CreateFilterBuffer();
	void ResetFiltr();
	void UpdateValue();
	// нормализация буфера для угловых параметров
	void NormalizeBuffer(float breakPoint) const;
	bool IsFilterReady() const	{ return valuesCount == filterBuffLength; }
};

#endif /* ICHANNELFILTER_H_ */
