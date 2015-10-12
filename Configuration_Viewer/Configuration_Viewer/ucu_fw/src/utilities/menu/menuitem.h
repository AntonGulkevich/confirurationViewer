/*
 * menuitem.h
 *
 *  Created on: 22 ���. 2015 �.
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
	bool _isActive; // ������� ��������� � �������� ������ � ��� ������� ������������� ����� ��������, ����� � ���������
	DWORD _points;

	void ValueToStr(char* buff, DWORD& points, float value, UINT trunc, UINT frac, bool sign);

public:
	explicit MenuItem(const char* name, const DWORD points = 0);
	virtual ~MenuItem();

	virtual bool IsHexView() { return false; }
	virtual const char* GetElementName() { return _name; }
	virtual const DWORD GetPoints() const { return _points; }
	bool IsSubMenu() const { return _isSubMenu; }
	virtual bool IsActive() { return _isActive; }

	virtual void Enter() {}
	virtual void LongEnter() {}
	virtual bool Exit() { return false; }

	virtual void Prev() {}
	virtual void Next() {}
};

#endif /* MENUITEM_H_ */
