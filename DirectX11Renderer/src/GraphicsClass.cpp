#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	// Initialize Direct3D pointer to NULL
	m_Direct3D = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Init(int screenWidth, int screenHeight, HWND hWnd)
{
	bool result;

	m_Direct3D = new D3DClass();
	if (!m_Direct3D)
	{
		return false;
	}

	result = m_Direct3D->Init(screenHeight, screenWidth, FULL_SCREEN, hWnd, VSYNC_ENABLED, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hWnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
	return;
}

bool GraphicsClass::Frame()
{
	bool result;
	
	// Render the graphics scene
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	// Clear the buffers to begin the scene
	m_Direct3D->BeginScene(.5f, .5f, .5f, 1.f);

	// Present the rendered scene to the screen
	m_Direct3D->PresentScene();

	return true;
}




