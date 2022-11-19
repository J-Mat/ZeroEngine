#pragma once

#include "Core.h"
#include "Application.h"

extern Zero::FApplication* Zero::CreateApplication(HINSTANCE hInst);


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{

	AllocConsole();
	SetConsoleTitle(L"debug console");
	freopen("CONOUT$", "w", stdout);
	std::cout << "Hello Zero Real Time Soft Engine!\n";

	Zero::FLog::Init();

	auto App = Zero::CreateApplication(hInst);

	App->Run();

	delete App;
	return 0;
}

