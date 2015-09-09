#ifndef COMMOD_H
#define COMMOD_H

#include <vector>

#include "../types.h"

class I2C;

// Управление списком кофигураций загруженых в КМ
class Commod
{
private:
	I2C* _i2c;
	char cmNumberBuff[5]; // Номер КМ Для вывода на дисплей

public:
	const DWORD COMMOD_SERIAL_NUM_ADDRESS = 64 * 1024 - 10;

	explicit Commod(I2C* i2c) : _i2c(i2c)	{ opened = -1; ptr = 0; }
	virtual ~Commod()	{};
	void Init();
	int OpenDevice(UINT index);
	int OpenDevice(char* name) const	{ return 0; }
	UINT GetDeviceCount() const	{ return cFat.size(); }
	char GetChar();
	bool Eof() { return opened == -1 || ptr >= cFat[opened].size; }
	void Close()	{ opened = -1; }
	void ShowConfig()const{};	// Показать конфигурацию в терминал
	bool IsConfigurationAvaible() const	{ return cFat.size() > 0; }

	void WriteConfig(char* buffer, UINT size) const;
	void ReadConfig(char* buffer);
	UINT GetConfigSize() { return cFat.size() == 0 ? 0 : cFat[cFat.size()-1].address + cFat[cFat.size()-1].size; }
	DWORD GetCMNumber() const;
	void SetCMNumber(WORD number);
	const char* GetCMNumberStr() const	{ return cmNumberBuff; }

private:
	struct FAT
	{
		UINT address;
		DWORD size;
	};

	std::vector<FAT> cFat;
	char _currentDevice[65536];
	short opened;  // -1 ниодно устройство не открыто
	UINT ptr;	// Указатель на позицию в текущем файле

};



#endif /* COMMOD_H */
