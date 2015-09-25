#include "cconfiguration.h"

#include <stdio.h>
#include "../compilier/cparserconfig.h"
#include "../compilier/cparserdevice.h"
#include "../drivers/gpio.h"
#include "../driversio/charincout.h"
#include "../driversio/charincin.h"
#include "../driversio/charincwordin.h"
#include "../driversio/charincwordout.h"
#include "../driversiohw/analogin.h"
#include "../types.h"
#include "../workmanager.h"
#include "../driversiomanager.h"
#include "../utilities/console.h"
#include "cpattern.h"


const CConfiguration::GROUP_ID CConfiguration::Groups[] =
{
	{"Без группы",                     uUndefined,     false, 0, 0},
	{"Курс",                           uDegree,        false, 3, 0},	// Курс
	{"Крен",                           uDegree,        true,  3, 0},	// Крен
	{"Тангаж",                         uDegree,        true,  3, 0},    // Тангаж
	{"Заданный путевой угол",          uDegree,        false, 3, 0},	// Заданный путевой угол
	{"Угловое положение закрылков",    uDegree,        true,  3, 0},	// УПЗ (угловое положение закрылков)
	{"Пеленг ВОР",                     uDegree,	       false, 3, 0},	// Пеленг ВОР
	{"Курсовой угол радиостанции",     uDegree,        false, 3, 0},	// Курсовой угол радиостанции (КУР)
	{"Угол сноса",                     uDegree,	       false, 3, 0},	// Угол сноса (УС)
	{"Нрв",	                           uMeter,         true,  4, 0},    // Нрв (радиовысотомер)
	{"Нотн",                           uMeter,         true,  4, 0},    // Нотн
	{"Набс",                           uMeter,         true,  4, 0},    // Набс осн.
	{"Нэшелона",                       uMeter,         false, 4, 0},    // Нэшелона
	{"Vy выч.",                        uKmph,          false, 4, 0},    // Vy выч.
	{"Vист",                           uKmph,          false, 3, 0},    // Vист
	{"V приб",                         uKmph,          false, 3, 0},    // V приб
	{"Мах",                            uNone,          false, 2, 2},    // Мах
	{"Дельта К",                       uPoints,        true,  1, 2},    // К
	{"Дельта Г",                       uPoints,        true,  1, 2},    // Г
	{"Тнар",                           uDegree,        true,  2, 1},    // Тнар
	{"W путевая скорость",             uKmph,          false, 4, 0},
	{"Wx",                             uKmph,          false, 4, 0},
	{"Wy",                             uKmph,          false, 4, 0},
	{"Wz",                             uKmph,          false, 4, 0},
	{"Оставшееся расстояние",          uKm,            false, 4, 0},
	{"ЛБУ",                            uKm,            true,  2, 1},
	{"Дальность",                      uKm,            false, 4, 0},
	{"Импульсный код",                 uUndefined,     false, 0, 0},
	{"Биполярный код",                 uUndefined,     false, 0, 0},
    {"RS-232",                         uUndefined,     false, 0, 0},
    {"Разовый признак",                uNone,	       false, 1, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
	{"Резерв",                         uUndefined,     false, 0, 0},
    {"Резерв",                         uUndefined,     false, 0, 0},
    {"Слово состояния",				   uUndefined,	   false, 0, 0}

};

CConfiguration::CConfiguration(WorkManager* workManager) : _workManager(workManager)
{
    _isInitialized = false;
    _arincInIndex = nullptr;
    _arincOutIndex = nullptr;
    _descreetOutIndex = nullptr;
    _syncUse = 0;

	for(UINT i = 0; i < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS); i++)
		registers_t[i].id = REGISTER_ID::NULLID;

    registers_t[static_cast<UINT>(REGISTER_ID::rVERSION)] = {REGISTER_ID::rVERSION, rwConstant, rtString, 0,0,0, true};
    registers_t[static_cast<UINT>(REGISTER_ID::rNAME)] = {REGISTER_ID::rNAME,    rwConstant, rtString, 0,0,0, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rDATABASENUM)] = {REGISTER_ID::rDATABASENUM, rwConstant, rtDec, 0,9999,0, true};
	registers_t[static_cast<UINT>(REGISTER_ID::rSETTINGSNUM)] = {REGISTER_ID::rSETTINGSNUM, rwConstant, rtDec, 0,99,0, false};

	for(UINT i = 0; i < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS); i++)
	{
		if (registers_t[i].id != REGISTER_ID::NULLID)
			registers_t[i].reg = new CRegister();
		else
			registers_t[i].reg = nullptr;
	}
}

CConfiguration::~CConfiguration()
{
    for (UINT i = 0; i < patterns.size(); i++)
        delete patterns[i];
	if (_arincInIndex != nullptr)
		delete[] _arincInIndex;
	if (_arincOutIndex != nullptr)
		delete[] _arincOutIndex;
	if (_descreetOutIndex != nullptr)
		delete[] _descreetOutIndex;
}

void CConfiguration::ProcessLogic()
{
	if (_isInitialized)
	{
		// Контейнерные каналы входов
		for(UINT i = 0; i < DriversIOManager::arincInCount; i++)
			if (_arincInIndex[i] != nullptr)
				_arincInIndex[i]->UpdateHWToData();
		// Остальные каналы
		for(UINT i = 0; i < channelList.size(); i++)
		{
			if (IS_IN(channelList[i]->GetType()))
				channelList[i]->UpdateHWToData();
			else if (IS_OUT(channelList[i]->GetType()))
			{
				channelList[i]->UpdateHWToData();
				static_cast<IChannelOut*>(channelList[i])->GetProcessor()->Convert();
				channelList[i]->UpdateDataToHW();
			}
		}
		// Контейнерные каналы выходов
		for(UINT i = 0; i < DriversIOManager::arincOutCount; i++)
			if (_arincOutIndex[i] != nullptr)
				_arincOutIndex[i]->UpdateDataToHW();
	}
}

IO_GROUP_STATE CConfiguration::GetState()
{
	auto state = _workManager->GetDriversIO()->GetState();
	if (IsInitialized())
	{
		state.iostArincIn1Fail = _arincInIndex[0] != nullptr && _arincInIndex[0]->IsDataExists() ? state.iostArincIn1Fail : 0;
		state.iostArincIn2Fail = _arincInIndex[1] != nullptr && _arincInIndex[1]->IsDataExists() ? state.iostArincIn2Fail : 0;
		state.iostArincIn3Fail = _arincInIndex[2] != nullptr && _arincInIndex[2]->IsDataExists() ? state.iostArincIn3Fail : 0;
		state.iostArincIn4Fail = _arincInIndex[3] != nullptr && _arincInIndex[3]->IsDataExists() ? state.iostArincIn4Fail : 0;
		state.iostArincIn5Fail = _arincInIndex[4] != nullptr && _arincInIndex[4]->IsDataExists() ? state.iostArincIn5Fail : 0;
		state.iostArincIn6Fail = _arincInIndex[5] != nullptr && _arincInIndex[5]->IsDataExists() ? state.iostArincIn6Fail : 0;

		state.iostArincOut1Fail = _arincOutIndex[0] != nullptr && _arincOutIndex[0]->IsDataExists() ? state.iostArincOut1Fail : 0;
		state.iostArincOut2Fail = _arincOutIndex[1] != nullptr && _arincOutIndex[1]->IsDataExists() ? state.iostArincOut2Fail : 0;
		state.iostArincOut3Fail = _arincOutIndex[2] != nullptr && _arincOutIndex[2]->IsDataExists() ? state.iostArincOut3Fail : 0;
		state.iostArincOut4Fail = _arincOutIndex[3] != nullptr && _arincOutIndex[3]->IsDataExists() ? state.iostArincOut4Fail : 0;

		state.iostSync1Fail = (_syncUse & 1) ? state.iostSync1Fail : 0;
		state.iostSync2Fail = (_syncUse & 2) ? state.iostSync2Fail : 0;
		state.iostSync3Fail = (_syncUse & 4) ? state.iostSync3Fail : 0;
		state.iostSync4Fail = (_syncUse & 8) ? state.iostSync4Fail : 0;
		state.iostSync5Fail = (_syncUse & (16+128)) ? state.iostSync5Fail : 0;
		state.iostSync6Fail = (_syncUse & (32+256)) ? state.iostSync6Fail : 0;
		state.iostSync7Fail = (_syncUse & (64+512)) ? state.iostSync7Fail : 0;
	}
	return state;
}


CConfiguration::LOAD_RESULT CConfiguration::LoadConfig()
{
#ifdef _NO_COMPILIER
    //manager.utils.GetTerminal().WriteLine("Загрузка конфигурации не поддерживается, загрузите соответствующее ПО");
    //manager.drivers.GetComMod().SetErrorLoad();
#else
	auto lresult = LOAD_RESULT::Error;
	auto parserC = new CParserConfig(this, _workManager->GetConsole(), _workManager->GetCommod(), _workManager->GetDriversIO());
	auto result = parserC->LoadConfiguration();
	delete parserC;
	for(UINT i = 0; i < patterns.size(); i++)
	{
		if (!GetPattern(i)->IsConstant())
		{
			auto parser = new CParserDevice(GetPattern(i)->GetDeviceNum(), this, *patterns[i], _workManager->GetConsole(), _workManager->GetCommod());
			result += parser->LoadConfiguration();
			delete parser;
		}
		Gpio::SVSet();
	}
	if (result == 0)
	{
		// Проверка логики КМ
		{
			// Проверка совместимости
			// Проверка табличных выходных аринк
			// Проверка использования функций

		}

		for(UINT i = 0; i < patterns.size(); i++)
        {
			_workManager->GetConsole()->Format("Инициализация устройства %s(%d)\r",
                GetPattern(i)->GetRegister(REGISTER_ID::rNAME)->GetValueStr(), GetPattern(i)->GetDeviceNum());
			GetPattern(i)->InitDevice();
			Gpio::SVSet();

        }
		_workManager->GetConsole()->WriteLine("Инициализация устройств завершена                                         ");
		for(UINT i = 0; i < patterns.size(); i++)
		{
			_workManager->GetConsole()->Format("Подготовка исполняемой части устройства %d\r", i);
			patterns[i]->CompileDevice();
			Gpio::SVSet();

		}
		_workManager->GetConsole()->WriteLine("Подготовка исполняемой части устройств успешна                            ");


		//_workManager->GetConsole()->Write("Конфигурация для быстрого старта подготавливается...\r");
		//manager.utils.GetArchive().Save();
		//_workManager->GetConsole()->WriteLine("Конфигурация для быстрого старта подготовлена        ");
		lresult = LOAD_RESULT::OK;
	}
	if (lresult == LOAD_RESULT::OK)
	{
		Instance(); // Преобразование номеров в указатели на входы
		PrepareLogicSequence(); // Подготовка последовательности расчета
		CreateIndexDeviceNum(); // Создание индекса по номерам устройств
		DefineChannelFailIndex();
	}
	_isInitialized = (lresult == LOAD_RESULT::OK);
	if (_isInitialized)
		sprintf(cfgNumberBuff, "%04d", GetRegister(REGISTER_ID::rDATABASENUM)->GetValueUInt());
    return lresult;
#endif
}

void CConfiguration::CreateIndexDeviceNum()
{
	memset(deviceNumIndex, 0, sizeof(deviceNumIndex));
	for(UINT i = 0; i < patterns.size(); i++)
		deviceNumIndex[patterns[i]->GetDeviceNum()] = patterns[i];
}

int CConfiguration::IndexOfChannelList(std::vector<IChannel*>& chs, IChannel* ch) const
{
	for(UINT i = 0; i < chs.size(); i++)
	{
		if (chs[i] == ch)
			return i;
	}
	return -1;
}

void CConfiguration::AddChannelToList(IChannel* ch)
{
	for(UINT i = 0; i < channelList.size(); i++)
		if (channelList[i] == ch)
			return;
	//Console::TraceLine("%d, %s, %d", ch->GetPatternIndex(), CParser::GetTypeStr(ch->GetType()), ch->GetNumber());
	channelList.push_back(ch);

}

void CConfiguration::ScanChannel(std::vector<IChannel*>& scanningChannels, IChannelOut* ch)
{
	if (ch != nullptr && IndexOfChannelList(channelList, ch) == -1 && IndexOfChannelList(scanningChannels, ch) == -1)
	{
		scanningChannels.push_back(ch);
		std::vector<IChannel*> chs;
		ch->GetProcessor()->GetDependentChannels(chs);
		for(UINT i = 0; i < chs.size(); i++)
		{
			if (IS_OUT(chs[i]->GetType()))
			{
				ScanChannel(scanningChannels, static_cast<IChannelOut*>(chs[i]));
			}
			else if (chs[i]->GetType() == ioLogicalIn)
			{
				BYTE ptn;
				IOTYPES type;
				BYTE num;
				auto res = GetConnection(chs[i]->GetPatternIndex(), chs[i]->GetType(), chs[i]->GetNumber(), ptn, type, num);
				if (res >= 0)
				{
					//Console::TraceLine(">> %d", ptn);
					ScanChannel(scanningChannels, patterns[ptn]->GetOutput(type, num));
					AddChannelToList(chs[i]);
					//Console::TraceLine("%d, %s, %d", chs[i]->GetPatternIndex(), CParser::GetTypeStr(chs[i]->GetType()), chs[i]->GetNumber());
					//Console::TraceLine("<< %d", ptn);
				}
			}
			else
			{
				AddChannelToList(chs[i]);
				//Console::TraceLine("%d, %s, %d", chs[i]->GetPatternIndex(), CParser::GetTypeStr(chs[i]->GetType()), chs[i]->GetNumber());
			}
		}
		AddChannelToList(ch);
		//Console::TraceLine("%d, %s, %d", ch->GetPatternIndex(), CParser::GetTypeStr(ch->GetType()), ch->GetNumber());
		scanningChannels.erase(scanningChannels.begin()+IndexOfChannelList(scanningChannels, ch));
	}
}

void CConfiguration::PrepareLogicSequence()
{
	std::vector<IChannel*> scanningChannels;
	for(UINT i = 0; i < patterns.size(); i++)
	{
		for(UINT o = 0; o < patterns[i]->outs.size(); o++)
		{
			ScanChannel(scanningChannels, patterns[i]->outs[o]);
		}
	}
}

UINT CConfiguration::AddPattern(bool constant)
{
	patterns.push_back(new CPattern(_workManager->GetDriversIO(), this, static_cast<UINT>(patterns.size()), constant));
	return static_cast<UINT>(patterns.size() - 1);
}

int CConfiguration::AddConnection(BYTE sp, IOTYPES st, BYTE sn, BYTE dp, IOTYPES dt, BYTE dn)
{
	PIN src;
	PIN dst;
	src.num = sn;
	src.pat = sp;
	src.type = st;
	dst.num = dn;
	dst.pat = dp;
	dst.type = dt;
	CONNECTION c;
	c.dst = dst;
	c.src = src;
	if ((IndexOfPin(src) == -1) && (IndexOfPin(dst) == -1))
		connections.push_back(c);
	else
		return -1;
	return 0;
}

int CConfiguration::IndexOfPin(PIN p)
{
	if (IS_OUT(p.type))
	{
		for (UINT i = 0; i < connections.size(); i++)
			if (p.num == connections[i].src.num &&
				p.pat == connections[i].src.pat &&
				p.type == connections[i].src.type)
					return i;
	}
	else if (IS_IN(p.type))
	{
		for (UINT i = 0; i < connections.size(); i++)
			if (p.num == connections[i].dst.num &&
				p.pat == connections[i].dst.pat &&
				((p.type == connections[i].dst.type) || (IS_LOGICAL(p.type) && IS_LOGICAL(connections[i].dst.type))))
					return i;
	}
	return -1;
}

int CConfiguration::TestConnection(BYTE sp, IOTYPES st, BYTE sn)
{
	PIN pin;
	pin.num = sn;
	pin.pat = sp;
	pin.type = st;
	return IndexOfPin(pin);
}

int CConfiguration::GetConnection(BYTE sp, IOTYPES st, BYTE sn, BYTE &dp, IOTYPES &dt, BYTE &dn)
{
	PIN pin;
	pin.num = sn;
	pin.pat = sp;
	pin.type = st;
	auto res = IndexOfPin(pin);
	if (res == -1)
		return -1;
	if (IS_OUT(st))
	{
		dp = connections[res].dst.pat;
		dt = connections[res].dst.type;
		dn = connections[res].dst.num;
	} else
	{
		dp = connections[res].src.pat;
		dt = connections[res].src.type;
		dn = connections[res].src.num;
	}
    return res;
}

BYTE CConfiguration::GetChannelHWNum(IChannel* ch)
{
	if (ch->GetType() == ioArincWordIn)
		return static_cast<ChArincWordIn*>(ch)->GetChannel()->GetNumber();
	else if (ch->GetType() == ioArincWordOut)
		return static_cast<ChArincWordOut*>(ch)->GetChannel()->GetNumber();
	else
	{
		BYTE chNum;
		BYTE ptNum;
		IOTYPES type;
		if (GetConnection(ch->GetPatternIndex(), ch->GetType(), ch->GetNumber(), ptNum, type, chNum) == -1)
			return 0xFF;
		else
			return chNum;
	}
}

void CConfiguration::Instance()
{
	for(UINT i = 0; i < patterns.size(); i++)
	{
		if (patterns[i]->IsConstant())
			constPatterns.InstanceDevice(i);
		else
			patterns[i]->InstanceDevice();
	}
}

void CConfiguration::DefineChannelFailIndex()
{
	_syncUse = 0;
	for(UINT i = 0; i < DriversIOManager::analogInCount; i++)
	{
		 if (_workManager->GetDriversIO()->GetAnalogIn(i)->GetSyncNum() != 0)
			 _syncUse = _syncUse | (1 << (_workManager->GetDriversIO()->GetAnalogIn(i)->GetSyncNum() - 1));
	}

	for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
	{
		for (UINT c = 0; c < connections.size(); c++)
			if (connections[c].dst.num == i &&
				connections[c].dst.pat == 0xFF &&
				connections[c].dst.type == ioSCT)
			{
				_syncUse = _syncUse | (1 << ((connections[c].src.num-1) + 4));
				break;
			}
	}

	_arincInIndex = new ChArincIn*[DriversIOManager::arincInCount];
	_arincOutIndex = new ChArincOut*[DriversIOManager::arincOutCount];
	_descreetOutIndex = new ChDescreeteOut*[DriversIOManager::descreeteOutputCount];

	for(UINT i = 0; i < DriversIOManager::arincInCount; i++)
	{
		_arincInIndex[i] = nullptr;
		for (UINT c = 0; c < connections.size(); c++)
			if (connections[c].src.num == i &&
				connections[c].src.pat == 0xFF &&
				connections[c].src.type == ioDigitalIn)
			{
				_arincInIndex[i] = static_cast<ChArincIn*>(GetPattern(connections[c].dst.pat)->GetInput(ioDigitalIn, connections[c].dst.num));
				break;
			}
	}
	for(UINT i = 0; i < DriversIOManager::arincOutCount; i++)
	{
		_arincOutIndex[i] = nullptr;
		for (UINT c = 0; c < connections.size(); c++)
			if (connections[c].dst.num == i &&
				connections[c].dst.pat == 0xFF &&
				connections[c].dst.type == ioDigitalOut)
			{
				_arincOutIndex[i] = static_cast<ChArincOut*>(GetPattern(connections[c].src.pat)->GetOutput(ioDigitalOut, connections[c].src.num));
				break;
			}
	}
	for(UINT i = 0; i < DriversIOManager::descreeteOutputCount; i++)
	{
		_descreetOutIndex[i] = nullptr;
		for (UINT c = 0; c < connections.size(); c++)
			if (connections[c].dst.num == i &&
				connections[c].dst.pat == 0xFF &&
				connections[c].dst.type == ioCommand)
			{
				_descreetOutIndex[i] = static_cast<ChDescreeteOut*>(GetPattern(connections[c].src.pat)->GetOutput(ioCommand, connections[c].src.num));
				break;
			}
	}
}

IChannel* CConfiguration::GetArincIn(UINT number)
{
	return number < DriversIOManager::arincInCount ? static_cast<IChannel*>(_arincInIndex[number]) : nullptr;
}

IChannel* CConfiguration::GetArincOut(UINT number)
{
	return number < DriversIOManager::arincOutCount? static_cast<IChannel*>(_arincOutIndex[number]) : nullptr;
}

IChannel* CConfiguration::GetCommandOut(UINT number)
{
	return number < DriversIOManager::descreeteOutputCount ? static_cast<IChannel*>(_descreetOutIndex[number]) : nullptr;
}


