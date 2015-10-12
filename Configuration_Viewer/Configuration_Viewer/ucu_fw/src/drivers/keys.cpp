/*
 * keys.cpp
 *
 *  Created on: 25 февр. 2015 г.
 *      Author: Youbrin_A
 */

#include "keys.h"


Keys::KEYS Keys::UpdateState()
{
	_prevKeyState.value = _keyState.value;
	_keyState.value = UCU_IORD_32DIRECT(_baseAddress, 0x8);
	return _keyState;
}

