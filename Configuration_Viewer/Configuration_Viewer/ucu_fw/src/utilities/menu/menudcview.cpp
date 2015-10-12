/*
 * menudcview.cpp
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: Youbrin_A
 */

#include "menudcview.h"

#include <string.h>

#include "../../driversiomanager.h"
#include "../../driversiohw/descreeteinput.h"
#include "../../drivers/display.h"

MenuDCView::MenuDCView(DriversIOManager* driverIO, const char* name, const DWORD points) : MenuItem(name, points), _driversIO(driverIO)
{
	_diagnosticView =false;

}

bool MenuDCView::IsHexView()
{
	return _isActive;
}

const DWORD MenuDCView::GetPoints() const
{
	if (_isActive)
	{
		if (_diagnosticView)
		{
			return 0xF;
		}
		else
			return (UINT)_driversIO->GetDescreeteIn(0)->GetValue() << (UINT)Display::POINTS::Left;
	}
	return MenuItem::GetPoints();

}

const char* MenuDCView::GetElementName()
{
	if (_isActive)
	{
		if (_diagnosticView)
		{
			_points = 0xF;
			memset(_dcbuff, 0xFF, 4);
			return _dcbuff;
		} else
		{
			memset(_dcbuff, 0, 4);
			UINT convert[7];
			convert[0] = 5;
			convert[1] = 4;
			convert[2] = 0;
			convert[3] = 6;
			convert[4] = 3;
			convert[5] = 1;
			convert[6] = 2;
			for(UINT i = 1; i < DriversIOManager::descreeteInputCount; i++)
			{
				auto io = _driversIO->GetDescreeteIn(i);
				_dcbuff[3 - ((i-1) / 7)] |= (io->GetValue() == 1) << convert[(i-1) % 7];
			}
			return _dcbuff;
		}
	}
	else
		return MenuItem::GetElementName();
	return "    ";

}

void MenuDCView::Enter()
{
	_isActive = true;
}

bool MenuDCView::Exit()
{
	_isActive = false;
	return false;
}


void MenuDCView::Prev()
{
	_diagnosticView = !_diagnosticView;
}

void MenuDCView::Next()
{
	_diagnosticView = !_diagnosticView;
}

