/*
 * ichannelcalibration.h
 *
 *  Created on: 14 июня 2015 г.
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
	float* _userPassport; // Пользовательское значение паспорта
	UINT _userPassportCount; // Длина таблицы (количество пар)

	IChannel* _channel;
	float** _koeffTable; // Коэффициенты для LINEAR
	UINT _koeffCount; // Длина таблицы (количество пар)
	bool _isCalibrated;
	bool _isDeltaHRequired;

	// Параметры для ручной тарировки ????
	const UINT _measuringLength = 32;
	bool _isMeasuring;
	float _measuredValue;
	UINT _currentMeasuringIndex;

	float** _tarKoeffTable; // Коэффициенты для вычисления паспортного значения при тарировке
	UINT _tarKoeffCount; // Длина таблицы (количество пар)

	void CreateKoeffTable(float**& kTable, UINT& kCount, const float* pasp, const UINT paspCount);

public:

	explicit ChannelCalibration(IChannel* channel, bool isDeltaHExist = false);
	virtual ~ChannelCalibration() {}
	float GetValueByKoeff(float deg) const;

	void CreateClibrationRegisters() const;
	// Построение таблицы для Linear из регистра PASSPORT или тарировки
	void CreateKoeffTable();
	float GetPoint(UINT index);
	UINT GetPointsCount();
	// Настроен вручную
	bool IsCalibrated() { return _isCalibrated; }
	// Требуется настройка DeltaH
	bool IsDeltaHRequired();

	// Параметры тарировки
	float GetTarMin() const;
	float GetTarMax() const;
	// Тарировка
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
