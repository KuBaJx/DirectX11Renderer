#include "CSystem.h"

CSystem::CSystem()
{
	m_Graphics = nullptr;
	m_Input = nullptr;
}

CSystem::CSystem(const CSystem& other)
{
}

CSystem::~CSystem()
{
}

bool CSystem::Init()
{
	int screenWidth, screenHeight;
	bool result;

	// Init sWidth & sHeight to 0
	screenWidth = 0;
	screenHeight = 0;

	// Init the windows API
	InitWindows(screenWidth, screenHeight);

	// Create a InputClass object -> This object will handle all the inputs
	m_Input = new CInput();
	if (!m_Input)
	{
		return false;
	}

	// Init the Input object
	m_Input->Init();

	// Create the GraphicsClass object -> This object will handle rendering graphics for this app
	m_Graphics = new CGraphics();
	if (!m_Graphics)
	{
		return false;
	}

	// Init the Graphics object
	result = m_Graphics->Init(screenWidth, screenHeight, m_hWnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void CSystem::Shutdown()
{
	// Release the GraphicsClass object
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	// Release the InputClass object
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	// Shutdown the window
	ShutdownWindows();

	return;
}

void CSystem::Run()
{
	MSG msg = {};
	bool done, result;

	// Init the message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until QUIT message
	done = false;
	while (!done)
	{
		// Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If quit signal - quit the application

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do frame processing
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool CSystem::Frame()
{
	bool result;

	// Check if user wants to quit app
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the Graphics object
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK CSystem::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// Check if key has been pressed on the keyboard
		case WM_KEYDOWN:
		{
			// If key was pressed send it's state to m_keys array in InputClass
			m_Input->KeyDown(static_cast<unsigned int>(wParam));
			return 0;
		}

		// Check if key was released and reset it's state in m_keys array
		case WM_KEYUP:
		{
			m_Input->KeyUp(static_cast<unsigned int>(wParam));
			return 0;
		}

		// Any other messages will be send to the default MessageHandler as we don't make use of them
		default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
}

void CSystem::InitWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc = {};
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get pointer to this object
	ApplicationHandle = this;

	// Get the instance of this app
	m_hInstance = GetModuleHandle(NULL);

	// Set the application name
	m_appName = "KEngine v0.0.1.D3D11a";

	// Setup wndclass with defaults
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_appName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register window class
	RegisterClassEx(&wc);

	// Determine clients screen res
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup depending on settings [FULLSCREEN / WINDOWED]
	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change display settings to fullscreen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner (x = 0, y = 0)
		posX = posY = 0;
	}
	else
	{
		// Windowed [800x600] default
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in center of the screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create a window with desired settings and get handle to it
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

	// Bring the window up and set it as main focus
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// Hide the mouse cursor
	ShowCursor(false);

	return;
}

void CSystem::ShutdownWindows()
{
	// Show the mouse cursor
	ShowCursor(true);

	// Fix the display settings when leaving FS mode
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;

	// Remove the app instance
	UnregisterClass(m_appName, m_hInstance);
	m_hInstance = nullptr;

	// Release the pointer to this class
	ApplicationHandle = nullptr;

	return;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
		// Check if window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		// Check if window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		// All other messages will be send to MessageHandler in SystemClass
		default:
		{
			return ApplicationHandle->MessageHandler(hWnd, uMessage, wParam, lParam);
		}
	}
}