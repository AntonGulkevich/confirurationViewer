/*
 * menuitem.cpp
 *
 *  Created on: 22 ���. 2015 �.
 *      Author: Youbrin_A
 */

#include "menuitem.h"

MenuItem::MenuItem(const char* name, const DWORD points) : _name(name), _points(points)
{
	_isSubMenu = false;

}

MenuItem::~MenuItem() {
	// TODO Auto-generated destructor stub
}

