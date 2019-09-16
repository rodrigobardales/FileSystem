#include "bitmap.h"

void Bitmap::printBitMap()
{
	const int SHIFT = 8 * sizeof( char ) - 1;
	const char MASK = 1 << SHIFT;

	for(int i = 0; i < size/8; i++)
	{
		char value;
		value = bitMap[i];
		for(int c = 1; c <= SHIFT + 1; c++ )
		{
			cout << (value & MASK ? '1' : '0');
			value <<= 1;

			if ( c % 8 == 0 )
				cout << ' '; 
		}
	}
	cout << endl;

}

void Bitmap::setOn(int nBlock)
{
	int positionByte = nBlock/8;
	int iniPosition = (nBlock/8) * 8;

	for(int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
	{
		if(i == nBlock)
		{
			bitMap[positionByte] |= 1 << x; 
			break;
		}
	}
}

void Bitmap::setOff(int nBlock)
{
	int positionByte = nBlock/8;
	int iniPosition = (nBlock/8) * 8;

	for(int i = iniPosition, x = 7; i < (positionByte * 8) + 8; i++, x--)
	{
		if(i == nBlock)
		{
			bitMap[positionByte] &= ~(1 << x); 
			break;
		}
	}
}

