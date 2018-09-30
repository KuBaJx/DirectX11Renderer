#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

// Custom headers
#include "CGraphics.h"
#include "CInput.h"


class CSystem
{
public:
	CSystem();
	CSystem(const CSystem&);
	~CSystem();

	bool Init();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCSTR m_appName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	
	CInput* m_Input;
	CGraphics* m_Graphics;
};
// Function proto
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global class pointer
static CSystem* ApplicationHandle = nullptr;

