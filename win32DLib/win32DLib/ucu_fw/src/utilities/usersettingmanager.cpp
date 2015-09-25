/*
 * usersettingmanager.cpp
 *
 *  Created on: 18 сент. 2015 г.
 *      Author: Youbrin_A
 */

#include "usersettingmanager.h"

#include "../application/cconfiguration.h"
#include "../utilities/commod.h"
#include "../drivers/clock.h"
#include "../utilities/console.h"

UserSettingManager::UserSettingManager(CConfiguration* config, Commod* commod, Clock* clock) :
	_config(config), _commod(commod), _clock(clock)
{


}

void UserSettingManager::Save()
{
	char buffer[64*1024];
	SettingsHeader* header = reinterpret_cast<SettingsHeader*>(&buffer[0]);
	header->marker = 0x27F235BD;
	header->configNumber = _config->GetRegister(REGISTER_ID::rDATABASENUM)->GetValueUInt();
	header->version = verion;
	header->datetime = 1;//_clock->GetCurrentTime();
	UINT paramsCount = 0;
	UINT userPassportCount = 0;
	UINT pos = sizeof(SettingsHeader);
	// Сохранение настроек регистров канала
	for(UINT i = 0; i < _config->GetAllChannels().size(); i++)
	{
		auto io = _config->GetAllChannels()[i];

		ChannelRegistersData* crd = reinterpret_cast<ChannelRegistersData*>(&buffer[pos]);
		crd->type = 0;
		crd->channelNum = io->GetChannelIdentificator().channelNum;
		crd->channelPattern = io->GetChannelIdentificator().channelPattern;
		crd->channelType = io->GetChannelIdentificator().channelType;
		crd->count = 0;
		for(UINT r = 0; r < (UINT)REGISTER_ID::COUNTREGISTERS; r++)
		{
			if (io->GetRegisterT((REGISTER_ID)r)->id != REGISTER_ID::NULLID && io->GetRegisterT((REGISTER_ID)r)->rw == RW::rwUser && io->GetRegister((REGISTER_ID)r)->IsFilled())
			{
				crd->data[crd->count].id = (REGISTER_ID)r;
				crd->data[crd->count].data = io->GetRegister((REGISTER_ID)r)->GetValueFloat();
				crd->count++;
			}
		}
		if (crd->count > 0)
		{
			paramsCount++;
			pos += sizeof(ChannelRegistersData) - ((UINT)REGISTER_ID::COUNTREGISTERS - crd->count) * sizeof(RegisterData);
		}
	}
	for(UINT i = 0; i < _config->GetAllChannels().size(); i++)
	{
		auto io = _config->GetAllChannels()[i];
		if (io->IsHasCalibration() && io->GetChCalibration()->IsCalibrated())
		{

			ChannelUserPassport* cup = reinterpret_cast<ChannelUserPassport*>(&buffer[pos]);
			cup->type = 1;
			cup->channelNum = io->GetChannelIdentificator().channelNum;
			cup->channelPattern = io->GetChannelIdentificator().channelPattern;
			cup->channelType = io->GetChannelIdentificator().channelType;
			cup->count = io->GetChCalibration()->GetPointsCount();
			for(UINT r = 0; r < cup->count; r++)
				cup->data[r] = io->GetChCalibration()->GetPoint(r);
			userPassportCount++;
			pos += sizeof(ChannelUserPassport) - (128 - cup->count) * sizeof(float);
		}
	}
	header->paramsCount = paramsCount;
	header->userPassportCount = userPassportCount;
	header->size = pos+4;
	UINT* cs = reinterpret_cast<UINT*>(&buffer[pos]);
	CRC32 crc;
	crc.ProcessCRC(buffer, pos);
	*cs = crc.GetCRC32();

	_commod->WriteSettings(buffer, header->size);
}

void UserSettingManager::Load()
{
	char buffer[64*1024];
	memset(buffer, 0xFF, 8);
	_commod->ReadSettings(buffer);

	SettingsHeader* header = reinterpret_cast<SettingsHeader*>(&buffer[0]);
	// Проверка маркера
	if (header->marker != 0x27F235BD)
		return;
	// Проверка версии КМ
	if (header->configNumber != _config->GetRegister(REGISTER_ID::rDATABASENUM)->GetValueUInt())
		return;
	// Проверка CS
	UINT* cs = reinterpret_cast<UINT*>(&buffer[header->size-4]);
	CRC32 crc;
	crc.ProcessCRC(buffer, header->size-4);
	if (*cs != crc.GetCRC32())
		return;

	UINT pos = sizeof(SettingsHeader);
	for(UINT i = 0; i < header->paramsCount; i++)
	{
		ChannelRegistersData* crd = reinterpret_cast<ChannelRegistersData*>(&buffer[pos]);
		pos += sizeof(ChannelRegistersData) - ((UINT)REGISTER_ID::COUNTREGISTERS - crd->count) * sizeof(RegisterData);
		auto io = FindChannel(crd->channelNum, crd->channelPattern, crd->channelType);
		if (io == nullptr)
			continue;
		if (crd->type != 0)
			continue;

		for(UINT r = 0; r < crd->count; r++)
		{
			if (io->GetRegisterT(crd->data[r].id)->id != REGISTER_ID::NULLID && io->GetRegisterT(crd->data[r].id)->rw == RW::rwUser)
			{
				io->GetRegister(crd->data[r].id)->SetValue(crd->data[r].data);
			}
		}
	}
	for(UINT i = 0; i < header->userPassportCount; i++)
	{
		ChannelUserPassport* cup = reinterpret_cast<ChannelUserPassport*>(&buffer[pos]);
		pos += sizeof(ChannelUserPassport) - (128 - cup->count) * sizeof(float);
		auto io = FindChannel(cup->channelNum, cup->channelPattern, cup->channelType);
		if (io == nullptr)
			continue;
		if (cup->type != 1)
			continue;
		if (!io->IsHasCalibration())
			continue;
		io->GetChCalibration()->SetUserPassport(cup->data, cup->count);
	}

}

IChannel* UserSettingManager::FindChannel(UINT num, UINT pat, UINT type)
{
	for(UINT i = 0; i < _config->GetAllChannels().size(); i++)
	{
		auto io = _config->GetAllChannels()[i];
		if (num == io->GetChannelIdentificator().channelNum &&
			pat == io->GetChannelIdentificator().channelPattern &&
			type == io->GetChannelIdentificator().channelType)
			return io;
	}
	return nullptr;
}

