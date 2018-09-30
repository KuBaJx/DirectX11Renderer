#pragma once

// Linking
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

constexpr float M_PI = 3.141592654f;

#include <d3d11.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
// Custom
#include "CFileSystem.h"

using namespace DirectX;


class CDirect3D
{
public:
	CDirect3D();
	CDirect3D(const CDirect3D&);
	~CDirect3D();

	bool Init(int&, int&, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void PresentScene();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthographicMatrix(XMMATRIX&);

	void GetGPUInfo(char*, int&);

private:
	bool m_VSyncEnabled;
	int m_videoCardMemory;
	char m_videoCardDesc[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterizerState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthographicMatrix;
	CFileSystem* m_fileSystem;
};

