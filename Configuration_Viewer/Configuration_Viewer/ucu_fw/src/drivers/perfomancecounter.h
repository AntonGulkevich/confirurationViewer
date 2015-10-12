/*
 * perfomancecounter.h
 *
 *  Created on: 06 июля 2015 г.
 *      Author: Youbrin_A
 */

#ifndef PERFOMANCECOUNTER_H_
#define PERFOMANCECOUNTER_H_
#include "../types.h"
#include "../dllapi/IPerfomanceCounter.h"

class PerfomanceCounter: public IPerfomanceCounter
{
	static DWORD _baseAddress;
	UINT _lastBG;
	UINT _lastIrq;
	UINT _lastLogic;
	UINT _lastDiag;
	UINT _totalBG;
	UINT _totalIrq;
	UINT _totalLogic;
	UINT _totalDiag;
	UINT _lastReset;
	UINT _irqWhileBg;
	UINT _irqWhileLogic;
	UINT _irqWhileDiag;
public:
	explicit PerfomanceCounter(DWORD baseAddress);
	virtual ~PerfomanceCounter();

	static void WaitMs(UINT value)
	{
		if (_baseAddress != 0)
		{
			UINT val = UCU_IORD_32DIRECT(_baseAddress, 0x74);
			while ((val + value * 50000) > UCU_IORD_32DIRECT(_baseAddress, 0x74));
		}
	}

	UINT GetValue() const override { return UCU_IORD_32DIRECT(_baseAddress, 0x74); }

	void Start() const override { UCU_IOWR_32DIRECT(_baseAddress, 0x70, 1); }
	void Stop() const override { UCU_IOWR_32DIRECT(_baseAddress, 0x70, 0); }

	void ProcessStart() override { _lastBG = GetValue(); _irqWhileBg = 0; }
	void ProcessStop() override  { _totalBG += GetValue()-_lastBG - _irqWhileBg;  }
	void LogicStart()override  { _lastLogic = GetValue(); _irqWhileLogic = 0; }
	void LogicStop()override { _totalLogic += GetValue()-_lastLogic - _irqWhileLogic; }
	void DiagStart() override { _lastDiag = GetValue(); _irqWhileDiag = 0; }
	void DiagStop() override { _totalDiag += GetValue()-_lastDiag - _irqWhileDiag; }
	void IrqStart() override { _lastIrq = GetValue(); }
	void IrqStop() override {auto last = GetValue()-_lastIrq; _totalIrq += last; _irqWhileBg += last; _irqWhileLogic += last; _irqWhileDiag += last; }
	void Reset() override { _lastReset = _lastBG = _lastIrq = _lastLogic = _lastDiag =  GetValue(); _totalBG = _totalIrq = _totalLogic = _totalDiag =  0; }
	float GetLoadBg() const override { return static_cast<float>(_totalBG) / (GetValue() - _lastReset) * 100.0; }
	float GetLoadLogic() const override { return static_cast<float>(_totalLogic) / (GetValue() - _lastReset) * 100.0; }
	float GetLoadDiag() const override { return static_cast<float>(_totalDiag) / (GetValue() - _lastReset) * 100.0; }
	float GetLoadIrq() const override { return static_cast<float>(_totalIrq) / (GetValue() - _lastReset)* 100.0; }
};

#endif /* PERFOMANCECOUNTER_H_ */
