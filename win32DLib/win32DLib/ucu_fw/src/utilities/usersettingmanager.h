/*
 * usersettingmanager.h
 *
 *  Created on: 18 сент. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef USERSETTINGMANAGER_H_
#define USERSETTINGMANAGER_H_
#include "../types.h"

class CConfiguration;
class Commod;
class Clock;
class IChannel;

class UserSettingManager
{
private:
	const UINT verion = 1;


	CConfiguration* _config;
	Commod* _commod;
	Clock* _clock;

	struct SettingsHeader
	{
		UINT size;
		UINT marker;
		UINT version;
		UINT datetime;
		UINT configNumber;
		UINT paramsCount;
		UINT userPassportCount;
	};

	struct RegisterData
	{
		REGISTER_ID id;
		float data;
	};

	struct ChannelRegistersData
	{
		BYTE type; // 0 - для списка регистров
		BYTE channelPattern;
		BYTE channelType;
		BYTE channelNum;
		UINT count;
		RegisterData data[static_cast<UINT>(REGISTER_ID::COUNTREGISTERS)];
	};

	struct ChannelUserPassport
	{
		BYTE type; // 1 - для пользовательских тарировок
		BYTE channelPattern;
		BYTE channelType;
		BYTE channelNum;
		UINT count;
		float data[128];
	};

	IChannel* FindChannel(UINT num, UINT pat, UINT type);

public:
	UserSettingManager(CConfiguration* config, Commod* commod, Clock* clock);
	virtual ~UserSettingManager() {}

	void Save();
	void Load();
};

#endif /* USERSETTINGMANAGER_H_ */
