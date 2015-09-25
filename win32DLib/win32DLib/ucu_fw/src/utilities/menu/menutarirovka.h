/*
 * menutarirovka.h
 *
 *  Created on: 15 сент. 2015 г.
 *      Author: Youbrin_A
 */

#ifndef MENUTARIROVKA_H_
#define MENUTARIROVKA_H_
#include "menuitem.h"

class ChannelCalibration;
class MenuInputValue;

class MenuTarirovka  : public MenuItem
{
private:
	enum class STAGES
	{
		Header,
		InputPointsCount,
		PointHeader,
		InputPointValue,
		SourseSelect,
		Measuring,
		Error,
		Finish,
	};

	ChannelCalibration* _channel;
	STAGES _stage;
	float _currentValue;
	MenuInputValue* _mInputValue;
	UINT _currentPointNum;
	UINT _totalPointsCount;
	bool _isRealMeasure;

	char _headerValue[5];
	float _tarArray[128];
	bool _tarIsRealValue[64];
	UINT _errorCode;

	UINT CheckTarirovka();
	void QSortLinearTable(int l,int r);


public:


	MenuTarirovka(ChannelCalibration* channel, const char* name, const BYTE points = 0);
	virtual ~MenuTarirovka() {}


	bool IsActive() override { return _stage != STAGES::Header; }
	const char* GetElementName() override;

	void Enter() override;
	bool Exit() override;

	void Prev() override;
	void Next() override;
};

#endif /* MENUTARIROVKA_H_ */
