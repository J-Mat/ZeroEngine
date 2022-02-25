#include "WindowsEngine.h"
#if defined(_WIN32)
#include "WindowsMessageProcessing.h"

int FWindowsEngine::PreInit(FWinMainCommandParameters InParameters)
{
	const char LogPath[] = "../log";
	init_log_system(LogPath);
	Engine_Log("Log Init.");
}

int FWindowsEngine::Init()
{

}

int FWindowsEngine::PostInit()
{

}

void FWindowsEngine::Tick()
{

}

int FWindowsEngine::PreExit()
{

}

int FWindowsEngine::Exit()
{

}

int FWindowsEngine::PostExit()
{

}

bool FWindowsEngine::InitWindows(FWinMainCommandParameters Inparameters)
{
	WNDCLASSEX WindowsClass;
	
}

#endif
