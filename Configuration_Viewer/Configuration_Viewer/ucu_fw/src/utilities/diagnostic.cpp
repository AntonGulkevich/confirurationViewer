/*
 * diagnostic.cpp
 *
 *  Created on: 07 ���. 2015 �.
 *      Author: Youbrin_A
 */

#include "diagnostic.h"

#include <string.h>


#include "../drivermanager.h"
#include "../driversiomanager.h"
#include "../driversiohw/analogin.h"
#include "../driversiohw/nptcontrol.h"
#include "../driversiohw/potentiometr.h"
#include "../driversiohw/sctout.h"
#include "../driversiohw/nptout.h"
#include "../driversiohw/descreeteinput.h"
#include "../driversiohw/descreeteoutput.h"
#include "../driversiohw/arincin.h"
#include "../driversiohw/arincout.h"
#include "../driversiohw/adccontrol.h"
#include "../driversiohw/calibrate.h"
#include "../utilities/m_math.h"
#include "../driversiohw/rsin.h"
#include "../driversiohw/rsout.h"
#include "../workmanager.h"
#include "../utilities/failmanager.h"
#include "../application/cconfiguration.h"
#include "../application/cpattern.h"
#include "../driversio/charincin.h"
#include "../driversio/charincwordin.h"
#include "../driversio/charincout.h"
#include "../driversio/charincwordout.h"
#include "../driversio/cregister.h"
#include "../utilities/console.h"
#include "../utilities/calibrationmanager.h"
#include "../driversio/cregister.h"
#include "../driversio/ichannel.h"
#include "../driversio/channelcalibration.h"



#include "commod.h"
#include "version.h"

using version::datetime;

Diagnostic::Diagnostic(WorkManager* workManager) :
		_driversIO(workManager->GetDriversIO()), _drivers(workManager->GetDrivers()), _commod(workManager->GetCommod()), _workManager(workManager),
		_packetNumAnalog(0), _packetNumDigital(0)
{
	_mode = DiagnosticMode::Disabled;
	_diagFlashState = false;
	_oscillFlashState = false;
	_measuringChannel = nullptr;
	_measuringPointIndex = -1;


	_drivers->GetUSB()->MaskIrq(UsbTr::DataXReceived | UsbTr::EndDiagMarker | UsbTr::SomeError);

	RegFlag(Flags::SendDiagnostic, PRIORITY::BackGround);
	RegFlag(Flags::DataPCReceived, PRIORITY::BackGround);
	RegFlag(Flags::MeasurmentComplete, PRIORITY::BackGround);
	RegFlag(Flags::DataPCParse, PRIORITY::BackGround);

	_packetType = PacketType::Data;
	_currentBufferPosition = 0;
	_totalCommands = 0;
	_currentCommand = 0;

	_mode = DiagnosticMode::NormalDiagnostic;

}

Diagnostic::~Diagnostic()
{
}


void Diagnostic::FillDiagVersion()
{
	_diagVersion.marker = 0x70A1EA5F;
	_diagVersion.PLDDate = _drivers->GetUSB()->GetPLDDate();

	_diagVersion.FWDate = datetime();
	_diagVersion.blockNumber = _workManager->GetCalibrationManager()->GetBlockNumber();
	_diagVersion.FWVersion = (VERSION_NUM << 24) | (SUBVERSION_NUM << 16) | PBV;
	_diagVersion.FWCS = 0;
	_diagVersion.cmNumber = _workManager->GetCommod()->GetCMNumber();
}

void Diagnostic::FillDiagDataAnalog()
{
	_diagDataAnalog.marker = 0xD1A9DA7A;
	_diagDataAnalog.packetNum = _packetNumAnalog++;



	for(UINT i = 0; i < DriversIOManager::analogInCount; i++)
	{
		auto ch = _driversIO->GetAnalogIn(i);
		_diagDataAnalog.mainAdcMeasureCount[i] = ch->GetMeasureCount();
		_diagDataAnalog.adcValue[i] = ch->GetValue();
		_diagDataAnalog.adcChannel1[i] = ch->GetRegister1();
		_diagDataAnalog.adcChannel2[i] = ch->GetRegister2();
		_diagDataAnalog.adcChannel3[i] = ch->GetRegister3();
		_diagDataAnalog.adcFails[i] = ch->GetState().dword;
	}
	for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
	{
		auto ch = _driversIO->GetSCTControl(i);
		_diagDataAnalog.mainAdcMeasureCount[i+DriversIOManager::analogInCount] = ch->GetMeasureCount();
		_diagDataAnalog.adcSCTControlValue[i] = ch->GetValue();
		_diagDataAnalog.adcSCTControlChannel1[i] = ch->GetRegister1();
		_diagDataAnalog.adcSCTControlChannel2[i] = ch->GetRegister2();
	}
	for(UINT i = 0; i < DriversIOManager::NPTOutCount; i++)
	{
		auto ch = _driversIO->GetNPTControl(i);
		_diagDataAnalog.mainAdcMeasureCount[i+DriversIOManager::analogInCount + DriversIOManager::SCTOutCount] = ch->GetMeasureCount();
		_diagDataAnalog.adcNPTControlValue[i] = ch->GetValue();
		_diagDataAnalog.adcNPTControlChannel3[i] = ch->GetRegister3();
	}
	for(UINT i = 0; i < DriversIOManager::potentiometrCount; i++)
	{
		auto ch = _driversIO->GetPotentiometr(i);
		_diagDataAnalog.potValue[i] = ch->GetValue();
		_diagDataAnalog.potChannel[i] = ch->GetRegister();
		_diagDataAnalog.potFails[i] = ch->GetState().dword;
	}
	for(UINT i = 0; i < DriversIOManager::SCTOutCount; i++)
	{
		auto ch = _driversIO->GetSCTOut(i);
		_diagDataAnalog.SCTOutValue[i] = ch->GetValue();
		_diagDataAnalog.SCTOutChannel1[i] = ch->GetRegisterSin();
		_diagDataAnalog.SCTOutChannel2[i] = ch->GetRegisterCos();
		_diagDataAnalog.SCTOutFails[i] = ch->GetState().dword;
	}
	for(UINT i = 0; i < DriversIOManager::NPTOutCount; i++)
	{
		auto ch = _driversIO->GetNPTOut(i);
		_diagDataAnalog.NPTOutValue[i] = ch->GetValue();
		_diagDataAnalog.NPTOutChannel1[i] = ch->GetRegister();
		_diagDataAnalog.NPTOutFails[i] = ch->GetState().dword;
	}
	_diagDataAnalog.calibrateChannels[0] = _driversIO->GetCalibrate()->GetRegister1();
	_diagDataAnalog.calibrateChannels[1] = _driversIO->GetCalibrate()->GetRegister2();
	_diagDataAnalog.calibrateChannels[2] = _driversIO->GetCalibrate()->GetRegister3();
	_diagDataAnalog.calibrateChannels[3] = _driversIO->GetCalibrate()->GetReference();

	DWORD di = 0;
	for(UINT i = 0; i < DriversIOManager::descreeteInputCount; i++)
	{
		auto ch = _driversIO->GetDescreeteIn(i);
		di |= static_cast<DWORD>(ch->GetValue()) << i;
	}
	_diagDataAnalog.descreeteInput = di;
	DWORD dout = 0;
	for(UINT i = 0; i < DriversIOManager::descreeteOutputCount; i++)
	{
		auto ch = _driversIO->GetDescreeteOut(i);
		dout |= (static_cast<DWORD>(ch->GetValue()) & 1) << i;
	}
	_diagDataAnalog.descreeteOutput = dout;

	_diagDataAnalog.mainAdcFail = _driversIO->GetADCControl()->GetState().dword;
	_diagDataAnalog.mainAdcMeasureTry = _driversIO->GetADCControl()->GetMeasureTry();
	_diagDataAnalog.mainAdcMeasureWritten = _driversIO->GetADCControl()->GetMeasureWritten();
	_diagDataAnalog.mainAdcMeasureCount[22] = _driversIO->GetCalibrate()->GetMeasureCount();
	_diagDataAnalog.mainAdcMeasureCount[23] = 0;
	_diagDataAnalog.mainAdcCnvst = _driversIO->GetADCControl()->GetCnvst();
	_diagDataAnalog.mainAdcClken = _driversIO->GetADCControl()->GetClkEn();
	_diagDataAnalog.mainAdcWrchn = _driversIO->GetADCControl()->GetWrChn();
	_diagDataAnalog.analogPacketCount = _drivers->GetUSB()->GetInputDiagAnalogPacketCount();
	_diagDataAnalog.analogErrorCount = _drivers->GetUSB()->GetInputDiagAnalogPacketErrorCount();
	_diagDataAnalog.digitalPacketCount = _drivers->GetUSB()->GetInputDiagDigitalPacketCount();
	_diagDataAnalog.endMarker = 0xC215BA76;

}


void Diagnostic::FillDiagDataDigital()
{
	_diagDataDigital.marker = 0xD1A9D191;
	_diagDataDigital.packetNum = _packetNumDigital++;
	_diagDataDigital.currentTime = _drivers->GetClock()->GetCurrentTime();
	UINT count = 0;
	UINT wc = 0;
	//ARINC IN
	for(UINT i = 0;i < DriversIOManager::arincInCount; i++)
	{
		auto ch = _driversIO->GetArincIn(i);
		wc = ch->GetWordBufferCount();
		_diagDataDigital.data[count++] = wc;
		for(UINT w = 0; w < wc; w++)
		{
			_diagDataDigital.data[count+w*2] = ch->GetWordBuffer()[w].time/ 10; // ��� ������������� � ������� ���
			_diagDataDigital.data[count+w*2+1] = ch->GetWordBuffer()[w].value;
		}
		count += wc * 2;
	}

	// ARINC OUT
	for(UINT i = 0;i < DriversIOManager::arincOutCount; i++)
	{
		auto ch = _driversIO->GetArincOut(i);
		wc = ch->GetWordBufferCount();
		_diagDataDigital.data[count++] = wc;
		for(UINT w = 0; w < wc; w++)
		{
			_diagDataDigital.data[count+w*2] = ch->GetWordBuffer()[w].time/ 10; // ��� ������������� � ������� ���;
			_diagDataDigital.data[count+w*2+1] = ch->GetWordBuffer()[w].value;
		}
		count += wc * 2;
		ch->ResetWordBuffer();
	}
	// RS ��
	for(UINT i = 0;i < DriversIOManager::rsInCount; i++)
	{
		auto ch = _driversIO->GetRSIn(i);
		wc = (ch->GetBufferSize() + 3) >> 2;
		_diagDataDigital.data[count++] = ch->GetBufferSize();
		memcpy(&_diagDataDigital.data[count], ch->GetBuffer(), ch->GetBufferSize());
		count += wc;
	}

	//RS���
	for(UINT i = 0;i < DriversIOManager::rsOutCount; i++)
	{
		auto ch = _driversIO->GetRSOut(i);
		wc = (ch->GetBufferSize() + 3) >> 2;
		_diagDataDigital.data[count++] = ch->GetBufferSize();
		memcpy(&_diagDataDigital.data[count], ch->GetBuffer(), ch->GetBufferSize());
		count += wc;
		ch->ResetDiagBuffer();
	}

	// ������ �����
	_diagDataDigital.data[count++] = 0xC2FAAD15;

	_diagDataDigital.count = count + 3; // 3 - ������ + ����� + �����
}

void Diagnostic::FillCommandResponse(Commands cmd)
{
	_commandResponse.marker = 0xABABABAB;
	_commandResponse.command = static_cast<UINT>(cmd);
	_commandResponse.allOkMarker = 0x3F344A20;
}

void Diagnostic::FillConfigurationData()
{
	ConfigurationData cdata;
	cdata.marker = 0xAC8D0440;
	UINT channelsCount = 0;
	if (_workManager->GetConfiguration()->IsInitialized())
	{
		for(UINT p = 0; p < _workManager->GetConfiguration()->GetPatternCount(); p++)
		{
			for(UINT c = 0; c < _workManager->GetConfiguration()->GetPattern(p)->GetAllInputs().size(); c++)
			{
				auto ch = _workManager->GetConfiguration()->GetPattern(p)->GetAllInputs()[c];
				cdata.data[channelsCount] = ch->GetChannelIdentificator();
				cdata.data[channelsCount].channelHWNum = _workManager->GetConfiguration()->GetChannelHWNum(ch);
				channelsCount++;
			}
			for(UINT c = 0; c < _workManager->GetConfiguration()->GetPattern(p)->GetAllOutputs().size(); c++)
			{
				auto ch = _workManager->GetConfiguration()->GetPattern(p)->GetAllOutputs()[c];
				cdata.data[channelsCount] = ch->GetChannelIdentificator();
				cdata.data[channelsCount].channelHWNum = _workManager->GetConfiguration()->GetChannelHWNum(ch);
				channelsCount++;
			}
		}
	}
	cdata.channelsCount = channelsCount;
	cdata.count = cdata.channelsCount * sizeof(IChannel::ChannelData) + 4 + 4; // ���������� ������� + ������ + ����������
	_drivers->GetUSB()->WriteData(&cdata, cdata.count);


}

void Diagnostic::FillRegisterData(UINT patternNum, IOTYPES type, UINT num)
{
	ChannelRegistersData cdata;
	cdata.marker = RegistersRequest;
	cdata.irqNum = _driversIO->GetCurrentIrqNum();
	cdata.channelPattern = patternNum;
	cdata.channelType = type;
	cdata.channelNum = num;
	cdata.count = 0;
	UINT size = 0;
	if (patternNum == ConfigurationId)
	{
		cdata.marker = ConfigurationId;
		cdata.count = 2;
		auto regT = _workManager->GetConfiguration()->GetRegisterT(REGISTER_ID::rNAME);
		auto reg = _workManager->GetConfiguration()->GetRegister(REGISTER_ID::rNAME);
		auto data = reinterpret_cast<RegisterData*>(&cdata.data[size]);
		data->id = static_cast<BYTE>(regT->id);
		data->type = static_cast<BYTE>(regT->type);
		data->isFilled = reg->IsFilled() ? 1 : 0;
		data->registerType = static_cast<BYTE>(regT->rw);
		data->isEmulated = static_cast<BYTE>(reg->GetEmulated()) ? 1 : 0;
		data->size = strlen(reg->GetValueStr());
		memcpy(data->data, reg->GetValueStr(), data->size);
		BYTE size4 = (data->size +3) & 0xFC;
		for(UINT i = data->size; i < size4; i++)
			data->data[i] = 0;
		data->size = size4;
		size += data->size + 8;

		regT = _workManager->GetConfiguration()->GetRegisterT(REGISTER_ID::rDATABASENUM);
		reg = _workManager->GetConfiguration()->GetRegister(REGISTER_ID::rDATABASENUM);
		data = reinterpret_cast<RegisterData*>(&cdata.data[size]);
		data->id = static_cast<BYTE>(regT->id);
		data->type = static_cast<BYTE>(regT->type);
		data->isFilled = reg->IsFilled() ? 1 : 0;
		data->registerType = static_cast<BYTE>(regT->rw);
		data->isEmulated = static_cast<BYTE>(reg->GetEmulated()) ? 1 : 0;
		data->size = 4;
		auto val = reg->GetValueInt();
		memcpy(data->data, &val, data->size);
		size += data->size + 8;
	}
	else
	{
		auto pattern = _workManager->GetConfiguration()->GetPatternByDeviceNum(patternNum);
		if (pattern != nullptr)
		{
			auto ch = (IS_IN(type) ? static_cast<IChannel*>(pattern->GetInput(type, num)) :
										  static_cast<IChannel*>(pattern->GetOutput(type, num)));
			if (ch != nullptr)
			{
				for(UINT i = 0; i < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS); i++)
					if (ch->GetRegisterT(static_cast<REGISTER_ID>(i))->id != REGISTER_ID::NULLID)
					{
						auto data = reinterpret_cast<RegisterData*>(&cdata.data[size]);
						data->id = static_cast<BYTE>(ch->GetRegisterT(static_cast<REGISTER_ID>(i))->id);
						data->type = static_cast<BYTE>(ch->GetRegisterT(static_cast<REGISTER_ID>(i))->type);
						data->isFilled =  ch->GetRegister(static_cast<REGISTER_ID>(i))->IsFilled() ? 1 : 0;
						data->registerType = static_cast<BYTE>(ch->GetRegisterT(static_cast<REGISTER_ID>(i))->rw);
						data->isEmulated = static_cast<BYTE>(ch->GetRegister(static_cast<REGISTER_ID>(i))->GetEmulated()) ? 1 : 0;
						switch(data->type)
						{
						case rtDec:
						{
							data->size = 4;
							auto val = ch->GetRegister(static_cast<REGISTER_ID>(i))->GetValueInt();
							memcpy(data->data, &val, data->size);
						}
							break;
						case rtFloat:
						{
							data->size = 4;
							auto val = ch->GetRegister(static_cast<REGISTER_ID>(i))->GetValueFloat();
							memcpy(data->data, &val, data->size);
						}
							break;
						case rtString:
						{
							data->size = strlen(ch->GetRegister(static_cast<REGISTER_ID>(i))->GetValueStr());
							memcpy(data->data, ch->GetRegister(static_cast<REGISTER_ID>(i))->GetValueStr(), data->size);
							BYTE size4 = (data->size +3) & 0xFC;
							for(UINT i = data->size; i < size4; i++)
								data->data[i] = 0;
							data->size = size4;
						}
							break;
						case rtArray:
						{
							data->size = ch->GetRegister(static_cast<REGISTER_ID>(i))->GetArraySize() * 4;
							memcpy(data->data, ch->GetRegister(static_cast<REGISTER_ID>(i))->GetArray(), data->size);
						}
							break;

						default:
							data->size = 0;
							break;
						}
						size += data->size + 8;
						cdata.count++;
					}

			}

		}
	}
	_drivers->GetUSB()->WriteData(&cdata, size + 4 + 4 + 4 + 4); // ������ + ���������� + ������������� ������ + ����� ����������

}

void Diagnostic::FillChannelsSettings()
{
	ChannelsSettings data;
	data.marker = Markers::ChannelSettings;
	for(UINT i = 0; i < DriversIOManager::analogInCount; i++)
	{
		data.analog[i].type = static_cast<UINT>(_driversIO->GetAnalogIn(i)->GetType());
		data.analog[i].analogType = data.analog[i].type == static_cast<UINT>(AnalogIn::TYPE::NPT) ? static_cast<UINT>(_driversIO->GetAnalogIn(i)->GetGainKoefficient()) : static_cast<UINT>(_driversIO->GetAnalogIn(i)->GetAngleType());
		data.analog[i].delay = _driversIO->GetAnalogIn(i)->GetDelay();
		data.analog[i].syncNum =  _driversIO->GetAnalogIn(i)->GetSyncNum();
	}
	for(UINT i = 0;i < DriversIOManager::SCTOutCount; i++)
	{
		data.sct[i].type = static_cast<WORD>(_driversIO->GetSCTOut(i)->GetType());
		data.sct[i].phase = static_cast<WORD>(_driversIO->GetSCTOut(i)->GetPhaseShift());
	}
	for(UINT i = 0;i < DriversIOManager::arincInCount; i++)
	{
		data.arincIn[i].rate = static_cast<UINT>(_driversIO->GetArincIn(i)->GetRate());
		data.arincIn[i].parity = static_cast<UINT>(_driversIO->GetArincIn(i)->GetParity());
	}
	for(UINT i = 0;i < DriversIOManager::arincOutCount; i++)
	{
		data.arincOut[i].rate = static_cast<UINT>(_driversIO->GetArincOut(i)->GetRate());
		data.arincOut[i].parity = static_cast<UINT>(_driversIO->GetArincOut(i)->GetParity());
	}
	for(UINT i = 0;i < DriversIOManager::rsInCount; i++)
	{
		data.rsIn[i].rate = static_cast<UINT>(_driversIO->GetRSIn(i)->GetBaudRate()) / 100;
		data.rsIn[i].parity = static_cast<UINT>(_driversIO->GetRSIn(i)->GetParityType());
		data.rsIn[i].stopBits = _driversIO->GetRSIn(i)->GetStopBits();
		data.rsIn[i].dataBits = _driversIO->GetRSIn(i)->GetNumberBits();
	}
	for(UINT i = 0;i < DriversIOManager::rsOutCount; i++)
	{
		data.rsOut[i].rate = static_cast<UINT>(_driversIO->GetRSOut(i)->GetBaudRate()) / 100;
		data.rsOut[i].parity = static_cast<UINT>(_driversIO->GetRSOut(i)->GetParityType());
		data.rsOut[i].stopBits = _driversIO->GetRSOut(i)->GetStopBits();
		data.rsOut[i].dataBits = _driversIO->GetRSOut(i)->GetNumberBits();
	}
	data.isInverseOff = _driversIO->GetADCControl()->IsInverseOff() ? 1 : 0;
	_drivers->GetUSB()->WriteData(&data, sizeof(ChannelsSettings));
}

RS::ParityType Diagnostic::ConvertRSParity(UINT p)
{
	switch(p)
	{
	case 0:
		return RS::ParityType::Odd;
	case 1:
		return RS::ParityType::Even;
	case 2:
		return RS::ParityType::None;
	case 3:
		return RS::ParityType::Always1;
	case 4:
		return RS::ParityType::Always0;
	default:
		return RS::ParityType::None;
	}
}

void Diagnostic::UnpackChannelsSettings(const ChannelsSettings* data)
{
	for(UINT i = 0;i < DriversIOManager::analogInCount; i++)
		if (data->analog[i].type == static_cast<UINT>(AnalogIn::TYPE::NPT))
		{
			_driversIO->GetAnalogIn(i)->SetType(AnalogIn::TYPE::NPT);
			_driversIO->GetAnalogIn(i)->SetSyncNumber(0);
			_driversIO->GetAnalogIn(i)->SetGainCoefficient(static_cast<AnalogIn::GAIN_COEFFICIENTS>(data->analog[i].analogType));
		}
		else if(data->analog[i].type == static_cast<UINT>(AnalogIn::TYPE::Selsin))
		{
			_driversIO->GetAnalogIn(i)->SetAngleType(AnalogIn::ANGLE_TYPE::Selsin);
			_driversIO->GetAnalogIn(i)->SetDelay(data->analog[i].delay);
			_driversIO->GetAnalogIn(i)->SetSyncNumber(data->analog[i].syncNum);
		}
		else if(data->analog[i].type == static_cast<UINT>(AnalogIn::TYPE::Logometr))
		{
			_driversIO->GetAnalogIn(i)->SetAngleType(AnalogIn::ANGLE_TYPE::Logometr);
			_driversIO->GetAnalogIn(i)->SetDelay(data->analog[i].delay);
			_driversIO->GetAnalogIn(i)->SetSyncNumber(data->analog[i].syncNum);
		}
		else if(data->analog[i].type == static_cast<UINT>(AnalogIn::TYPE::SCT))
		{
			_driversIO->GetAnalogIn(i)->SetAngleType(static_cast<AnalogIn::ANGLE_TYPE>(data->analog[i].analogType));
			_driversIO->GetAnalogIn(i)->SetDelay(data->analog[i].delay);
			_driversIO->GetAnalogIn(i)->SetSyncNumber(data->analog[i].syncNum);
		}
	_driversIO->GetCalibrate()->SetGainCoefficient(static_cast<AnalogIn::GAIN_COEFFICIENTS>(data->calibrate.analogType));
	for(UINT i = 0;i < DriversIOManager::SCTOutCount; i++)
	{
		_driversIO->GetSCTOut(i)->SetType(static_cast<SCTOut::SCTType>(data->sct[i].type));
		_driversIO->GetSCTOut(i)->SetPhaseShift(data->sct[i].phase);
	}
	for(UINT i = 0;i < DriversIOManager::arincInCount; i++)
	{
		_driversIO->GetArincIn(i)->SetRate(static_cast<Arinc::Rate>(data->arincIn[i].rate));
		_driversIO->GetArincIn(i)->SetParity(static_cast<Arinc::ParityIn>(data->arincIn[i].parity));
	}
	for(UINT i = 0;i < DriversIOManager::arincOutCount; i++)
	{
		_driversIO->GetArincOut(i)->SetRate(static_cast<Arinc::Rate>(data->arincOut[i].rate));
		_driversIO->GetArincOut(i)->SetParity(static_cast<Arinc::ParityOut>(data->arincOut[i].parity));
		_driversIO->GetArincOut(i)->ClearFifo();
	}
	for(UINT i = 0;i < DriversIOManager::rsInCount; i++)
	{
		_driversIO->GetRSIn(i)->SetBaudRate(static_cast<RS::BaudRate>(data->rsIn[i].rate));
		_driversIO->GetRSIn(i)->SetParityType(ConvertRSParity(data->rsIn[i].parity));
		_driversIO->GetRSIn(i)->SetStopBits(data->rsIn[i].stopBits);
		_driversIO->GetRSIn(i)->SetNumberBits(data->rsIn[i].dataBits);
	}
	for(UINT i = 0;i < DriversIOManager::rsOutCount; i++)
	{
		_driversIO->GetRSOut(i)->SetBaudRate(static_cast<RS::BaudRate>(data->rsOut[i].rate));
		_driversIO->GetRSOut(i)->SetParityType(ConvertRSParity(data->rsOut[i].parity));
		_driversIO->GetRSOut(i)->SetStopBits(data->rsOut[i].stopBits);
		_driversIO->GetRSOut(i)->SetNumberBits(data->rsOut[i].dataBits);
	}
	_driversIO->GetADCControl()->SetInverseOff(data->isInverseOff == 1);

	switch(data->refernce)
	{
	case 0:
		_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Off);
		break;
	case 1:
		_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Ref1);
		break;
	case 2:
		_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Ref2);
		break;
	case 3:
		_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Ref3);
		break;
	default:
		_driversIO->GetADCControl()->SetReference(ADCControl::Reference::Auto);
		break;
	}
}

void Diagnostic::FillDynamicData()
{
	DynamicData data;
	data.marker = 0xb3897c68;
	data.datetime = _drivers->GetClock()->GetCurrentTime();
	data.perfomanceIrq = _drivers->GetPerfomance()->GetLoadIrq();
	data.perfomanceBg = _drivers->GetPerfomance()->GetLoadBg();
	data.perfomanceLogic = _drivers->GetPerfomance()->GetLoadLogic();
	data.perfomanceDiag = _drivers->GetPerfomance()->GetLoadDiag();

	data.temperature = _drivers->GetTermometr()->GetCurrentTemperature();
	_drivers->GetUSB()->WriteData(&data, sizeof(DynamicData));

}

void Diagnostic::FillFailsData()
{
	FailsData data;
	data.marker = Markers::Fails;
	data.count = _workManager->GetFailManager()->GetFailsCount();
	data.currentFailIndex = _workManager->GetFailManager()->GetCurrentFail();
	for(UINT i = 0; i < data.count; i++)
	{
		data.fails[i].chData = _workManager->GetFailManager()->GetFails()[i].chData;
		auto st = _workManager->GetFailManager()->GetFails()[i].status;
		data.fails[i].status = (st.first % 0xFF) | ((st.second % 0xFF) << 8) | ((st.third % 0xFF) << 16) | ((st.fourth % 0xFF) << 24);
	}

	_drivers->GetUSB()->WriteData(&data, data.count * 8 + 4 + 4 + 4);

}

void Diagnostic::ParsePacketData()
{
	auto data = _drivers->GetUSB()->GetXDataBuffer();
	auto size = _drivers->GetUSB()->GetXDataSize();
	if (_currentBufferPosition == 0)
	{
		_totalCommands = 0;
		_drivers->GetUSB()->StartXDataParse();
		data = _drivers->GetUSB()->GetXDataBuffer();
		size = _drivers->GetUSB()->GetXDataSize();
		if (size < 4)
		{
			_currentBufferPosition = 0;
			return;
		}
	}
	while(_currentBufferPosition < size)
	{
		if (_totalCommands == 0)
		{
			auto marker = *reinterpret_cast<const DWORD*>(&data[_currentBufferPosition]);
			if (marker == (DWORD)Markers::PCCommands)
			{
				_packetType = PacketType::Commands;
				_currentBufferPosition += 4;
				_totalCommands = *reinterpret_cast<const DWORD*>(&data[_currentBufferPosition]);
				if (_totalCommands > 100)
				{
					Console::TraceLine("������ ��������� ������");
					_currentBufferPosition = 0;
					return;
				}
				_currentBufferPosition +=4;
				_currentCommand = 0;
			} else if (marker == (DWORD)Markers::PCData)
			{
				_packetType = PacketType::Data;
				_currentBufferPosition += 4;
				_dataSize = *reinterpret_cast<const UINT*>(&data[_currentBufferPosition]);
				_currentBufferPosition += 4;
			} else
			{
				Console::TraceLine("������ ��������� ���� ������");
				_currentBufferPosition = 0;
				return;
			}
		}

		switch(_packetType)
		{
		case PacketType::Commands:
			if (_currentCommand < _totalCommands)
			{
				if (data[_currentBufferPosition] == 0x5D && data[_currentBufferPosition+3] == 0xE4)
					ProcessCommands(static_cast<Commands>(*reinterpret_cast<const WORD*>(&data[_currentBufferPosition+1])));
				_currentBufferPosition += 4;
				_currentCommand++;
			}
			else
			{
				_totalCommands = 0;
			}

			break;
		case PacketType::Data:
			ProcessData(&data[_currentBufferPosition], _dataSize);
			_currentBufferPosition+=_dataSize;
			break;
		}
	/*if (_currentBufferPosition >= size)
	{
		_currentBufferPosition = 0;
	} else
		SetFlag(Flags::DataPCParse);*/
	}
	_currentBufferPosition = 0;
}

void Diagnostic::ProcessCommands(Commands cmd)
{
	switch(cmd)
	{
	case Commands::VersionRequest:
		FillDiagVersion();
		_drivers->GetUSB()->WriteData(&_diagVersion, sizeof(VersionData));
		break;
	case Commands::LoadFW:
		FillCommandResponse(cmd);
		_drivers->GetUSB()->WriteData(&_commandResponse, sizeof(CommandResponse));
		_drivers->GetFWLoader()->LoadFW();
		break;
	case Commands::SaveCM:
	{
		auto size = _commod->GetConfigSize();
		SaveCMData str;
		str.marker = Markers::LoadCM;

		if (size > 0 && size < 64*1024)
		{
			str.size = size;
			_commod->ReadConfig(str.data);
			_drivers->GetUSB()->WriteData(&str, size+8);
		}
		else
		{
			str.size = 4;
			DWORD* answ = reinterpret_cast<DWORD*>(str.data);
			*answ = 0xDEADDEAD;
		}
	}
	break;
	case Commands::DiagnosticEnable:
		_mode = DiagnosticMode::NormalDiagnostic;
		_driversIO->GetADCControl()->DisableOscillograph();
		break;
	case Commands::DiagnosticDisable:
		_mode = DiagnosticMode::Disabled;
		_driversIO->GetADCControl()->DisableOscillograph();
		break;
	case Commands::Configuration:
		FillConfigurationData();
		break;
	case Commands::GetChannelSettings:
		FillChannelsSettings();
		break;
	case Commands::GetDynamicInfo:
		FillDynamicData();
		break;
	case Commands::GetFails:
		FillFailsData();
		break;
	case Commands::Reboot:
		_drivers->GetDisplay()->SetTextByText("����");
		for(;;);
		break;
	case Commands::StartCalibrate:
		_workManager->GetCalibrationManager()->StartCalibrationIn();
		break;
	case Commands::GetCalibrate:
	{
		BYTE data[sizeof(DriversIOManager::Calibration) + 4]; // ������ + ����� �����
		*reinterpret_cast<DWORD*>(&data) = Markers::Calibrate;
		memcpy(&data[4], &_workManager->GetCalibrationManager()->GetCalibration(), sizeof(DriversIOManager::Calibration));
		_drivers->GetUSB()->WriteData(data, sizeof(DriversIOManager::Calibration) + 4);
	}
		break;
	case Commands::SaveUserSettings:
		_drivers->GetGpio()->SetCMWriteProtect(false);
		_workManager->GetUserSettings()->Save();
		_drivers->GetGpio()->SetCMWriteProtect(true);
		break;
	default:
		break;
	}
}

void Diagnostic::ProcessData(const BYTE* _inDataBuffer, UINT size)
{
	auto marker = *reinterpret_cast<const DWORD*>(_inDataBuffer);
	switch(marker)
	{
	case ChannelSettings:
		UnpackChannelsSettings(reinterpret_cast<const ChannelsSettings*>(_inDataBuffer));
		break;
	case EnableOscillograph:
	{
		auto channelNum = *reinterpret_cast<const DWORD*>(&_inDataBuffer[4])-1;
		auto koeff = *reinterpret_cast<const DWORD*>(&_inDataBuffer[8]);
		_driversIO->GetADCControl()->EnableOscillograph(channelNum, static_cast<AnalogIn::GAIN_COEFFICIENTS>(koeff));
		_mode = DiagnosticMode::Oscillograph;
	}
		break;
	case RegistersRequest:
	{
		if (_workManager->GetConfiguration()->IsInitialized())
		{
			auto count = (*reinterpret_cast<const DWORD*>(&_inDataBuffer[4]) & 0xF);
			for(UINT i = 0; i < count; i++)
			{
				auto patternNum = *reinterpret_cast<const DWORD*>(&_inDataBuffer[8+i*12]);
				auto channelType = *reinterpret_cast<const IOTYPES*>(&_inDataBuffer[12+i*12]);
				auto channelNum = *reinterpret_cast<const DWORD*>(&_inDataBuffer[16+i*12]);
				FillRegisterData(patternNum, channelType, channelNum);
			}
		}
	}
	break;
	case SetRegister:
	{
		if (_workManager->GetConfiguration()->IsInitialized())
		{
				DWORD patternNum = _inDataBuffer[4];
				auto channelType = static_cast<IOTYPES>(_inDataBuffer[5]);
				DWORD channelNum = _inDataBuffer[6];
				DWORD regId = _inDataBuffer[7];
			auto isEmulated = *reinterpret_cast<const DWORD*>(&_inDataBuffer[8]);
				auto pattern = _workManager->GetConfiguration()->GetPatternByDeviceNum(patternNum);
				if (pattern != nullptr)
				{
					auto ch = (IS_IN(channelType) ? static_cast<IChannel*>(pattern->GetInput(channelType, channelNum)) :
												  static_cast<IChannel*>(pattern->GetOutput(channelType, channelNum)));
					if (ch != nullptr && regId < static_cast<UINT>(REGISTER_ID::COUNTREGISTERS) && ch->GetRegister(static_cast<REGISTER_ID>(regId)) != nullptr)
					{
						ch->GetRegister(static_cast<REGISTER_ID>(regId))->SetEmulated(isEmulated == 1);
						if (isEmulated == 1)
						{
							auto val  = *reinterpret_cast<const float*>(&_inDataBuffer[12]);
							ch->GetRegister(static_cast<REGISTER_ID>(regId))->SetEmulatedValue(val);
						}

					}
				}

		}
	}
		break;
	case LoadCM:
	{
		CRC32* crc = new CRC32();
		crc->ProcessCRC(&_inDataBuffer[4], size-8);
		auto crcR = *reinterpret_cast<const DWORD*>(&_inDataBuffer[size-4]);
		SaveCMData str;
		str.marker = Markers::LoadCM;
		str.size = 4;
		DWORD* answ = reinterpret_cast<DWORD*>(str.data);
		if (size > 64*1024 - 10 - 8) // ������������ ������ ��
			*answ = 0xDEAD1024;
		else if (crcR != crc->GetCRC32())
			*answ = 0xDEADCCCC;
		else
		{
			_drivers->GetGpio()->SetCMWriteProtect(false);
			_commod->WriteConfig(reinterpret_cast<const char*>(&_inDataBuffer[4]), size-8);
			*answ = Markers::AllGood;
			_drivers->GetGpio()->SetCMWriteProtect(true);
			_commod->Init();
		}
		_drivers->GetUSB()->WriteData(&str, 12);
		delete crc;
	}
	break;
	case SetTime:
	{
		Clock::DATE_STRUCT dt;
		Clock::TIME_STRUCT tm;
		tm.sec = _inDataBuffer[4];
		tm.min = _inDataBuffer[5];
		tm.hour = _inDataBuffer[6];
		dt.day = _inDataBuffer[7];
		dt.month = _inDataBuffer[8];
		dt.year = _inDataBuffer[9];
		_drivers->GetClock()->SetDate(dt);
		_drivers->GetClock()->SetTime(tm);
	}
	break;
	case Calibrate:
		if (size-4 == sizeof(DriversIOManager::Calibration))
		{
			if (_workManager->GetCalibrationManager()->SetCalibration(*reinterpret_cast<const DriversIOManager::Calibration*>(&_inDataBuffer[4])))
			{
				_drivers->GetUSB()->SendAnswer(UsbTr::PACKET_RESULT::Correct);
				break;
			}
		}
		_drivers->GetUSB()->SendAnswer(UsbTr::PACKET_RESULT::WrongCrc);
		break;
	case DataByAddressRequest:
	{
		auto address =  *reinterpret_cast<const DWORD*>(&_inDataBuffer[size-4]);
		DWORD data[2];
		data[0] =DataByAddressRequest;
		data[1] = UCU_IORD_32DIRECT(address, 0);
		_drivers->GetUSB()->WriteData(&data, 8);
	}
		break;
	case SetCMNumber:
	{
		auto number =  *reinterpret_cast<const DWORD*>(&_inDataBuffer[size-4]);
		_drivers->GetGpio()->SetCMWriteProtect(false);
		_workManager->GetCommod()->SetCMNumber((WORD)number);
		_drivers->GetGpio()->SetCMWriteProtect(true);
	}
		break;
	case SetBlockNumber:
	{
		auto number =  *reinterpret_cast<const DWORD*>(&_inDataBuffer[size-4]);
		_workManager->GetCalibrationManager()->SetBlockNumber(number);
	}
		break;
	case GetPassport:
		if (_workManager->GetConfiguration()->IsInitialized())
		{
			ChannelCalibrationData ccd;
			ccd.marker = GetPassport;
			ccd.channelPattern = _inDataBuffer[4];
			ccd.channelType = static_cast<IOTYPES>(_inDataBuffer[5]);
			ccd.channelNum = _inDataBuffer[6];
			//DWORD reserv = _inDataBuffer[7];
			auto pattern = _workManager->GetConfiguration()->GetPatternByDeviceNum(ccd.channelPattern);
			if (pattern != nullptr)
			{
				auto ch = (IS_IN((IOTYPES)ccd.channelType) ? static_cast<IChannel*>(pattern->GetInput((IOTYPES)ccd.channelType, ccd.channelNum)) :
											  static_cast<IChannel*>(pattern->GetOutput((IOTYPES)ccd.channelType, ccd.channelNum)));
				if (ch != nullptr && ch->IsHasCalibration() && ch->GetChCalibration()->IsCalibrated())
				{
					for(UINT i = 0; i < ch->GetChCalibration()->GetPointsCount() && i < 128; i++)
						ccd.data[i] = ch->GetChCalibration()->GetPoint(i);
					ccd.count = ch->GetChCalibration()->GetPointsCount();
				}
				else
					ccd.count = 0;
			}
			_drivers->GetUSB()->WriteData(&ccd, ccd.count * 4 + 8);
		}
		break;
	case SetPassport:
	{
		DWORD buff[2];
		buff[0] = SetPassport;
		buff[1] = 0;

		if (_workManager->GetConfiguration()->IsInitialized())
		{
			UINT channelPattern = _inDataBuffer[4];
			IOTYPES channelType = static_cast<IOTYPES>(_inDataBuffer[5]);
			UINT channelNum = _inDataBuffer[6];
			auto pattern = _workManager->GetConfiguration()->GetPatternByDeviceNum(channelPattern);
			if (pattern != nullptr)
			{
				auto ch = (IS_IN(channelType) ? static_cast<IChannel*>(pattern->GetInput(channelType, channelNum)) :
											  static_cast<IChannel*>(pattern->GetOutput(channelType, channelNum)));
				if (ch != nullptr && ch->IsHasCalibration())
				{
					if (_inDataBuffer[7] == 0)
						ch->GetChCalibration()->SetUserPassport(nullptr, 0);
					else
						ch->GetChCalibration()->SetUserPassport(reinterpret_cast<const float*>(&_inDataBuffer[8]), _inDataBuffer[7]);
					ch->GetChCalibration()->CreateKoeffTable();
					_drivers->GetGpio()->SetCMWriteProtect(false);
					_workManager->GetUserSettings()->Save();
					_drivers->GetGpio()->SetCMWriteProtect(true);

					buff[1] = AllGood;
				}
			}
		}
		_drivers->GetUSB()->WriteData(&buff, 8);
	}
		break;
	case GetMeasuredInfo:
		if (_workManager->GetConfiguration()->IsInitialized())
		{
			UINT channelPattern = _inDataBuffer[4];
			IOTYPES channelType = static_cast<IOTYPES>(_inDataBuffer[5]);
			UINT channelNum = _inDataBuffer[6];
			_measuringPointIndex = _inDataBuffer[7];
			auto pattern = _workManager->GetConfiguration()->GetPatternByDeviceNum(channelPattern);
			if (pattern != nullptr)
			{
				_measuringChannel = (IS_IN(channelType) ? static_cast<IChannel*>(pattern->GetInput(channelType, channelNum)) :
											  static_cast<IChannel*>(pattern->GetOutput(channelType, channelNum)));
				if (_measuringChannel != nullptr && _measuringChannel->IsHasCalibration())
				{
					_measuringChannel->GetChCalibration()->StartMeasure();
				} else
				{
					_measuringChannel = nullptr;
					_measuringPointIndex = -1;
				}
			}
		}
		break;
	default:
		Console::TraceLine("�� ���� %08X", marker);
		break;
	}

}



DISPATH_RESULT Diagnostic::ProcessFlag(Flags id)
{

	_drivers->GetPerfomance()->DiagStart();
	auto result = drDispatched;
	switch(id)
	{
	case Flags::MeasurmentComplete:
		if (_measuringChannel != nullptr && _measuringPointIndex != -1)
		{
			ChannelCalibrationData ccd;
			ccd.marker = GetMeasuredInfo;
			ccd.channelPattern = _measuringChannel->GetPatternDeviceNum();
			ccd.channelType = _measuringChannel->GetType();
			ccd.channelNum = _measuringChannel->GetNumber();
			ccd.count = _measuringPointIndex;
			ccd.data[0] = _measuringChannel->GetChCalibration()->GetMeasuredValue();
			_drivers->GetUSB()->WriteData(&ccd, 12);
			_measuringChannel = nullptr;
			_measuringPointIndex = -1;
		}
		break;

	case Flags::SendDiagnostic:
		switch(_mode)
		{
		case DiagnosticMode::NormalDiagnostic:
			FillDiagDataAnalog();
			_drivers->GetUSB()->WriteData(GetDiagDataAnalog(), sizeof(Diagnostic::DiagDataAnalog));
			FillDiagDataDigital();
			_drivers->GetUSB()->WriteData(GetDiagDataDigital(), GetDiagDataDigital()->count*4);
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L3_2, _diagFlashState);
			_diagFlashState = !_diagFlashState;
			break;
		case DiagnosticMode::Oscillograph:
			_drivers->GetUSB()->WriteData(_driversIO->GetADCControl()->GetOscillographData(), sizeof(ADCControl::OscillographData));
			_drivers->GetGpio()->SetLED(Gpio::LEDS::L3_3, _oscillFlashState);
			_oscillFlashState = !_oscillFlashState;
			break;
		default:
			result = drNotDispatched;
			break;
		}
		break;
	case Flags::DataPCParse:
		SetFlag(Flags::DataPCReceived);
		break;
	case Flags::DataPCReceived:
		ParsePacketData();
		break;
	default:
		result = drNotDispatched;
		break;
	}
	_drivers->GetPerfomance()->DiagStop();
	return result;
}
