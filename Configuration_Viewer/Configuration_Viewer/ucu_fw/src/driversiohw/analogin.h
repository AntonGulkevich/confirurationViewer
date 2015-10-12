/*
 * analogin.h
 *
 *  Created on: 30 марта 2015 г.
 *      Author: Youbrin_A
 */

#ifndef ANALOGIN_H_
#define ANALOGIN_H_
#include <cmath>

#include "../types.h"
#include "../interfaces/iinputvalue.h"
#include "../utilities/m_math.h"


class AnalogIn : public IInputValue
{

public:

	enum class GAIN_COEFFICIENTS
	{
		K_x01,
		K_x02,
		K_x05,
		K_x1,
		Count
	};

	enum class TYPE
	{
		Disabled,
		SCT,
		Logometr,
		Selsin,
		NPT,
		ControlNPT,
		ControlSCT,
		ControlSelsin,
		Calibration,
		Count
	};

	enum class ANGLE_TYPE
	{
		NoAngleType,
		SCT8,
		SCT20,
		SCT36,
		ControlSCT,
		Logometr,
		Selsin,
		ControlSelsin,
		Count,
	};

	static const UINT maxScaleADC = (1 << 15)-1;
	static const UINT delayStep = 40;
	static const float SCTControlMaxVoltage;
	static const UINT breakBuffLengthLog = 6;
	static const UINT breakBuffLength = 1 << breakBuffLengthLog;

protected:


	struct ANGLE_CONST
	{
		float MinLevelV;
		GAIN_COEFFICIENTS GainCoeff;
		TYPE Type;
		float MaxScaleV;
	};


	// Адреса параметров в ПЛИС
	const UINT SYNC_OFFSET = 0;
	const UINT DELAY_OFFSET = 0x100;
	const UINT DELAYINVERSE_OFFSET = 0xE00;
	const UINT GAIN_OFFSET = 0x200;
	const UINT TYPE_OFFSET = 0x300;
	const UINT MIN_LEVEL_OFFSET = 0x400;
	const UINT NO_SIGNAL_OFFSET = 0x500;
	const UINT VALUE_OFFSET = 0x600;
	const UINT REG1_OFFSET = 0x700;
	const UINT REG2_OFFSET = 0x800;
	const UINT REG3_OFFSET = 0x900;

	const UINT FAIL_LOW_LEVEL_OFFSET = 0xB00;
	const UINT FAIL_NO_SIGNAL_OFFSET = 0xC00;
	const UINT FAIL_OVERSCALE_OFFSET = 0xD00;
	const UINT MEASURE_COUNT_OFFSET = 0xF00;





	// Константы усилений
	const float maxScaleV[static_cast<UINT>(GAIN_COEFFICIENTS::Count)] = { 100.0f, 50.0f, 20.0f, 10.0f };
	// Константы для угловых датчиков
	const ANGLE_CONST angleConsts[static_cast<UINT>(ANGLE_TYPE::Count)] =
	{
			{0, GAIN_COEFFICIENTS::K_x01, TYPE::Disabled, 1.0f },
			{ 6, GAIN_COEFFICIENTS::K_x05, TYPE::SCT, 20.0f },
			{ 13, GAIN_COEFFICIENTS::K_x02, TYPE::SCT, 50.0f },
			{ 20, GAIN_COEFFICIENTS::K_x01, TYPE::SCT, 100.0f },
			{ 6, GAIN_COEFFICIENTS::K_x05, TYPE::ControlSCT, SCTControlMaxVoltage },
			{ 7, GAIN_COEFFICIENTS::K_x02, TYPE::Logometr, 50.0f },
			{ 30, GAIN_COEFFICIENTS::K_x01, TYPE::Selsin,100.0f },
			{ 6, GAIN_COEFFICIENTS::K_x05, TYPE::ControlSelsin, SCTControlMaxVoltage },
	};


	DWORD _baseAddress;
	DWORD _inputOffset;

	WORD reg1;
	WORD reg2;
	WORD reg3;

	ANGLE_TYPE _angleType = ANGLE_TYPE::NoAngleType;
	GAIN_COEFFICIENTS _gainKoeff;

	// Контроль обрывов и уход точности
	short _reg1Buffer[breakBuffLength];
	short _reg2Buffer[breakBuffLength];
	short _reg3Buffer[breakBuffLength];
	float _valBuffer[breakBuffLength];
	float _breakBuffer[breakBuffLength];
	UINT _breakBufferCount;

public:

	AnalogIn(DWORD baseAddress, DWORD inputOffset);
	virtual ~AnalogIn();
	// Установка номера синхронизации
	void SetSyncNumber(UINT sync) { UCU_IOWR_32DIRECT(_baseAddress, SYNC_OFFSET + _inputOffset, sync); }
	// Установка задержки измерения в мкс
	void SetDelay(UINT delay) { UCU_IOWR_32DIRECT(_baseAddress, DELAY_OFFSET + _inputOffset, delay / delayStep);
								UCU_IOWR_32DIRECT(_baseAddress, DELAYINVERSE_OFFSET + _inputOffset, delay / delayStep); }
	// Установка коэффициента усиления
	void SetGainCoefficient(GAIN_COEFFICIENTS coeff);
	// Установка коэффициента усиления
	void SetType(TYPE type) { UCU_IOWR_32DIRECT(_baseAddress, TYPE_OFFSET + _inputOffset, static_cast<UINT>(type)); }
	// Установка порога низкого уровня сигнала (доля от максимальной шкалы 0..1)
	// Устанавливается для угловых типов, вызывать после установки типа
	void SetMinlevel(float voltage)
	{
		UINT minLevel = roundf(SQR(voltage *  maxScaleADC  / angleConsts[static_cast<UINT>(_angleType)].MaxScaleV));
		UCU_IOWR_32DIRECT(_baseAddress, MIN_LEVEL_OFFSET + _inputOffset, minLevel);
	}
	// Установка коэффициента усиления
	void SetAngleType(ANGLE_TYPE type);

	TYPE GetType() const	{ return static_cast<TYPE>(UCU_IORD_32DIRECT(_baseAddress, TYPE_OFFSET + _inputOffset)); }
	ANGLE_TYPE GetAngleType() const	{ return _angleType; }
	UINT GetDelay() const	{ return UCU_IORD_32DIRECT(_baseAddress, DELAY_OFFSET + _inputOffset) * delayStep; }
	UINT GetSyncNum() const	{ return  UCU_IORD_32DIRECT(_baseAddress, SYNC_OFFSET + _inputOffset); }
	GAIN_COEFFICIENTS GetGainKoefficient() const	{ return  static_cast<GAIN_COEFFICIENTS>(UCU_IORD_32DIRECT(_baseAddress, GAIN_OFFSET + _inputOffset)); }


	virtual void UpdateValue() override;

	WORD GetRegister1() const	{ return reg1; }
	WORD GetRegister2() const	{ return reg2; }
	WORD GetRegister3() const	{ return reg3; }

	BYTE GetMeasureCount() const	{ return UCU_IORD_32DIRECT(_baseAddress, MEASURE_COUNT_OFFSET + _inputOffset); }

	// Контроль обрывов
	bool IsBreakBufferReady() { return _breakBufferCount == breakBuffLength; }
	void CalcBreak();


};

#endif /* ANALOGIN_H_ */
