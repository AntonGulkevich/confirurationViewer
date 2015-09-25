/*
 * menuitem.cpp
 *
 *  Created on: 22 рту. 2015 у.
 *      Author: Youbrin_A
 */

#include "menuitem.h"

#include "../../drivers/display.h"
#include "../../utilities/m_math.h"

MenuItem::MenuItem(const char* name, const DWORD points) : _name(name), _points(points)
{
	_isSubMenu = false;
	_isActive = false;

}

MenuItem::~MenuItem() {
	// TODO Auto-generated destructor stub
}


void MenuItem::ValueToStr(char* buff, DWORD& points, float value, UINT trunc, UINT frac, bool sign)
{
	for (int lFrac = frac; lFrac > 0; lFrac--)
		value *= 10;
	if (frac == 1)
		points = 1 << (UINT)Display::POINTS::BottomRight;
	else if(frac == 2)
		points = 1 << (UINT)Display::POINTS::BottomMiddle;
	else
		points = 0;

	if (value >= 16000)
    {
         value = 15999;
         points |= (1 << (UINT)Display::POINTS::Left);
    }
	else if (sign && value <= -1000)
    {
         value = -999;
         points |= (1 << (UINT)Display::POINTS::Left);
    }
	else if (!sign && value < 0)
    {
         value = 0;
         points |= (1 << (UINT)Display::POINTS::Left);
    }

	int ival = ROUND(value);
    buff[0] = (ival < 0 && sign) ? '-' : 0;
    ival = m_abs(ival);
	if (buff[0] == 0)
	{
		auto highdigit = (ival / 1000) % 16;
		buff[0] = highdigit > 9 ? highdigit + 'A'- 10 : highdigit + '0';
	}
	buff[3] = (ival % 10) + '0';
	buff[2] = ((ival / 10) % 10) + '0';
	buff[1] = ((ival / 100) % 10) + '0';
}

