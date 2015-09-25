/*
 * menuinputvalue.h
 *
 *  Created on: 15 сент. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MENUINPUTVALUE_H_
#define MENUINPUTVALUE_H_
#include "menuitem.h"

class MenuInputValue : public MenuItem
{
private:
	static const UINT _symbolsCount = 13;
	static const UINT _valueCount = 100;
	static const UINT _editSymbolPos = _valueCount-2;
	static const UINT _showStartPos = _valueCount-5;

	float* _value;
	char _valueBuff[_valueCount];
	UINT _currentPos;
	UINT _currentSymPos;
	static const char _symbols[_symbolsCount];

	bool _pointExist;

public:
	MenuInputValue(float* value);
	virtual ~MenuInputValue() {}

	const char* GetElementName() override;

	void Reset();

	void Enter() override;
	bool Exit() override;

	void Prev() override;
	void Next() override;
};

#endif /* MENUINPUTVALUE_H_ */
