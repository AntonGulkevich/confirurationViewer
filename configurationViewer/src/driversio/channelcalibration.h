/*
 * ichannelcalibration.h
 *
 *  Created on: 14 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef ICHANNELCALIBRATION_H_
#define ICHANNELCALIBRATION_H_
#include "../types.h"

class IChannel;

class ChannelCalibration
{
private:
	IChannel* _channel;
	float** koeffTable; // ������������ ��� LINEAR
	BYTE koeffCount; // ����� ������� (���������� ���)

	// ��������� ��� ������ ��������� ????

public:
	explicit ChannelCalibration(IChannel* channel);
	virtual ~ChannelCalibration() {}
	float GetValueByKoeff(float deg) const;

	void CreateClibrationRegisters() const;
	// ���������� ������� ��� Linear �� �������� PASSPORT ��� ���������
	void CreateKoeffTable();
};

#endif /* ICHANNELCALIBRATION_H_ */
