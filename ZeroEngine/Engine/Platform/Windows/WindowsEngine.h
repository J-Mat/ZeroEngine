#pragma once

#if defined(_WIN32)
#include "../../Core/Engine.h"

class FWindowsEngine : public FEngine
{
public:
	virtual int PreInit(FWinMainCommandParameters InParameters);

	virtual int Init();
	virtual int PostInit();

	virtual void Tick();

	virtual int PreExit();
	virtual int Exit();
	virtual int PostExit();
	

private:
	bool InitWindows(FWinMainCommandParameters Inparameters);
};
#endif