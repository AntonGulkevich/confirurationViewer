#include "m_math.h"



unsigned int ftou__(float constant)
{
	int pow = ((((*reinterpret_cast<DWORD*>(&constant)) & 0x7F800000) >> 23) -127);
	if (pow & 0x80)
		return 0;
	unsigned int val = (((*reinterpret_cast<int*>(&constant)) & 0x7FFFFF) | 0x800000);
	auto retval = (pow <= 23 ?  (val >> (23 - pow)) : (pow > 54 ? 0 : (val << (pow - 23))));
	return ((*reinterpret_cast<DWORD*>(&constant)) & 0x80000000) ? static_cast<UINT>(-static_cast<int>(retval)) : retval;
}



int NOK(int a, int b, int& gcd) // ���������� ����� �������  /  gcd - ���
{
	auto oldA = a;
	auto oldB = b;
	int x1;
    int y1;
    int x2;
    int y2;
    int q;
    int r;
	int x;
	int y;

    if( b!=0&&a!=0 )
    {
        x2 = 1;
        x1 = 0;
        y2 = 0;
        y1 = 1;
        a = m_abs(a);
        b = m_abs(b);
        while(b>0)
        {
            q = a/b;
            r = a-q*b;
            x = x2-q*x1;
            y = y2-q*y1;
            a = b;
            b = r;
            x2 = x1;
            x1 = x;
            y2 = y1;
            y1 = y;
        }
        gcd = a;
    }
    else
    {
        gcd = 0;
    }
	return oldA * (oldB / gcd);
}

CRC32::CRC32()
{
	const UINT CRC_POLY = 0x20EDB883;
	UINT i, j, r;
	for (i = 0; i < 256; i++)
	{
		for (r = i, j = 8; j; j--)
			r = r & 1? (r >> 1) ^ CRC_POLY: r >> 1;
		table[i] = r;
	}
	_crc32 = 0;
}

void CRC32::ProcessCRC(const void* pData, DWORD nLen)
{
	const UINT CRC_MASK = 0x8DD202EFu;
	auto bdata = reinterpret_cast<const BYTE*>(pData);
	auto crc = _crc32;
	for(UINT i = 0; i < nLen; i++)
	{
		crc = table[static_cast<BYTE>(crc) ^ bdata[i]] ^ crc >> 8;
		crc ^= CRC_MASK;
	}
	_crc32 = crc;
}

BYTE const octTable[378] = 
{
	0,
	128,
	64,
	192,
	32,
	160,
	96,
	224,
	0,
	0,
	16,
	144,
	80,
	208,
	48,
	176,
	112,
	240,
	0,
	0,
	8,
	136,
	72,
	200,
	40,
	168,
	104,
	232,
	0,
	0,
	24,
	152,
	88,
	216,
	56,
	184,
	120,
	248,
	0,
	0,
	4,
	132,
	68,
	196,
	36,
	164,
	100,
	228,
	0,
	0,
	20,
	148,
	84,
	212,
	52,
	180,
	116,
	244,
	0,
	0,
	12,
	140,
	76,
	204,
	44,
	172,
	108,
	236,
	0,
	0,
	28,
	156,
	92,
	220,
	60,
	188,
	124,
	252,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	2,
	130,
	66,
	194,
	34,
	162,
	98,
	226,
	0,
	0,
	18,
	146,
	82,
	210,
	50,
	178,
	114,
	242,
	0,
	0,
	10,
	138,
	74,
	202,
	42,
	170,
	106,
	234,
	0,
	0,
	26,
	154,
	90,
	218,
	58,
	186,
	122,
	250,
	0,
	0,
	6,
	134,
	70,
	198,
	38,
	166,
	102,
	230,
	0,
	0,
	22,
	150,
	86,
	214,
	54,
	182,
	118,
	246,
	0,
	0,
	14,
	142,
	78,
	206,
	46,
	174,
	110,
	238,
	0,
	0,
	30,
	158,
	94,
	222,
	62,
	190,
	126,
	254,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	129,
	65,
	193,
	33,
	161,
	97,
	225,
	0,
	0,
	17,
	145,
	81,
	209,
	49,
	177,
	113,
	241,
	0,
	0,
	9,
	137,
	73,
	201,
	41,
	169,
	105,
	233,
	0,
	0,
	25,
	153,
	89,
	217,
	57,
	185,
	121,
	249,
	0,
	0,
	5,
	133,
	69,
	197,
	37,
	165,
	101,
	229,
	0,
	0,
	21,
	149,
	85,
	213,
	53,
	181,
	117,
	245,
	0,
	0,
	13,
	141,
	77,
	205,
	45,
	173,
	109,
	237,
	0,
	0,
	29,
	157,
	93,
	221,
	61,
	189,
	125,
	253,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	3,
	131,
	67,
	195,
	35,
	163,
	99,
	227,
	0,
	0,
	19,
	147,
	83,
	211,
	51,
	179,
	115,
	243,
	0,
	0,
	11,
	139,
	75,
	203,
	43,
	171,
	107,
	235,
	0,
	0,
	27,
	155,
	91,
	219,
	59,
	187,
	123,
	251,
	0,
	0,
	7,
	135,
	71,
	199,
	39,
	167,
	103,
	231,
	0,
	0,
	23,
	151,
	87,
	215,
	55,
	183,
	119,
	247,
	0,
	0,
	15,
	143,
	79,
	207,
	47,
	175,
	111,
	239,
	0,
	0,
	31,
	159,
	95,
	223,
	63,
	191,
	127,
	255
};

UINT bitReverse (unsigned int x, int log2n) 
{  
	int n = 0;   
	for (int i = 0; i < log2n; i++) 
	{    
		n <<= 1;    
		n |= (x & 1);    
		x >>= 1;  
	}  
	return n;
}