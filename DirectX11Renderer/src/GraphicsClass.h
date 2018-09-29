#pragma once

#include "D3DClass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 1.0f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Init(int, int, HWND);
	bool Frame();
	void Shutdown();

private:
	bool Render();	
private:
	D3DClass* m_Direct3D;
};

