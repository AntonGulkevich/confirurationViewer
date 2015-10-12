/*
 * analogin.cpp
 *
 *  Created on: 30 марта 2015 г.
 *      Author: Youbrin_A
 */

#include "analogin.h"
#include <complex>

#include "../utilities/console.h"

const float AnalogIn::SCTControlMaxVoltage = 20.0f;

AnalogIn::AnalogIn(DWORD baseAddress, DWORD inputOffset) :
	_baseAddress(baseAddress), _inputOffset(inputOffset)
{
	_state.dword = 0;
	_breakBufferCount = 0;
	_gainKoeff = GAIN_COEFFICIENTS::K_x01;
}

AnalogIn::~AnalogIn()
{
	// TODO Auto-generated destructor stub
}

void AnalogIn::UpdateValue()
{
	reg1 = UCU_IORD_32DIRECT(_baseAddress, REG1_OFFSET + _inputOffset);
	reg2 = UCU_IORD_32DIRECT(_baseAddress, REG2_OFFSET + _inputOffset);
	reg3 = UCU_IORD_32DIRECT(_baseAddress, REG3_OFFSET + _inputOffset);

	_value = UCU_IORD_32DIRECTF(_baseAddress, VALUE_OFFSET + _inputOffset);
	if (_breakBufferCount == breakBuffLength)
	{
		for(UINT i = 1; i < breakBuffLength; i++)
		{
			 // Можно заоптимизировать заменой на круговую очередь
			_reg1Buffer[i-1] = _reg1Buffer[i];
			_reg2Buffer[i-1] = _reg2Buffer[i];
			_reg3Buffer[i-1] = _reg3Buffer[i];
			_valBuffer[i-1] = _valBuffer[i];
		}
		_reg1Buffer[_breakBufferCount-1] = reg1;
		_reg2Buffer[_breakBufferCount-1] = reg2;
		_reg3Buffer[_breakBufferCount-1] = reg3;
		_valBuffer[_breakBufferCount-1] = _value;
	}
	else
	{
		_reg1Buffer[_breakBufferCount] = reg1;
		_reg2Buffer[_breakBufferCount] = reg2;
		_reg3Buffer[_breakBufferCount] = reg3;
		_valBuffer[_breakBufferCount] = _value;
		_breakBufferCount++;
	}

	switch(GetType())
	{
		case TYPE::NPT:
		{
			_state.iostAnalogNoSignal = 0;
			_state.iostAnalogLowLevel = 0;
			_state.iostAnalogHighLevel = (UCU_IORD_32DIRECT(_baseAddress, FAIL_OVERSCALE_OFFSET + _inputOffset) & 0x3) != 0 ? 1 : 0;
			_state.iostAnalogSyncFail = 0;
		}
		break;
		case TYPE::Logometr:
		{
			_state.iostAnalogLowLevel = UCU_IORD_32DIRECT(_baseAddress, FAIL_LOW_LEVEL_OFFSET + _inputOffset);
			_state.iostAnalogNoSignal = UCU_IORD_32DIRECT(_baseAddress, FAIL_NO_SIGNAL_OFFSET + _inputOffset);
			_state.iostAnalogHighLevel = (UCU_IORD_32DIRECT(_baseAddress, FAIL_OVERSCALE_OFFSET + _inputOffset) & 0x3) != 0 ? 1 : 0;
			_state.iostAnalogSyncFail = 0;
		}
		break;
		default:
		{
			_state.iostAnalogLowLevel = UCU_IORD_32DIRECT(_baseAddress, FAIL_LOW_LEVEL_OFFSET + _inputOffset);
			_state.iostAnalogNoSignal = UCU_IORD_32DIRECT(_baseAddress, FAIL_NO_SIGNAL_OFFSET + _inputOffset);
			_state.iostAnalogHighLevel = (UCU_IORD_32DIRECT(_baseAddress, FAIL_OVERSCALE_OFFSET + _inputOffset) & 0x7) != 0 ? 1 : 0;
			_state.iostAnalogSyncFail = ((UCU_IORD_32DIRECT(_baseAddress, 0x1040) >> (_inputOffset/4)) & 1) == 0 ? 1 : 0;
		}
		break;
	}
	_state.iostAnalogFail = GetMeasureCount();
	// ((UCU_IORD_32DIRECT(_baseAddress, 0x104C) >> (_inputOffset/4)) & 1) == 0 ? 1 : 0;
}


void AnalogIn::CalcBreak()
{
	float failSum = 0;

	for(UINT i = 0; i < breakBuffLength; i++)
	{
		failSum += m_abs(_reg1Buffer[i] + _reg2Buffer[i] + _reg3Buffer[i]);
		switch(GetType())
		{
			case TYPE::NPT:
			{
				_breakBuffer[i] = (float)(m_abs(_valBuffer[0] - _valBuffer[i]));
			}
				break;
			case TYPE::Logometr:
			{
				_breakBuffer[i] = (float)(m_abs(_reg1Buffer[0] - _reg1Buffer[i]) + m_abs(_reg2Buffer[0] - _reg2Buffer[i]) + m_abs(_reg3Buffer[0] - _reg3Buffer[i]));
				_breakBuffer[i] /= (float)0x900;
			}
				break;
			case TYPE::SCT:
			case TYPE::Selsin:
			{
				if (m_abs(_valBuffer[0] - _valBuffer[i]) < 180)
					_breakBuffer[i] = _valBuffer[i];
				else
					_breakBuffer[i] = (_valBuffer[0] < 0 && _valBuffer[i] > 0) ? (_valBuffer[i] - 360) :
                            			((_valBuffer[0] > 0 && _valBuffer[i] < 0) ? (_valBuffer[i] + 360) : _valBuffer[i]);
			}
				break;
			default:
				break;
		}
	}


    std::complex<float> a[breakBuffLength];
    std::complex<float> b[breakBuffLength];
    float avg = 0;
    for (UINT i = 0; i < breakBuffLength; i++)
        avg += _breakBuffer[i];
    avg /= (float)breakBuffLength;
    for (UINT i = 0; i < breakBuffLength; i++)
        a[i] = std::complex<float>(_breakBuffer[i] - avg ,0);
    fft(a, b, breakBuffLengthLog);
    /*if (_inputOffset == 0)
    {
        Console::Trace("%d=%08.4f\t", 0, b[0].real() * b[0].real() + b[0].imag() * b[0].imag());
        for (UINT i = 1; i <= breakBuffLength>>1; i++)
        {
            if (i & 1)
            	Console::TraceLine("%d=%08.4f", i, b[i].real() * b[i].real() + b[i].imag() * b[i].imag());
            else
            	Console::Trace("%d=%08.4f\t", i, b[i].real() * b[i].real() + b[i].imag() * b[i].imag());
        }
        Console::TraceLine("\r\n %s, %d\r\n", "Аналог", _inputOffset/4);
    }*/
	switch(GetType())
	{
	case TYPE::NPT:
        _state.iostAnalogBreak = (((b[13].real() * b[13].real() + b[13].imag() * b[13].imag() +
                     b[26].real() * b[26].real() + b[26].imag() * b[26].imag()) > 600.0f) &&
                     ((b[1].real() * b[1].real() + b[1].imag() * b[1].imag() +
                     (b[2].real() * b[2].real() + b[2].imag() * b[2].imag())< 2000.0f)));

        break;
	case TYPE::Logometr:
		_state.iostAnalogBreak = (((b[13].real() * b[13].real() + b[13].imag() * b[13].imag() +
                     b[26].real() * b[26].real() + b[26].imag() * b[26].imag()) > 100.0f) &&
                     ((b[1].real() * b[1].real() + b[1].imag() * b[1].imag() +
                     (b[2].real() * b[2].real() + b[2].imag() * b[2].imag())< 1000.0f)));
        break;
	case TYPE::SCT:
		_state.iostAnalogBreak = (((b[13].real() * b[13].real() + b[13].imag() * b[13].imag() +
                     b[26].real() * b[26].real() + b[26].imag() * b[26].imag()) > 3000.0f) &&
                     ((b[1].real() * b[1].real() + b[1].imag() * b[1].imag() +
                     (b[2].real() * b[2].real() + b[2].imag() * b[2].imag())< 15000.0f)));
        break;
	case TYPE::Selsin:
		_state.iostAnalogBreak = (((b[13].real() * b[13].real() + b[13].imag() * b[13].imag() +
                     b[26].real() * b[26].real() + b[26].imag() * b[26].imag()) > 300.0f) &&
                     ((b[1].real() * b[1].real() + b[1].imag() * b[1].imag() +
                     (b[2].real() * b[2].real() + b[2].imag() * b[2].imag())< 2000.0f)));
        break;
    default:
    	_state.iostAnalogBreak = 0;
    }
	_state.iostAnalogFail = (failSum / breakBuffLength) > 0x40;
}

void AnalogIn::SetGainCoefficient(GAIN_COEFFICIENTS coeff)
{
	_gainKoeff = coeff;
	UCU_IOWR_32DIRECT(_baseAddress, GAIN_OFFSET + _inputOffset, static_cast<UINT>(coeff));
	UINT noSignalLevel = roundf(125 * 100.0f / maxScaleV[static_cast<UINT>(coeff)]);
	UCU_IOWR_32DIRECT(_baseAddress, NO_SIGNAL_OFFSET + _inputOffset, noSignalLevel);

}

void AnalogIn::SetAngleType(ANGLE_TYPE type)
{
	_angleType = type;
	SetGainCoefficient(angleConsts[static_cast<UINT>(type)].GainCoeff);
	SetType(angleConsts[static_cast<UINT>(type)].Type);
	SetMinlevel(angleConsts[static_cast<UINT>(type)].MinLevelV);
}
