#ifndef CDISPLAY_H_
#define CDISPLAY_H_
#include "../types.h"

// Драйвер индикатора
class Display
{
  public:

	enum class POINTS
	{
		TopMiddle,
		BottomMiddle,
		BottomRight,
		Left,
	};

#ifdef _UCU_EMULATION
    Display(UINT baseAddress) {}
#else
	explicit Display(UINT baseAddress_) : baseAddress(baseAddress_) { }
#endif

    void Init();
	void Show();
    void Hide();
    void RunningZero();
    void RunningB();
    void SetValue(const BYTE* text);
	void SetPoint(bool p1, bool p2, bool p3, bool p4);
	void SetPoint(POINTS point, bool val);
	void SetTextByValue(int lValue, bool sign);
	void SetTextByText(const char* text, bool clearPoint = false);
	BYTE GetByteByDigit(BYTE digit) const;
	BYTE GetByteByLetter(char ch) const;
private:
    BYTE value[4];
	bool points[4];
	static BYTE digitTable[16];
	BYTE curZero;
	const UINT baseAddress;
};

#endif /* CDISPLAY_H_ */
