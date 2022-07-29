#pragma once


#include "Core.h"
#include "Application.h"

extern Zero::FApplication* Zero::CreateApplication(HINSTANCE hInst);

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	Zero::FLog::Init();

	auto App = Zero::CreateApplication(hInst);

	App->Run();

	delete App;
}

