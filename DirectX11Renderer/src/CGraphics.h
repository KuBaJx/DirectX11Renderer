#pragma once

#include "CDirect3D.h"

const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 1.0f;

class CGraphics
{
public:
	CGraphics();
	CGraphics(const CGraphics&);
	~CGraphics();

	bool Init(int, int, HWND);
	bool Frame();	
	void Shutdown();

private:
	bool Render();	
private:
	CDirect3D* m_Direct3D;
};

