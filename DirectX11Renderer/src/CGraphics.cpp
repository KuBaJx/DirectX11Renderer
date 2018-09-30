#include "CGraphics.h"

CGraphics::CGraphics()
{
	// Initialize Direct3D pointer to NULL
	m_Direct3D = nullptr;
}

CGraphics::CGraphics(const CGraphics& other)
{
}

CGraphics::~CGraphics()
{
}

bool CGraphics::Init(int screenWidth, int screenHeight, HWND hWnd)
{
	bool result;

	m_Direct3D = new CDirect3D();
	if (!m_Direct3D)
	{
		return false;
	}

	result = m_Direct3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hWnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hWnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	return true;
}

void CGraphics::Shutdown()
{
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}
	return;
}

bool CGraphics::Frame()
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

bool CGraphics::Render()
{
	// Clear the buffers to begin the scene
	m_Direct3D->BeginScene(.5f, .3f, .2f, 1.f);

	// Present the rendered scene to the screen
	m_Direct3D->PresentScene();

	return true;
}




