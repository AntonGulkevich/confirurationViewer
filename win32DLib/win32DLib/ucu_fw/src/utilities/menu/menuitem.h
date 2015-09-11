/*
 * menuitem.h
 *
 *  Created on: 22 рту. 2015 у.
 *      Author: Youbrin_A
 */

#ifndef MENUITEM_H_
#define MENUITEM_H_
#include "../../types.h"

class MenuItem
{
private:
	const char* _name;

protected:
	bool _isSubMenu;
	DWORD _points;

public:
	explicit MenuItem(const char* name, const DWORD points = 0);
	virtual ~MenuItem();

	virtual const char* GetElementName() const { return _name; }
	virtual const DWORD GetPoints() const { return _points; }
	bool IsSubMenu() const { return _isSubMenu; }

	virtual void Enter() {}
};

#endif /* MENUITEM_H_ */
