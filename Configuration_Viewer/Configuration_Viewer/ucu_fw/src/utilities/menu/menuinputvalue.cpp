/*
 * menuinputvalue.cpp
 *
 *  Created on: 15 сент. 2015 г.
 *      Author: Youbrin_A
 */

#include "menuinputvalue.h"

#include <string.h>

const char MenuInputValue::_symbols[_symbolsCount] = { '-', '0', '1','2','3','4','5','6','7','8','9', '_', 16 };

MenuInputValue::MenuInputValue(float* value) : MenuItem("    "), _value(value)
{
	Reset();
}

void MenuInputValue::Reset()
{
	_isActive = true;
	memset(_valueBuff, '^', _valueCount-1);
	_valueBuff[_valueCount-2] = '0';
	_valueBuff[_valueCount-1] = 0;
	_currentPos = 0;
	_currentSymPos = 1;
	_pointExist = 0;
}

const char* MenuInputValue::GetElementName()
{
	return &_valueBuff[_showStartPos];
}

void MenuInputValue::Enter()
{
	if (!_isActive)
		_isActive = true;
	else
	{
		if (_symbols[_currentSymPos] == 16 || _currentPos == _valueCount-3) // Подтверждение ввода
		{
			*_value = 0;
			int sign = 1;
			int pointPos = -1;
			for(UINT i = 0; i < _editSymbolPos; i++)
			{
				if (pointPos != -1)
					pointPos *= 10;
				if (_valueBuff[i] == '^')
					continue;
				else if (_valueBuff[i] == '-')
					sign = -1;
				else if (_valueBuff[i] == '_')
					pointPos = 1;
				else
				{
					*_value = *_value * 10 + (_valueBuff[i]-'0');
				}
			}
			*_value *= sign;
			if (pointPos != -1)
				*_value /= pointPos;
			_isActive = false;
		}
		else
		{
			for(UINT i = _editSymbolPos-_currentPos; i <= _editSymbolPos; i++)
				_valueBuff[i-1] = _valueBuff[i];
			_currentSymPos = 1;
			_currentPos++;
			_valueBuff[_editSymbolPos] = _symbols[_currentSymPos];
		}
	}
}

bool MenuInputValue::Exit()
{
	if (_currentPos == 0)
		_isActive = false;
	else
	{
		for(UINT i = _editSymbolPos-1; i >= _currentPos; i--)
			_valueBuff[i] = _valueBuff[i-1];
		_currentPos--;
	}
	return false;
}

void MenuInputValue::Prev()
{
	if (_currentSymPos <= (_currentPos > 0 ? 1 : 0))
		_currentSymPos = _symbolsCount-1;
	else
	{
		_currentSymPos--;
		if (_currentSymPos == _symbolsCount-2 && _pointExist)
			_currentSymPos--;
	}
	_valueBuff[_editSymbolPos] = _symbols[_currentSymPos];
}

void MenuInputValue::Next()
{
	if (_currentSymPos >= _symbolsCount-1)
		_currentSymPos =  _currentPos > 0 ? 1 : 0;
	else
	{
		_currentSymPos++;
		if (_currentSymPos == _symbolsCount-2 && _pointExist)
			_currentSymPos++;
	}
	_valueBuff[_editSymbolPos] = _symbols[_currentSymPos];
}
