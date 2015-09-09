/*
 * chnotconnectedio.h
 *
 *  Created on: 05 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef CHNOTCONNECTEDIO_H_
#define CHNOTCONNECTEDIO_H_
#include "ichannel.h"

class ChNotConnectedIO : public IChannel
{
public:
	ChNotConnectedIO(CPattern* pattern, IOTYPES type, UINT num);
	virtual ~ChNotConnectedIO() {}

	virtual void InitRegisters() override
	{}
	virtual void UpdateDataToHW() override
	{}
	virtual void UpdateHWToData() override
	{}
};

#endif /* CHNOTCONNECTEDIO_H_ */
