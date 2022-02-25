#pragma once


#if defined(_WIN32)
#include "../EngineMinimal.h"
class FWinMainCommandParameters
{
public:
	FWinMainCommandParameters(HINSTANCE InhInstance, HINSTANCE InPreInstance, PSTR InCmdLine, int InShowCmd);
	
	HINSTANCE HInstance;
	HINSTANCE PrevInstance;
	PSTR CmdLine;
	int ShowCmd;
};

#elif 0

#endif
