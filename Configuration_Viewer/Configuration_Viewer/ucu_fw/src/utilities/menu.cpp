/*
 * menu.cpp
 *
 *  Created on: 22 авг. 2015 г.
 *      Author: Youbrin_A
 */

#include "menu.h"

#include <stdio.h>

#include "../drivers/keys.h"
#include "../drivers/display.h"
#include "../utilities/console.h"
#include "../workmanager.h"
#include "../drivermanager.h"
#include "../application/cconfiguration.h"
#include "../application/cpattern.h"
#include "../driversio/chanalogin.h"
#include "../utilities/commod.h"
#include "../driversio/cregister.h"
#include "menu/menuanalogparamsview.h"
#include "menu/menuchannelaction.h"
#include "menu/menudcview.h"
#include "menu/menupassportview.h"
#include "menu/menutarirovka.h"
#include "menu/menudcview.h"


Menu::Menu(WorkManager* workManager) : _workManager(workManager), _keys(workManager->GetDrivers()->GetKeys()), _display(workManager->GetDrivers()->GetDisplay())
{

	_isActive = false;
	_14KeysPressedTime = 0;
	_23KeysPressedTime = 0;
	sprintf(versionNumBuffer, "%04d", MAJORVERSION_NUM * 100 + VERSION_NUM);
	CreateMenu();
}

char* Menu::CreateNameByGroup(UINT groupId, UINT subGroup)
{
	char* text = new char[5];
	text[0] = (groupId / 10) % 10;
	text[1] = ((groupId) % 10);
	text[2] = 0xFF;
	text[3] = subGroup % 10;
	return text;
}

void Menu::CreateMenu()
{
	_mainMenu = new SubMenu("MAIN");
	_mainMenu->AddItem(new MenuDCView(_workManager->GetDriversIO(), "ПРП_"));
	auto ubs = new SubMenu("УБС_");
	_mainMenu->AddItem(ubs);
		auto apprMenu = new SubMenu("АППР");
		ubs->AddItem(apprMenu);
			apprMenu->AddItem(new MenuItem("1002", 1 << static_cast<UINT>(Display::POINTS::BottomMiddle))); // ТЮКН.467475.010-02
		auto fwMenu = new SubMenu("ПО__");
		ubs->AddItem(fwMenu);
			fwMenu->AddItem(new MenuItem(versionNumBuffer, 1 << static_cast<UINT>(Display::POINTS::BottomMiddle)));
		auto cfMenu = new SubMenu("CFr_");
		ubs->AddItem(cfMenu);
			cfMenu->AddItem(new MenuItem(_workManager->GetConfiguration()->GetCfgNumberStr()));
		auto cfuMenu = new SubMenu("CFUr");
		ubs->AddItem(cfuMenu);
			cfuMenu->AddItem(new MenuItem(_workManager->GetCommod()->GetCMNumberStr()));

}

void Menu::CreateConfigMenu()
{
	const UINT outPoints = (1 << static_cast<UINT>(Display::POINTS::TopMiddle)) | (1 << static_cast<UINT>(Display::POINTS::BottomMiddle));
	for (UINT i = 0; i < _workManager->GetConfiguration()->GetAllChannels().size(); ++i)
	{
		auto io = _workManager->GetConfiguration()->GetAllChannels()[i];
		if (io->IsConfigured())
		{
			if (io->GetRegister(REGISTER_ID::rGROUP) != nullptr && io->GetRegister(REGISTER_ID::rGROUP)->GetValueInt() != gtUnknown)
			{
				SubMenu* chmenu = nullptr;
				if (IS_IN(io->GetType()) && io->GetRegister(REGISTER_ID::rPASSPORT)!= nullptr &&  io->GetRegister(REGISTER_ID::rPASSPORT)->IsFilled())
				{
					chmenu = new SubMenu(CreateNameByGroup(io->GetRegister(REGISTER_ID::rGROUP)->GetValueUInt(), io->GetRegister(REGISTER_ID::rSUBGROUP)->GetValueUInt()), IS_IN(io->GetType()) ? 0 : outPoints);
					chmenu->AddItem(new MenuTarirovka(io->GetChCalibration(), _workManager->GetUserSettings(), _workManager->GetDrivers()->GetGpio(), "НСР "));
					auto pasp = new SubMenu("ПАСП");
					chmenu->AddItem(pasp);
						pasp->AddItem(new MenuPassportView(io, "h   ")); // Просмотр метрических параметров
						if (io->GetType() == ioAnalog && io->GetRegister(REGISTER_ID::rTYPE)->GetValueUInt() != static_cast<UINT>(ChAnalogIn::ANALOG_TYPE::NPT))
							pasp->AddItem(new MenuAnalogParamsView(io, "y   ")); // Просмотр угловых параметров
					chmenu->AddItem(new MenuChannelAction(io, MenuChannelAction::ACTIONS::ResetTarirovka, "СБРС"));
					chmenu->AddItem(new MenuChannelAction(io, MenuChannelAction::ACTIONS::ShowValue, "ПРС "));
				}
				else if (io->GetType() == ioAnalog && (io->GetRegister(REGISTER_ID::rPASSPORT)== nullptr || !io->GetRegister(REGISTER_ID::rPASSPORT)->IsFilled()))
				{
					chmenu = new SubMenu(CreateNameByGroup(io->GetRegister(REGISTER_ID::rGROUP)->GetValueUInt(), io->GetRegister(REGISTER_ID::rSUBGROUP)->GetValueUInt()), IS_IN(io->GetType()) ? 0 : outPoints);
					chmenu->AddItem(new MenuChannelAction(io, MenuChannelAction::ACTIONS::ShowValue, "ПРС "));
					auto pasp = new SubMenu("ПАСП");
					chmenu->AddItem(pasp);
						pasp->AddItem(new MenuAnalogParamsView(io, "y   ")); // Просмотр угловых параметров

				}
				else if (IS_OUT(io->GetType()) && io->GetRegister(REGISTER_ID::rDELTA)!= nullptr)
				{
					chmenu = new SubMenu(CreateNameByGroup(io->GetRegister(REGISTER_ID::rGROUP)->GetValueUInt(), io->GetRegister(REGISTER_ID::rSUBGROUP)->GetValueUInt()), IS_IN(io->GetType()) ? 0 : outPoints);
					chmenu->AddItem(new MenuItem("ППР "));
					chmenu->AddItem(new MenuItem("ПАСП"));
					chmenu->AddItem(new MenuItem("СБРС"));
				}
				if (chmenu != nullptr)
				_mainMenu->AddItem(chmenu);

			}
		}

	}
}

void Menu::ProcessInput()
{
	if (_isActive)
	{
		// Проверка на бездействие
		if (_keys->IsAnyKeyPressed())
			_idleTimeout = 0;
		else
			_idleTimeout += 80;
		if (_idleTimeout >= 60000 * 3)
		{
			while(!_mainMenu->Exit());
			_isActive = false;
		}
		else
		{
			if (_keys->GetPrevKeyState().menu.Up == Keys::KEY_STATE::Released && _keys->GetKeyState().menu.Up == Keys::KEY_STATE::Pressed)
				_mainMenu->Prev();
			if (_keys->GetPrevKeyState().menu.Down == Keys::KEY_STATE::Released && _keys->GetKeyState().menu.Down == Keys::KEY_STATE::Pressed)
				_mainMenu->Next();
			if (_keys->GetPrevKeyState().menu.Enter == Keys::KEY_STATE::Released && _keys->GetKeyState().menu.Enter == Keys::KEY_STATE::Pressed)
				_mainMenu->Enter();
			if (_keys->GetPrevKeyState().menu.Esc == Keys::KEY_STATE::Released && _keys->GetKeyState().menu.Esc == Keys::KEY_STATE::Pressed)
				if (_mainMenu->Exit())
					_isActive = false;
		}
	}else
	{
		if (_14KeysPressedTime > 0)
			_14KeysPressedTime+=80;
		if (_23KeysPressedTime > 0)
			_23KeysPressedTime+=80;
		// Отпущена комбинация 1 4
		if (_keys->GetPrevKeyState().menu.Enter == Keys::KEY_STATE::Pressed && _keys->GetKeyState().menu.Enter == Keys::KEY_STATE::Released
			&&_keys->GetPrevKeyState().menu.Esc == Keys::KEY_STATE::Pressed && _keys->GetKeyState().menu.Esc == Keys::KEY_STATE::Released)
		{
			_14KeysPressedTime = 80;
		}
		// Нажата комбинация 2 3
		else if (_keys->GetPrevKeyState().menu.Up == Keys::KEY_STATE::Released && _keys->GetKeyState().menu.Up == Keys::KEY_STATE::Pressed
		 && _keys->GetPrevKeyState().menu.Down == Keys::KEY_STATE::Released && _keys->GetKeyState().menu.Down == Keys::KEY_STATE::Pressed
		 && _14KeysPressedTime > 0 && _14KeysPressedTime < 12000)
			_23KeysPressedTime = 80;
		// Отпущена комбинация 2 3
		else  if (_keys->GetPrevKeyState().menu.Up == Keys::KEY_STATE::Pressed && _keys->GetKeyState().menu.Up == Keys::KEY_STATE::Released
				 && _keys->GetPrevKeyState().menu.Down == Keys::KEY_STATE::Pressed && _keys->GetKeyState().menu.Down == Keys::KEY_STATE::Released
				 && _23KeysPressedTime > 2000 && _14KeysPressedTime < 12000)
		{
			_idleTimeout = 0;
			_isActive= true;
		}
		else if (_keys->GetPrevKeyState().value != _keys->GetKeyState().value)
			_23KeysPressedTime = 0;
		//Console::TraceLine("prev %X, cur %X, 14 %d, 23 %d", _keys->GetPrevKeyState().Value, _keys->GetKeyState().Value, _14KeysPressedTime, _23KeysPressedTime);
	}

}

void Menu::Draw() const
{
	if (_isActive)
	{
		auto points = _mainMenu->GetPoints();
		if(_mainMenu->IsHexView())
		{
			_display->SetValue(reinterpret_cast<const BYTE*>(_mainMenu->GetElementName()));
			_display->SetPoint((points & 1) == 1, (points & 2) == 2,(points & 4) == 4,(points & 8) == 8);
			_display->Show();

		}
		else
		{
			_display->SetPoint((points & 1) == 1, (points & 2) == 2,(points & 4) == 4,(points & 8) == 8);
			_display->SetTextByText(_mainMenu->GetElementName(), false);
		}
	}
}
