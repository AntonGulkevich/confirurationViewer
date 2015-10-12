/*
 * ichannelcalibration.h
 *
 *  Created on: 14 ���� 2015 �.
 *      Author: Youbrin_A
 */

#ifndef ICHANNELCALIBRATION_H_
#define ICHANNELCALIBRATION_H_
#include "../interfaces/iusingflags.h"
#include "../types.h"
#include "ichannel.h"


class ChannelCalibration : public IUsingFlags
{
private:
	float* _userPassport; // ���������������� �������� ��������
	UINT _userPassportCount; // ����� ������� (���������� ���)

	IChannel* _channel;
	float** _koeffTable; // ������������ ��� LINEAR
	UINT _koeffCount; // ����� ������� (���������� ���)
	bool _isCalibrated;
	bool _isDeltaHRequired;

	// ��������� ��� ������ ��������� ????
	const UINT _measuringLength = 32;
	bool _isMeasuring;
	float _measuredValue;
	UINT _currentMeasuringIndex;

	float** _tarKoeffTable; // ������������ ��� ���������� ����������� �������� ��� ���������
	UINT _tarKoeffCount; // ����� ������� (���������� ���)

	void CreateKoeffTable(float**& kTable, UINT& kCount, const float* pasp, const UINT paspCount);

public:

	explicit ChannelCalibration(IChannel* channel, bool isDeltaHExist = false);
	virtual ~ChannelCalibration() {}
	float GetValueByKoeff(float deg) const;

	void CreateClibrationRegisters() const;
	// ���������� ������� ��� Linear �� �������� PASSPORT ��� ���������
	void CreateKoeffTable();
	float GetPoint(UINT index);
	UINT GetPointsCount();
	// �������� �������
	bool IsCalibrated() { return _isCalibrated; }
	// ��������� ��������� DeltaH
	bool IsDeltaHRequired();

	// ��������� ���������
	float GetTarMin() const;
	float GetTarMax() const;
	// ���������
	void StartMeasure();
	void UpdateValue();
	bool IsMeasuring() { return _isMeasuring; }
	float GetMeasuredValue() { return _measuredValue / _currentMeasuringIndex; }
	UINT GetCurrentMeasurionIndex() { return _currentMeasuringIndex; }
	float GetTarPaspValue(float value);

	void SetUserPassport(const float* userPasp, UINT count);
	IChannel::CheckConfigurationResult CheckConfiguration();
};

#endif /* ICHANNELCALIBRATION_H_ */
