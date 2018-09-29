#include "D3DClass.h"

D3DClass::D3DClass()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterizerState = nullptr;
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Init(int& screenWidth, int& screenHeight, bool vsync, HWND hWnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPointer;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	D3D11_VIEWPORT viewport;
	float FOV, screenAspect;
	unsigned int numModes, iterator, numerator, denominator;
	unsigned long long stringLength;
	int error;

	// vsync
	m_VSyncEnabled = vsync;

	// Create DirectX graphics interface factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)factory);
	if (FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary gpu
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// Enum the primary monitor
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Get the number of modes that meets this requirements: DXGI_FORMAT_R8G8B8A8_UNORM display format
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	// Create a list and populate it with all possible display modes for this Monitor/GPU combo
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Iterate trough all modes and find that one which fits the screen width and height
	// When a match is found store the nominator and denominator of the refresh rate for this particular monitor
	for (iterator = 0; iterator < numModes; iterator++)
	{
		if (displayModeList[iterator].Width == static_cast<unsigned int>(screenWidth))
		{
			if (displayModeList[iterator].Height == static_cast<unsigned int>(screenHeight))
			{
				numerator = displayModeList[iterator].RefreshRate.Numerator;
				denominator = displayModeList[iterator].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter(GPU) description and VRAM size
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store VRAM in MB
	m_videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the GPU to char** 
	error = wcstombs_s(&stringLength, m_videoCardDesc, 128, adapterDesc.Description, 128);
	if (error != NULL)
	{
		return false;
	}

	/* As we have Numerator and Denominator store we can release the structs and interfaces
	   which were used to get that informations
	*/

	// Release displayModeList
	delete[] displayModeList;
	displayModeList = nullptr;

	// Release the adapter output
	adapterOutput->Release();
	adapterOutput = nullptr;

	// Release the adapter
	adapter->Release();
	adapter = nullptr;

	// Release the factory
	factory->Release();
	factory = nullptr;

	// Now, we have refresh rate we can start initializing DirectX

	// Init SwapChainDesc
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set single back buffer
	swapChainDesc.BufferCount = 1;

	// Set width and height of the back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set 32BIT(Regular) surface for the back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer
	if (m_VSyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		// If VSYNC is OFF
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the window handle where render to
	swapChainDesc.OutputWindow = hWnd;

	// Turn MS(Multisampling) OFF
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Handle FULLSCREEN or WINDOWED mode
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set scan ordering and scaling to UNSPECIFIED
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer content after presenting to a front buffer
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Any advanced flags
	swapChainDesc.Flags = NULL;

	// Set the feature level (minimum DirectX version)
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/* 
	Create the SwapChain, 
	Direct3D Device and Direct3D Device Context
	[Those are very important interfaces for handling all Direct3D functions]
	*/

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device,
		nullptr, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the backbuffer
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)(&backBufferPointer));
	if (FAILED(result))
	{
		return false;
	}

	// Create a RTV(RenderTargetView) with back buffer pointer attached to it
	result = m_device->CreateRenderTargetView(backBufferPointer, nullptr, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Release the pointer to the back buffer since we don't need it anymore
	backBufferPointer->Release();
	backBufferPointer = nullptr;

	/* 
	Now we need to setup DepthBufferDesc -> so our polys can be rendered properly in 3D space
	In the same time we need to attach StencilBuffer to our DepthBuffer -> so we can achieve effects
	like motion blur or volumetric shadows, etc...
	*/

	// Init the desc of DepthBuffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Setup desc of the DepthBuffer
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Now let's create Depth buffer using out description
	result = m_device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the desc of DepthStencil
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Setup of the desc of the stencil state
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	
	depthStencilDesc.DepthEnable = true;
	// Must be HEX
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// If pixel is front facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// IF pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// When description is filled we can create depth stencil state
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	/* 
	Next step is to create DepthStencilView description so Direct3D knows
	to use DepthBuffer as a DepthStencil texture
	*/

	// Init DepthStencilView struct
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the DepthStencilView
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}
}

void D3DClass::Shutdown()
{

}