/*
 * sktout.h
 *
 *  Created on: 13 апр. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef SKTOUT_H_
#define SKTOUT_H_

#include "../types.h"
#include "../interfaces/ioutputvalue.h"

class AnalogIn;

class SCTOut : public IOutputValue
{

public:
	enum class SCTType
	{
		SCT,
		ZSelsin
	};


	struct Calibrations
	{
		float scaleSin;
		int offsetSin;
		float scaleCos;
		int offsetCos;
	};

private:

	static const DWORD maxRefLevel;
	static const DWORD minRefLevel;

	DWORD _baseAddress;
	DWORD _outputOffset;
	AnalogIn* _control;

	const DWORD typeOffset = 0x0;
	const DWORD phaseShift = 0x100;
	const DWORD maxDeviationOffset = 0x240;
	const DWORD valueOffset = 0x300;
	const DWORD register1Offset = 0x400;
	const DWORD register2Offset = 0x440;
	const DWORD failRefOffset = 0x340;
	const DWORD refOverloadOffset = 0x380;
	const DWORD failOffset = 0x40;
	const DWORD overloadOffset = 0x80;
	const DWORD minRefLevelOffset = 0x280;
	const DWORD maxRefLevelOffset = 0x2C0;

	void SetMinRefLevel(DWORD level) { UCU_IOWR_32DIRECT(_baseAddress, _outputOffset + minRefLevelOffset, level); }
	void SetMaxRefLevel(DWORD level) { UCU_IOWR_32DIRECT(_baseAddress, _outputOffset + maxRefLevelOffset, level); }
	void SetMaxDeviation(float val) { UCU_IOWR_32DIRECTF(_baseAddress, _outputOffset + maxDeviationOffset, val); }

public:
	void SetType(SCTType type);
	void SetPhaseShift(float phase) { UCU_IOWR_32DIRECTF(_baseAddress, _outputOffset + phaseShift, phase); }

	void UpdateValue() override;


	SCTOut(DWORD baseAddress, DWORD outputOffset, AnalogIn* control);
	virtual ~SCTOut();

	// Функции для диагностики
	float GetPLDValue() const	{ return UCU_IORD_32DIRECTF(_baseAddress, _outputOffset + valueOffset); }
	WORD GetRegisterSin() const	{ return UCU_IORD_32DIRECT(_baseAddress, _outputOffset + register1Offset); }
	WORD GetRegisterCos() const	{ return UCU_IORD_32DIRECT(_baseAddress, _outputOffset + register2Offset); }
	float GetControlValue() const	{ return UCU_IORD_32DIRECTF(_baseAddress, _outputOffset + 0x5C0); }
	float GetPrevValue() const { return UCU_IORD_32DIRECTF(_baseAddress, _outputOffset + 0x600); }
	DWORD GetSumSq()const { return UCU_IORD_32DIRECT(_baseAddress, _outputOffset + 0x640); }
	DWORD GetControlReg1()const { return UCU_IORD_32DIRECT(_baseAddress, _outputOffset + 0x680); }
	DWORD GetControlReg2() const { return UCU_IORD_32DIRECT(_baseAddress, _outputOffset + 0x6C0); }
	SCTType GetType()const { return static_cast<SCTType>(UCU_IORD_32DIRECT(_baseAddress, _outputOffset + typeOffset)); }
	float GetPhaseShift() const { return UCU_IORD_32DIRECTF(_baseAddress, _outputOffset + phaseShift); }

	// Функции калибровки
	void SetCalibration(Calibrations& calib);
	void PrepareCalibrationSin0();
	void PrepareCalibrationCos0();
	void PrepareCalibrationScale();
	void ToNormalWork();


};

#endif /* SKTOUT_H_ */
