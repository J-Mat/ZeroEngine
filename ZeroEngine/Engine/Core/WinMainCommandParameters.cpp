#include "WinMainCommandParameters.h"

FWinMainCommandParameters::FWinMainCommandParameters(HINSTANCE InhInstance, HINSTANCE InPreInstance, PSTR InCmdLine, int InShowCmd):
	HInstance(InhInstance),
	PrevInstance(InPreInstance),
	CmdLine(InCmdLine),
	ShowCmd(InShowCmd)
{

}

