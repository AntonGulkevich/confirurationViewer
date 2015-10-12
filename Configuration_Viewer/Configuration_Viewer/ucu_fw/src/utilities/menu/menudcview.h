/*
 * menudcview.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MENUDCVIEW_H_
#define MENUDCVIEW_H_
#include "menuitem.h"

class DriversIOManager;

class MenuDCView  : public MenuItem
{
private:
	DriversIOManager* _driversIO;
	bool _diagnosticView;
	char _dcbuff[5];

public:
	MenuDCView(DriversIOManager* driverIO, const char* name, const DWORD points = 0);
	virtual ~MenuDCView() {}

	bool IsHexView() override;
	const char* GetElementName() override;
	const DWORD GetPoints() const override;

	void Next() override;
	void Prev() override;
	void Enter() override;
	bool Exit() override;

};

#endif /* MENUDCVIEW_H_ */
