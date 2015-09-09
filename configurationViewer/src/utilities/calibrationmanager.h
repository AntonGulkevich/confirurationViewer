/*
 * calibrationmanager.h
 *
 *  Created on: 14 ���. 2015 �.
 *      Author: Youbrin_A
 */

#ifndef CALIBRATIONMANAGER_H_
#define CALIBRATIONMANAGER_H_
#include "../driversiomanager.h"

class DriverManager;

class CalibrationManager
{
private:
	DriverManager* _drivers;
	DriversIOManager* _driversIO;

	static const UINT traceLength = 100;
	// ��������� ����������
	DriversIOManager::Calibration _calibration;
	UINT _calibrateState;
	short _calib[DriversIOManager::SCTOutCount*2][traceLength];

	// �������� ������������ ����������
	bool CheckCalibration(DriversIOManager::Calibration& calib) const;
	void CalcAvg(int gainIndex);
	void CalcAvgSCT(int number, int regNum);
	void CalcScaleSCT(int number);
	void CalcAvgNPT(int number);
	void SaveCalibration();
	void ClearCalibration();

public:
	CalibrationManager(DriverManager* drivers, DriversIOManager* _driversIO);
	virtual ~CalibrationManager() {}

	void LoadCalibration();
	void ApplyCalibration();

	//����������
	DriversIOManager::Calibration& GetCalibration();
	DWORD GetBlockNumber() const	{ return _calibration.blockNumber; }
	// ��������� ������������ ���������� � ���������� ��, ���������� false ���� ���������� �� ���������
	bool SetCalibration(DriversIOManager::Calibration& calib);
	void SetBlockNumber(DWORD blockNumber);
	void StartCalibrationIn();
	bool IsCalibrationInProgress() const	{ return _calibrateState > 0; }
	void ProcessCalibration();


};

#endif /* CALIBRATIONMANAGER_H_ */
