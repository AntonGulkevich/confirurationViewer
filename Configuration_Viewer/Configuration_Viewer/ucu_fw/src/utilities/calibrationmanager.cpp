/*
 * calibrationmanager.cpp
 *
 *  Created on: 14 авг. 2015 г.
 *      Author: Youbrin_A
 */

#include "calibrationmanager.h"

#include <string.h>


#include "../utilities/m_math.h"
#include "../drivermanager.h"
#include "../driversiohw/calibrate.h"
#include "../driversiohw/nptcontrol.h"


CalibrationManager::CalibrationManager(DriverManager* drivers, DriversIOManager* driversIO) : _drivers(drivers), _driversIO(driversIO)
{
	_calibrateState = 0;
}


DriversIOManager::Calibration& CalibrationManager::GetCalibration()
{
	return _calibration;
}

bool CalibrationManager::SetCalibration(const DriversIOManager::Calibration& calib)
{
	if (CheckCalibration(calib))
	{
		memcpy(&_calibration, &calib, sizeof(DriversIOManager::Calibration));
		SaveCalibration();
		ApplyCalibration();
		return true;
	}
	return false;

}

void CalibrationManager::SetBlockNumber(DWORD blockNumber)
{
	_calibration.blockNumber = blockNumber;
	SaveCalibration();
}

void CalibrationManager::StartCalibrationIn()
{
	ClearCalibration();
	ApplyCalibration();
	_calibrateState = 0x100;

}

bool CalibrationManager::CheckCalibration(const DriversIOManager::Calibration& calib) const
{
	CRC32 crc;
	crc.ProcessCRC(&calib, sizeof(DriversIOManager::Calibration) - 4);
	return crc.GetCRC32() == calib.cs;
}

void CalibrationManager::LoadCalibration()
{
	_drivers->ReadEEPROM(&_calibration, 0, sizeof(DriversIOManager::Calibration));
	if (!CheckCalibration(_calibration))
	{
		_calibration.version = 1;
		_calibration.blockNumber = 0xFFFFFFFF;
		ClearCalibration();
		CRC32 crc;
		crc.ProcessCRC(&_calibration, sizeof(DriversIOManager::Calibration) - 4);
		_calibration.cs = crc.GetCRC32();
	}
}

void CalibrationManager::ClearCalibration()
{
	memset(&_calibration.analogIn, 0, sizeof(ADCControl::Calibrations));
	for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
	{
		_calibration.SCT[i].scaleCos = 1;
		_calibration.SCT[i].scaleSin = 1;
	}
	for(UINT i = 0; i < DriversIOManager::NPTOutCount; i++)
	{
		_calibration.NPT[i].offset = 0;
	}
	_calibration.datetime = 0xFFFFFFFF;
}

void CalibrationManager::SaveCalibration()
{
	CRC32 crc;
	crc.ProcessCRC(&_calibration, sizeof(DriversIOManager::Calibration) - 4);
	_calibration.cs = crc.GetCRC32();
	_drivers->WriteEEPROM(&_calibration, 0, sizeof(DriversIOManager::Calibration));
}

void CalibrationManager::ApplyCalibration()
{
	_driversIO->GetADCControl()->SetCalibration(_calibration.analogIn);
	for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
		_driversIO->GetSCTOut(i)->SetCalibration(_calibration.SCT[i]);
	for(UINT i = 0; i < DriversIOManager::NPTOutCount; i++)
		_driversIO->GetNPTOut(i)->SetCalibration(_calibration.NPT[i]);

}

void CalibrationManager::CalcAvg(int gainIndex)
{
	int sumReg1 = 0;
	int sumReg2 = 0;
	int sumReg3 = 0;
	for(UINT i = 0; i < traceLength; i++)
	{
		sumReg1 += _calib[0][i];
		sumReg2 += _calib[1][i];
		sumReg3 += _calib[2][i];
	}
	_calibration.analogIn.a1[gainIndex] = ROUND(-sumReg1 / (float)traceLength);
	_calibration.analogIn.a2[gainIndex] = ROUND(-sumReg2 / (float)traceLength);
	_calibration.analogIn.a3[gainIndex] = ROUND(-sumReg3 / (float)traceLength);
}

void CalibrationManager::CalcScaleSCT(int number)
{
	int sumSin = 0;
	int sumCos = 0;
	for(UINT i = 0; i < traceLength; i++)
	{
		sumSin += _calib[number][i];
		sumCos += _calib[number+ DriversIOManager::SCTOutCount][i];
	}
	auto avgSum = (sumSin + sumCos) / 2.0f;
	_calibration.SCT[number].scaleSin = avgSum / sumSin;
	_calibration.SCT[number].scaleCos = avgSum / sumCos;
}

void CalibrationManager::CalcAvgNPT(int number)
{
	int sumReg = 0;
	for(UINT i = 0; i < traceLength; i++)
	{
		sumReg += _calib[number][i];
	}
#pragma warning вернуть калибровки НПТ после исправления контроля
	//_calibration.NPT[number].offset = ROUND(sumReg * 4 / (float)traceLength);  // Умножаем на 4 потому что ЦАП 16 бит а АЦП 12 бит
}

//#define CALIB_CHANNEL _driversIO->GetAnalogIn(0)
#define CALIB_CHANNEL _driversIO->GetCalibrate()

void CalibrationManager::ProcessCalibration()
{
	auto localStage = _calibrateState & 0xFF;
	auto globalStage = _calibrateState>>8;

	if (localStage == 0x00)
	{
		_calibrateState++; // Переход к этапу измерения
		switch(_calibrateState>>8)
		{
		case 1: // Подготовка к калибровкам
			_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Off);
			memset(&_calibration.analogIn, 0, sizeof(ADCControl::Calibrations));
			_driversIO->GetADCControl()->SetCalibration(_calibration.analogIn);
			_drivers->GetDisplay()->SetTextByText("CL_1", true);
			//_driversIO->GetAnalogIn(0)->SetSyncNumber(0);  // Раскомментировать если калибровка по информационному каналу
			break;
		case 2: // Усиление 1
			CALIB_CHANNEL->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x01);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_2, true);
			_drivers->GetDisplay()->SetTextByText("CL_2", true);
			break;
		case 3:
			CALIB_CHANNEL->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x02);
			CalcAvg(0);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_3, true);
			_drivers->GetDisplay()->SetTextByText("CL_3", true);
			break;
		case 4:
			CALIB_CHANNEL->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x05);
			CalcAvg(1);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_4, true);
			_drivers->GetDisplay()->SetTextByText("CL_4", true);
			break;
		case 5:
			CALIB_CHANNEL->SetGainCoefficient(AnalogIn::GAIN_COEFFICIENTS::K_x1);
			CalcAvg(2);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_5, true);
			_drivers->GetDisplay()->SetTextByText("CL_5", true);
			break;
		case 6:
			CalcAvg(3);
			ApplyCalibration();
			_drivers->GetDisplay()->SetTextByText("CL_6", true);
			if (_driversIO->GetState().iostSync5Fail == 1 || _driversIO->GetState().iostSync6Fail == 1  || _driversIO->GetState().iostSync7Fail == 1) //
			{
				// Если синхронизация отсутствует прерываем калибровку выходов
				_calibrateState = 0;
				_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Auto);
				_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_2, false);
				_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_3, false);
				_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_4, false);
				_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_5, false);
				_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_6, false);
				_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_7, false);
				_drivers->GetUSB()->SendProgress(0xFF, 0);
				_drivers->GetDisplay()->SetTextByText("FAIL", true);
				return;
			}
			break; // ВЫход из функции при завершении калибровок
		case 7: // Установка значений для калибровок ЦАП Sin
			for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
			{
				_driversIO->GetSCTOut(i)->PrepareCalibrationScale();
			}
			break;
		case 8: // Сбор данных
			break;
		case 9: // Калибровка масштаба
			for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
			{
				CalcScaleSCT(i);
				_driversIO->GetSCTOut(i)->ToNormalWork();
			}
			for(UINT i = 0; i < DriversIOManager::NPTOutCount; i++)
			{
				_driversIO->GetNPTOut(i)->PrepareCalibration0();
			}
			_drivers->GetDisplay()->SetTextByText("CL_A", true);
			break;
		case 10:
			break;
		case 11:
			for(UINT i = 0; i < DriversIOManager::NPTOutCount; i++)
			{
				CalcAvgNPT(i);
				_driversIO->GetNPTOut(i)->ToNormalWork();
			}
			_calibration.datetime = _drivers->GetClock()->GetCurrentTime();
			SaveCalibration();
			ApplyCalibration();
			_calibrateState = 0;
			_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Auto);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_2, false);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_3, false);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_4, false);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_5, false);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_6, false);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L7_7, false);
			_drivers->GetDisplay()->SetTextByText("dOnE", true);
			break;
		}
		_drivers->GetUSB()->SendProgress(globalStage, 0);
	} else
	{
		if (globalStage >= 2 && globalStage <= 5)
		{
			_calib[0][localStage-1] = CALIB_CHANNEL->GetRegister1();
			_calib[1][localStage-1] = CALIB_CHANNEL->GetRegister2();
			_calib[2][localStage-1] = CALIB_CHANNEL->GetRegister3();
		}
		if (globalStage == 8)

		{
			for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
			{
				_calib[i][localStage-1] = _driversIO->GetSCTControl(i)->GetRegister1();
				_calib[i + DriversIOManager::SCTOutCount][localStage-1] = _driversIO->GetSCTControl(i)->GetRegister2();
			}
		}
		if (globalStage == 10)
		{
			for(UINT i = 0; i < DriversIOManager::NPTOutCount; i++)
				_calib[i][localStage-1] = _driversIO->GetNPTControl(i)->GetRegister3();
		}
		_calibrateState++;
		if ((_calibrateState & 0xFF) == traceLength + 1)
			_calibrateState = (_calibrateState & 0xF00) + 0x100;
	}
	
}
