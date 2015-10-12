/*
 * submenu.cpp
 *
 *  Created on: 22 рту. 2015 у.
 *      Author: Youbrin_A
 */

#include "submenu.h"

void SubMenu::Next()
{
	if (_isInSubMenu)
		GetCurrentSubMenu()->Next();
	else if (!GetCurrentLocalItem()->IsActive())
		_currentIndex = (_currentIndex + 1) % _items.size();
	else
		GetCurrentLocalItem()->Next();
}

void SubMenu::Prev()
{
	if (_isInSubMenu)
		GetCurrentSubMenu()->Prev();
	else if (!GetCurrentLocalItem()->IsActive())
	{
		if (_currentIndex  == 0)
			_currentIndex= _items.size()-1;
		else
			_currentIndex--;
	}
	else
		GetCurrentLocalItem()->Prev();
}


void SubMenu::Enter()
{
	if (_isInSubMenu)
		GetCurrentSubMenu()->Enter();
	else
	{
		if (GetCurrentLocalItem()->IsSubMenu())
		{
			_isInSubMenu = true;
			GetCurrentSubMenu()->_currentIndex = 0;
		} else
			GetCurrentLocalItem()->Enter();
	}
}

void SubMenu::LongEnter()
{
	if (_isInSubMenu)
		GetCurrentSubMenu()->LongEnter();
	else
		GetCurrentLocalItem()->LongEnter();
}

bool SubMenu::Exit()
{
	if (_isInSubMenu)
	{
		if (GetCurrentLocalItem()->Exit())
			_isInSubMenu = false;
		return false;
	}
	else
		if (GetCurrentLocalItem()->IsSubMenu())
			return true;
		else if (GetCurrentLocalItem()->IsActive())
			return GetCurrentLocalItem()->Exit();
		else
			return true;

}

const char* SubMenu::GetElementName()
{
	if (_isInSubMenu)
		return GetCurrentSubMenu()->GetElementName();
	else
		if (GetCurrentLocalItem()->IsSubMenu())
			return GetCurrentLocalItem()->MenuItem::GetElementName();
		else
			return GetCurrentLocalItem()->GetElementName();
}

bool SubMenu::IsHexView()
{
	if (_isInSubMenu)
		return GetCurrentSubMenu()->IsHexView();
	else
		if (GetCurrentLocalItem()->IsSubMenu())
			return GetCurrentLocalItem()->MenuItem::IsHexView();
		else
			return GetCurrentLocalItem()->IsHexView();
}


const DWORD SubMenu::GetPoints() const
{
	if (_isInSubMenu)
		return GetCurrentSubMenu()->GetPoints();
	else
		if (GetCurrentLocalItem()->IsSubMenu())
			return GetCurrentLocalItem()->MenuItem::GetPoints();
		else
			return GetCurrentLocalItem()->GetPoints();
}
