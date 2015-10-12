/*
 * menu.h
 *
 *  Created on: 22 авг. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MENU_H_
#define MENU_H_
#include "menu/submenu.h"

class Keys;
class Display;
class WorkManager;

class Menu
{
private:
	bool _isActive;
	WorkManager* _workManager;
	Keys* _keys;
	Display* _display;


	SubMenu* _mainMenu;
	UINT _idleTimeout;

	void CreateMenu();

	// Процедура входа в меню первое нажатие
	int _14KeysPressedTime;
	int _23KeysPressedTime;

	char versionNumBuffer[5];
	char* CreateNameByGroup(UINT groupId, UINT subGroup);

public:
	Menu(WorkManager* workManager);
	virtual ~Menu() {}

	bool IsActive() const{ return _isActive; }
	void ProcessInput();
	void Draw() const;
	void CreateConfigMenu();
};

#endif /* MENU_H_ */
