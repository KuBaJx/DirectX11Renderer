#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pSCmdLine, int nCmdShow)
{
	SystemClass* System;
	bool result;

	// Create the SystemClass object
	System = new SystemClass();
	if (!System)
	{
		return 0;
	}

	// Initializ and run system object
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