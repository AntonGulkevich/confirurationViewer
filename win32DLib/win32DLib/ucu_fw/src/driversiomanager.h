/*
 * driversiomanager.h
 *
 *  Created on: 10 апр. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef DRIVERSIOMANAGER_H_
#define DRIVERSIOMANAGER_H_
#include <cwctype>

#include "interfaces/ioutputvalue.h"
#include <vector>

#include "driversiohw/adccontrol.h"



#include "driversiohw/rsin.h"
#include "driversiohw/rsout.h"
#include "driversiohw/sctout.h"
#include "driversiohw/nptout.h"

#include "interfaces/idispatchflags.h"
#include "interfaces/iusingflags.h"
#include "interfaces/iusingirq.h"


#include "types.h"
#include "dllapi/idriversiomanager.h"


class AnalogIn;
class Potentiometr;
class IInputValue;
class ADCControl;
class SCTOut;
class NPTOut;
class DescreeteInput;
class DescreeteOutput;
class ArincIn;
class ArincOut;
class ArincWordOut;
class ArincWordIn;
class NptControl;
class Calibrate;

class DriversIOManager : public IDriversIOManager, public IDispatchFlags, public IUsingIrq, public IUsingFlags
{

public:
	const static UINT analogInCount = 13;
	const static UINT potentiometrCount = 6;
	const static UINT descreeteInputCount = 29;
	const static UINT arincInCount = 6;
	const static UINT rsInCount = 2;

	const static UINT SCTOutCount = 6;
	const static UINT NPTOutCount = 3;
	const static UINT descreeteOutputCount = 8;
	const static UINT arincOutCount = 4;
	const static UINT rsOutCount = 2;

	struct Calibration
	{
		DWORD version;
		DWORD blockNumber;
		ADCControl::Calibrations analogIn;
		NPTOut::Calibrations NPT[NPTOutCount];
		SCTOut::Calibrations SCT[SCTOutCount];
		DWORD datetime;
		DWORD cs;
	};

private:
	ADCControl* _adcControl;

	AnalogIn* _analogIns[analogInCount];
	AnalogIn* _SCTControls[SCTOutCount];
	NptControl* _NPTControls[NPTOutCount];
	Potentiometr* _potentiometrs[potentiometrCount];
	DescreeteInput* _descreeteInputs[descreeteInputCount];
	ArincIn* _arincIns[arincInCount];
	RSIn* _rsIns[rsInCount];


	SCTOut* _SCTOuts[SCTOutCount];
	NPTOut* _NPTOuts[NPTOutCount];
	DescreeteOutput* _descreeteOutputs[descreeteOutputCount];
	ArincOut* _arincOuts[arincOutCount];
	RSOut* _rsOuts[rsOutCount];

	Calibrate* _calibrate;

	std::vector<ArincWordOut*> _arincWordsOut;
	std::vector<ArincWordIn*> _arincWordsIn;

	UINT _timerCount;


	std::vector<IInputValue*> _inputs;
	std::vector<IOutputValue*> _outputs;

    IO_GROUP_STATE _state;

    // Контроль наличия прерываний
    UINT _prevTimerCount;
    UINT _irqCount;

    void UpdateGloabalState();

public:
	DriversIOManager();
	virtual ~DriversIOManager();

	ADCControl* GetADCControl() const override	{ return _adcControl; }
	Calibrate* GetCalibrate() const override { return _calibrate; }
	AnalogIn* GetAnalogIn(UINT index)override { return index < analogInCount ? _analogIns[index] : nullptr; }
	AnalogIn* GetSCTControl(UINT index) override { return index < SCTOutCount ? _SCTControls[index] : nullptr; }
	NptControl* GetNPTControl(UINT index)override { return index < NPTOutCount ? _NPTControls[index] : nullptr; }
	Potentiometr* GetPotentiometr(UINT index) override { return index < potentiometrCount ? _potentiometrs[index] : nullptr; }
	DescreeteInput* GetDescreeteIn(UINT index) override { return index < descreeteInputCount ? _descreeteInputs[index] : nullptr; }
	ArincIn* GetArincIn(UINT index)override { return index < arincInCount ? _arincIns[index] : nullptr; }
	RSIn* GetRSIn(UINT index) override { return index < rsInCount ? _rsIns[index] : nullptr;  }
	SCTOut* GetSCTOut(UINT index) override { return index < SCTOutCount ? _SCTOuts[index] : nullptr; }
	NPTOut* GetNPTOut(UINT index) override { return index < NPTOutCount ? _NPTOuts[index] : nullptr; }
	DescreeteOutput* GetDescreeteOut(UINT index) override { return index < descreeteOutputCount ? _descreeteOutputs[index] : nullptr; }
	ArincOut* GetArincOut(UINT index) override { return index < arincOutCount ? _arincOuts[index] : nullptr; }
	RSOut* GetRSOut(UINT index) override { return index < rsOutCount ? _rsOuts[index] : nullptr;  }
	void AddArincWord(ArincWordOut* word) override { _arincWordsOut.push_back(word); }
	void AddArincWord(ArincWordIn* word) override { _arincWordsIn.push_back(word); }

	void UpdateInputData()override;
	void UpdateOutputData()override;

	IInputValue* GetInput(IOTYPES ioType, UINT num)override;
	IOutputValue* GetOutput(IOTYPES ioType, UINT num)override;

	void OnIrq(DWORD id) override; 
	DISPATH_RESULT ProcessFlag(Flags id) override;

	bool IsIOExist(IOTYPES ioType, UINT num) const override;

	IO_GROUP_STATE GetState() override { return _state; }
	bool IsIrqNormal() override {auto diff = _prevTimerCount != _timerCount; _prevTimerCount = _timerCount;  return diff; }
	UINT GetCurrentIrqNum() const override
	{ return _irqCount; }
};

#endif /* DRIVERSIOMANAGER_H_ */
