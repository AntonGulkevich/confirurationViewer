#ifndef APPLICATION_CCONFIGURATION_H_HEADER_INCLUDED_B8BBE068
#define APPLICATION_CCONFIGURATION_H_HEADER_INCLUDED_B8BBE068

#include "../types.h"
#include <vector>
#include "cconstpatterns.h"
#include "../dllAPI/ICConfiguration.h"
#include "../driversio/chdescreeteout.h"

class WorkManager;
class CPattern;
class CProcessor;
class IChannelOut;
class ChArincOut;
class ChArincIn;

class CConfiguration: public ICConfiguration
{
public:
	struct GROUP_ID
	{
		const char* name;
		UNITS unit;
		bool sign;  // Формат вывода
		int trunc; // число целых
		int frac;  // число дробных
	};

	static const GROUP_ID Groups[];

	explicit CConfiguration(WorkManager* workManager);
	virtual ~CConfiguration();

	LOAD_RESULT LoadConfig() override;
	void ProcessLogic() override;

	CPattern* GetPattern(BYTE index) override {	return patterns[index]; }
	CPattern* GetPatternByDeviceNum(BYTE deviceNum) const override	{	return deviceNumIndex[deviceNum]; }
	UINT GetPatternCount() const override {	return patterns.size(); }
	UINT AddPattern(bool constant = false) override;
	CConstPatternEngine& GetConstPatterns()	override {	return constPatterns; }
	int AddConnection(BYTE sp, IOTYPES st, BYTE sn, BYTE dp, IOTYPES dt, BYTE dn) override;
	int TestConnection(BYTE sp, IOTYPES st, BYTE sn) override;
	int GetConnection(BYTE sp, IOTYPES st, BYTE sn, BYTE &dp, IOTYPES &dt, BYTE &dn) override;
	BYTE GetChannelHWNum(IChannel* ch) override;
	void Instance() override;
	// Возвращает указатель на регистр по ID
	CRegister* GetRegister(REGISTER_ID id) override { return (id != REGISTER_ID::NULLID) ? registers_t[static_cast<UINT>(id)].reg : nullptr; }
	// Возвращает указатель на параметры регистров по ID
	const IChannel::REGISTER_TYPE* GetRegisterT(REGISTER_ID id) override { return (id != REGISTER_ID::NULLID) ? &registers_t[static_cast<UINT>(id)] : nullptr; }

	bool IsInitialized() const override	{ return _isInitialized; }
	// Подготовка последовательности расчета (поиск в грубину)
	 void PrepareLogicSequence() override;
	IO_GROUP_STATE GetState() override;
	std::vector<IChannel*>& GetAllChannels() override { return channelList; }
	IChannel* GetArincIn(UINT number) override;
	IChannel* GetArincOut(UINT number) override;
	IChannel* GetCommandOut(UINT number) override;
	const char* GetCfgNumberStr() { return cfgNumberBuff; }
private:

	char cfgNumberBuff[5]; // Номер конфигурации Для вывода на дисплей
	WorkManager* _workManager;

    struct PIN
    {
        BYTE pat;  // 0xFF Вход
        IOTYPES type;
        BYTE num;
    };

    struct CONNECTION
    {
        PIN src;
        PIN dst;
    };

    IChannel::REGISTER_TYPE registers_t[static_cast<UINT>(REGISTER_ID::COUNTREGISTERS)];

	int IndexOfPin(PIN p); // sd = 0 src, sd = 1 dst
	int IndexOfChannelList(std::vector<IChannel*>& chs, IChannel* ch) const;
	void ScanChannel(std::vector<IChannel*>& scanningChannels, IChannelOut* ch);
	void CreateIndexDeviceNum();
	void CreateChannelIndex();
	void DefineChannelFailIndex(); // Создание индекса для формирования структуры state	
	void AddChannelToList(IChannel* ch) override;

	std::vector<CPattern*> patterns;
	CPattern* deviceNumIndex[256];
	std::vector<CONNECTION> connections;
	CConstPatternEngine constPatterns;
	std::vector<IChannel*> channelList; // Последовательность каналов для запуска вычислительной последовательности
	ChArincIn** _arincInIndex;
	ChArincOut** _arincOutIndex;
	ChDescreeteOut** _descreetOutIndex;
    bool _isInitialized;
    UINT _syncUse; // Использование синхронизаций в конфигурации (для формирования отказов)

};

#endif /* APPLICATION_CCONFIGURATION_H_HEADER_INCLUDED_B8BBE068 */
