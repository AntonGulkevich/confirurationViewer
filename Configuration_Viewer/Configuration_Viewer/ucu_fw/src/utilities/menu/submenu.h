/*
 * submenu.h
 *
 *  Created on: 22 рту. 2015 у.
 *      Author: Youbrin_A
 */

#ifndef SUBMENU_H_
#define SUBMENU_H_
#include <vector>
#include "menuitem.h"
#include "../../types.h"

class SubMenu : public MenuItem
{
private:
	std::vector<MenuItem*> _items;
	UINT _currentIndex;
	bool _isInSubMenu;

	SubMenu* GetCurrentSubMenu() const { return reinterpret_cast<SubMenu*>(_items[_currentIndex]); }
	MenuItem* GetCurrentLocalItem() const { return _currentIndex > _items.size() ? nullptr : _items[_currentIndex]; }

public:
	explicit SubMenu(const char* name, const BYTE points = 0) : MenuItem(name, points) {  _currentIndex = 0;  _isInSubMenu = false; _isSubMenu = true;}
	virtual ~SubMenu() {}

	virtual bool IsHexView() override;
	virtual const char* GetElementName() override;
	virtual const DWORD GetPoints() const override;

	void AddItem(MenuItem* item) { _items.push_back(item); }
	void Next() override;
	void Prev() override;
	void Enter() override;
	bool Exit() override;
	void LongEnter()  override;


};

#endif /* SUBMENU_H_ */
