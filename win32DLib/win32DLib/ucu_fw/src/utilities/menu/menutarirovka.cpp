/*
 * menutarirovka.cpp
 *
 *  Created on: 15 сент. 2015 г.
 *      Author: Youbrin_A
 */

#include "menutarirovka.h"

#include <stdio.h>


#include "../../driversio/channelcalibration.h"
#include "../../utilities/m_math.h"
#include "menuinputvalue.h"

#define _TAR_TRACE

#ifdef _TAR_TRACE
#include "../../utilities/console.h"
#endif

MenuTarirovka::MenuTarirovka(ChannelCalibration* channel, const char* name, const BYTE points) : MenuItem(name, points), _channel(channel)
{
	_stage = STAGES::Header;
	_mInputValue = new MenuInputValue(&_currentValue);
	_currentPointNum = 0;
	_totalPointsCount = 0;
	_isRealMeasure = true;

}

void MenuTarirovka::QSortLinearTable(int l,int r)
{
	int i,j;
	i=l;
	j=r;

	float x;
	x = _tarArray[((UINT)(l+r) & ~1)];
	do
	{
		while(_tarArray[i<<1] < x)
			i++;
		while(x < _tarArray[j<<1])
			j--;
		if(i<=j)
		{
			float tmp = _tarArray[i<<1];
			_tarArray[i<<1] = _tarArray[j<<1];
			_tarArray[j<<1] = tmp;
			tmp = _tarArray[(i<<1)+1];
			_tarArray[(i<<1)+1] = _tarArray[(j<<1)+1];
			_tarArray[(j<<1)+1] = tmp;
			i++;
			j--;
		}

	}
	while(i<=j);
	if(l < j)
		QSortLinearTable(l, j);
	if(i < r)
		QSortLinearTable(i, r);

}

UINT MenuTarirovka::CheckTarirovka()
{
	UINT res = 0;
	float tarmin = _channel->GetTarMin();
	float tarmax = _channel->GetTarMax();
	float deltah = 0;
	for(UINT i = 0; i < _totalPointsCount; i++)
	{
		if (_tarArray[i << 1] > tarmax || _tarArray[i << 1] < tarmin)
		{
#ifdef _TAR_TRACE
			Console::TraceLine("Точка %d вне пределов тарировки", i + 1);
#endif
			res = 1;
		}
	}
	if (_channel->IsDeltaHRequired())
	{
		if (_tarArray[0] <= 10 && _tarArray[0] >= 0)
		{
			deltah = _tarArray[1] - _channel->GetTarPaspValue(_tarArray[0]);
#ifdef _TAR_TRACE
			Console::TraceLine("deltaH = %3.4f", deltah);
#endif
		}
		else
		{
#ifdef _TAR_TRACE
			Console::TraceLine("Невозможно определить поправку нулевой точки");
#endif
			res = 2;
		}
	}
	if (res == 0)
	{
    	QSortLinearTable(0,_totalPointsCount-1);
    	auto lastVal = _tarArray[1];
    	bool descending = (lastVal > _tarArray[3]);
    	for(UINT i = 0; i < _totalPointsCount; i++)
    	{
			if ((descending && lastVal >= _tarArray[(i << 1) + 1]) || (!descending && lastVal <= _tarArray[(i << 1) + 1]))
				lastVal = _tarArray[(i << 1) + 1];
			else
			{
#ifdef _TAR_TRACE
				Console::TraceLine("Функция тарировки не монотонна");
#endif
				res = 3;
			}
    	}
    	if(res == 0)
        {
    		for (UINT i = 0; i < _totalPointsCount; i++)
    		{
				if (!_tarIsRealValue[i*2+1])
					_tarArray[i*2+1] += deltah;
    		}
    		_channel->SetUserPassport(_tarArray, _totalPointsCount*2);
    		_channel->CreateKoeffTable();
#ifdef _TAR_TRACE
    		Console::TraceLine("Сохранено", _currentPointNum, _currentValue);
#endif
		}
	}


	return 0;
}

const char* MenuTarirovka::GetElementName()
{
	switch(_stage)
	{
		case STAGES::Header:
			return MenuItem::GetElementName();
		case STAGES::InputPointsCount:
		case STAGES::InputPointValue:
			return _mInputValue->GetElementName();
		case STAGES::PointHeader:
			sprintf(_headerValue, "Y_%02d", _currentPointNum+1);
			return _headerValue;
		case STAGES::SourseSelect:
			if (_isRealMeasure)
				return "PEA_";
			else
				return "CPH_";
		case STAGES::Measuring:
			if (!_channel->IsMeasuring())
			{
				_tarArray[_currentPointNum*2+1] = _channel->GetMeasuredValue();
				_currentPointNum++;
#ifdef _TAR_TRACE
				Console::TraceLine("%d. C = %f  (РЕА)", _currentPointNum, _tarArray[_currentPointNum*2+1]);
#endif
				if (_currentPointNum >= _totalPointsCount)
				{
					_errorCode = CheckTarirovka();
					if (_errorCode == 0)
						_stage = STAGES::Finish;
					else
						_stage = STAGES::Error;
				}
				else
					_stage = STAGES::PointHeader;
			}
			sprintf(_headerValue, "%04d", _channel->GetCurrentMeasurionIndex());
			return _headerValue;
		case STAGES::Error:
			sprintf(_headerValue, "Err%d", _errorCode);
			return _headerValue;
		case STAGES::Finish:
			return "H3_0";
	}
	return "    ";
}

void MenuTarirovka::Enter()
{
	switch(_stage)
	{
		case STAGES::Header:
			_stage = STAGES::InputPointsCount;
			_currentValue = 0;
			_mInputValue->Reset();
			break;
		case STAGES::InputPointsCount:
			_mInputValue->Enter();
			if (!_mInputValue->IsActive())
			{
				if (_currentValue >= 2 && _currentValue <= 64)
				{
					_currentPointNum = 0;
					_totalPointsCount = ROUND(_currentValue);
					_stage = STAGES::PointHeader;
#ifdef _TAR_TRACE
					Console::TraceLine("Тарировка %d точек", _totalPointsCount);
#endif

				}
				else
					_stage = STAGES::Error;
			}
			break;
		case STAGES::PointHeader:
			_stage = STAGES::InputPointValue;
			_mInputValue->Reset();
			break;
		case STAGES::InputPointValue:
			_mInputValue->Enter();
			if (!_mInputValue->IsActive())
			{
				_tarArray[_currentPointNum*2] = _currentValue;
#ifdef _TAR_TRACE
				Console::TraceLine("%d. y = %f", _currentPointNum, _currentValue);
#endif

				_stage = STAGES::SourseSelect;
			}
			break;
		case STAGES::SourseSelect:
			_tarIsRealValue[_currentPointNum] = _isRealMeasure;
			if(_isRealMeasure)
			{
				_stage = STAGES::Measuring;
				_channel->StartMeasure();
			} else
			{
				_tarArray[_currentPointNum*2+1] = _channel->GetTarPaspValue(_tarArray[_currentPointNum*2]);
				_currentPointNum++;
				if (_currentPointNum >= _totalPointsCount)
				{
					if (CheckTarirovka() == 0)
						_stage = STAGES::Finish;
					else
						_stage = STAGES::Error;
				}
				else
					_stage = STAGES::PointHeader;
#ifdef _TAR_TRACE
				Console::TraceLine("%d. C = %f  (СРН)", _currentPointNum, _currentValue);
#endif

			}
			break;
		case STAGES::Measuring:
			break;
		case STAGES::Error:
		case STAGES::Finish:
			_stage = STAGES::Header;
			break;
	}
}

bool MenuTarirovka::Exit()
{
	switch(_stage)
	{
		case STAGES::Header:
			return true;
		case STAGES::Error:
		case STAGES::Finish:
		case STAGES::PointHeader:
		case STAGES::SourseSelect:
			_stage = STAGES::Header;
#ifdef _TAR_TRACE
		Console::TraceLine("Прервано", _currentPointNum, _currentValue);
#endif
			break;
		case STAGES::InputPointValue:
		case STAGES::InputPointsCount:
			_mInputValue->Exit();
			if (!_mInputValue->IsActive())
				_stage = STAGES::Header;
			break;
		case STAGES::Measuring:
			break;
	}
	return false;
}

void MenuTarirovka::Prev()
{
	switch(_stage)
	{
		case STAGES::Header:
		case STAGES::Measuring:
		case STAGES::Error:
		case STAGES::Finish:
		case STAGES::PointHeader:
			break;
		case STAGES::InputPointValue:
		case STAGES::InputPointsCount:
			_mInputValue->Prev();
			break;
		case STAGES::SourseSelect:
			_isRealMeasure = !_isRealMeasure;
			break;
	}
}

void MenuTarirovka::Next()
{
	switch(_stage)
	{
		case STAGES::Header:
		case STAGES::Measuring:
		case STAGES::Error:
		case STAGES::Finish:
		case STAGES::PointHeader:
			break;
		case STAGES::InputPointValue:
		case STAGES::InputPointsCount:
			_mInputValue->Next();
			break;
		case STAGES::SourseSelect:
			_isRealMeasure = !_isRealMeasure;
			break;
	}
}



