#include "CSystem.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine, int nCmdShow)
{
	CSystem* System;
	bool result;

	// Create the SystemClass object
	System = new CSystem();
	if (!System)
	{
		return 0;
	}

	// Initialize and run system object
	result = System->Init();
	if (result)
	{
		System->Run();
	}

	// Shutdown and release the system object
	System->Shutdown();
	delete System;
	return 0;
}