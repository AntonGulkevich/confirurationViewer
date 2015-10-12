/*
 * workmanager.h
 *
 *  Created on: 24 апр. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef WORKMANAGER_H_
#define WORKMANAGER_H_
#include "dllapi/iworkmanager.h"
#include "driversiomanager.h"
#include "interfaces/idispatchflags.h"
#include "interfaces/iusingflags.h"
#include "types.h"
#include "utilities/usersettingmanager.h"


class Commod;
class Diagnostic;
class Console;
class CConfiguration;
class FailManager;
class CalibrationManager;
class Menu;
 

class WorkManager :public IWorkManager, public IDispatchFlags, public IUsingFlags
{
private:
	DriversIOManager* _driversIO;
	DriverManager* _drivers;
	Commod* _commod;
	Diagnostic* _diagnostic;
	Console* _console;
	CConfiguration* _config;
	FailManager* _failManager;
	CalibrationManager* _calibrationManager;
	Menu* _menu;
	UserSettingManager* _userSettings;

public:
	WorkManager(DriversIOManager* driversIO, DriverManager* drivers);
	virtual ~WorkManager() {}
	void Init() const override;
	virtual DISPATH_RESULT ProcessFlag(Flags id) override;
	DriverManager* GetDrivers() const override	{ return _drivers; }
	DriversIOManager* GetDriversIO() const override	{ return _driversIO; }
	Commod* GetCommod() const override	{ return _commod; }
	Diagnostic* GetDiagnostic() const override	{ return _diagnostic; }
	Console* GetConsole() const override	{ return _console; }
	CConfiguration* GetConfiguration() const override	{ return _config; }
	FailManager* GetFailManager() const override	{ return _failManager; }
	CalibrationManager* GetCalibrationManager() const override	{ return _calibrationManager; }
	UserSettingManager* GetUserSettings() const { return _userSettings; }
	bool validateConfig();

};

#endif /* WORKMANAGER_H_ */
