/*
 * keys.h
 *
 *  Created on: 25 ����. 2015 �.
 *      Author: Youbrin_A
 */

#ifndef KEYS_H_
#define KEYS_H_
#include "../types.h"



class Keys
{
public:

	enum class KEY_STATE : UINT
	{
		Released = 1,
		Pressed = 0
	};

	// ����� ��� ������ �������
	union KEYS
	{
		struct
		{
			KEY_STATE Key1 : 1;
			KEY_STATE Key2 : 1;
			KEY_STATE Key3 : 1;
			KEY_STATE Key4 : 1;
			UINT  : 28;
		};
		struct
		{
			KEY_STATE Enter : 1;
			KEY_STATE Up : 1;
			KEY_STATE Down : 1;
			KEY_STATE Esc : 1;
			UINT  : 28;
		} menu;
		UINT value;
	};

private:
	KEYS _prevKeyState;
	KEYS _keyState;
	DWORD _baseAddress;

public:

	KEYS GetPrevKeyState() { return _prevKeyState; }
	KEYS GetKeyState() { return _keyState; }
	KEYS UpdateState();
#ifndef EMULATION
	bool IsBreakCombinationPressed() { UpdateState(); return _keyState.Key1 == KEY_STATE::Pressed && _keyState.Key4 == KEY_STATE::Pressed; }
#else
	bool IsBreakCombinationPressed() {
		return false;
	};
#endif
	bool IsAnyKeyPressed() { return _keyState.value != 0xF; }
	Keys(DWORD baseAddress) : _baseAddress(baseAddress) { _keyState.value = 0; }
	virtual ~Keys() {}

};

#endif /* KEYS_H_ */