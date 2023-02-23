#include <iostream> 
#include "StringUtils.h"
#include "ZConfig.h"
#include "Log.h"
#include "mini/ini.h"
#include "Utils.h"
#include "tinyxml2.h"
#include "json.hpp"
#include <string>
#include <stdio.h>
#include <fstream>
#include "Math/ZMath.h"
using namespace std;

int main()
{
	/*
	unsigned int v;  // 32-bit value to find the log2 of 
	while (cin >> v)
	{
		unsigned int r; // result of log2(v) will go here
		unsigned int shift;

		r = (v > 0xFFFF) << 4; v >>= r;
		shift = (v > 0xFF) << 3; v >>= shift; r |= shift;
		shift = (v > 0xF) << 2; v >>= shift; r |= shift;
		shift = (v > 0x3) << 1; v >>= shift; r |= shift;
		r |= (v >> 1);


		cout << r << endl;
	}
	*/
	unsigned long mask = 0x1000;
	unsigned long index;
	unsigned char isNonzero;

	cout << "Enter a positive integer as the mask: " << flush;
	while (cin >> mask)
	{
		isNonzero = _BitScanReverse(&index, mask + mask - 1);
		if (isNonzero)
		{
			cout << "--Mask: " << mask << " Index: " << index << endl;
			isNonzero = _BitScanReverse(&index, mask);
			cout << "--Mask: " << mask << " Indes: " << index << endl;
		}
		else
		{
			cout << "No set bits found.  Mask is zero." << endl;
		}
	}
	return 0;
}