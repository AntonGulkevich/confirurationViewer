#ifndef CREGISTER_H
#define CREGISTER_H

#include "../utilities/m_math.h"
#include "../dllAPI/ICRegister.h"

class Diagnostic;
//  ласс управл€ющий значением "регистра устройства".
class CRegister : public ICRegister
{
public:
	CRegister();
	virtual ~CRegister();
	float GetValueFloat()		const override { return static_cast<float>(GetValueUCUFloat()); }
	UCU_FLOAT GetValueUCUFloat()	const override { return isEmulated ? _emulatedData : _data; }
	int GetValueInt()			const override { return static_cast<int>(ftou__(GetValueUCUFloat())); }
	UINT GetValueUInt()			const override { return ftou__(GetValueUCUFloat()); }
	float GetValueArray(UINT num) const override { return ((num < sizeArray) ? dataArray[num] : 0); }
	BYTE GetArraySize() const override { return sizeArray; }
	float* GetArray() const override { return dataArray; }

	const char* GetValueStr()	const override { return (dataStr) ? dataStr : noname; }
	bool IsFilled()				const override { return filled; }

	void SetValue(const UINT lData)	override		{ _data = static_cast<UCU_FLOAT>(lData);	filled = true; }
	void SetValue(const int lData)	override		{ _data = static_cast<UCU_FLOAT>(lData);	filled = true; }
	void SetValue(const float lData) override		{ _data = static_cast<UCU_FLOAT>(lData);	filled = true; }
	void SetValue(const char* lData) override;
	void SetValueArray(float* arr, UINT size) override;

	void SetFilled(const bool lFilled = true) override	{ filled = lFilled; }
	CRegister& operator=(const CRegister& copy) { return Copy(copy); }

	static const char* GetRusName(REGISTER_ID rId);

	friend Diagnostic;
	// ”брать после отладки
	bool IsEmulated() const override{ return isEmulated; }
private:
	CRegister& Copy(const CRegister& copy);
	void SetEmulated(bool isEmul) { isEmulated = isEmul; }
	bool GetEmulated() const	{ return isEmulated; }
	void SetEmulatedValue(float lData)		{ _emulatedData = static_cast<UCU_FLOAT>(lData); }
	// „исловое значение регистра
	UCU_FLOAT _data;
	// Ёмулируемое числовое значение регистра
	UCU_FLOAT _emulatedData;
	// “екстовое значение регистра
	//std::string dataStr;
	char* dataStr;
	// массив
	float* dataArray;
	BYTE sizeArray;
	// ѕризнак заполненного регистра
	bool filled;
	// Ёмул€ци€ значени€ из диагностики
	bool isEmulated;
	static const char* noname;
};

#endif

