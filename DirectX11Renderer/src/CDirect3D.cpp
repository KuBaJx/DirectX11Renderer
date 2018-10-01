#include "CDirect3D.h"

CDirect3D::CDirect3D()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterizerState = nullptr;
	m_fileSystem = nullptr;
}

CDirect3D::CDirect3D(const CDirect3D& other)
{
}

CDirect3D::~CDirect3D()
{
}

bool CDirect3D::Init(int& screenWidth, int& screenHeight, bool vsync, HWND hWnd, bool fullscreen, float screenDepth, float screenNear)
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
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
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
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
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

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// Iterate trough all modes and find that one which fits the screen width and height
	// When a match is found store the nominator and denominator of the refresh rate for this particular monitor
	for (iterator = 0; iterator < numModes; iterator++)
	{
		if (displayModeList[iterator].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[iterator].Height == (unsigned int)screenHeight)
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

	// Write gpu info to a file
	m_fileSystem = new CFileSystem("gpuinfo.log");
	if (!m_fileSystem)
	{
		return false;
	}

	std::string msg = "[GPU INFO]: ";

	m_fileSystem->WriteToFile(msg.append(m_videoCardDesc));

	/* As we have Numerator and Denominator store we can release the structs and interfaces
	   which were used to get that informations
	*/

	// Delete file system pointer since we already written gpu info to a file
	delete m_fileSystem;
	m_fileSystem = nullptr;

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

	/* 
	Now we need to set RTV -> 
	This will bind RTV and DepthStencilBuffer to the output render pipeline. 
	This will draw graphics from the pipeline to the backbuffer then we'll swap the
	backbuffer to the front and display rendered image to the user
	*/
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	// Setup the rasterizer description -> this will determine how and what polys will be drawn
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = .0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = .0f;

	// Create a rasterizer state from the filled description above
	result = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	if (FAILED(result))
	{
		return false;
	}

	// Set the rasterizer state
	m_deviceContext->RSSetState(m_rasterizerState);

	// Also we need to setup viewport so Direct3D can clip space coords to the RT space.
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.MinDepth = .0f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = .0f;
	viewport.TopLeftY = .0f;

	// Create the viewport
	m_deviceContext->RSSetViewports(1, &viewport);

	/* 
	Now we'll setup ProjectionMatrix ->
	this will translate 3D coords to previously created 2D viewport space
	*/

	FOV = M_PI / 4.f;
	screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

	// Create the ProjectionMatrix for 3D rendering
	m_projectionMatrix = XMMatrixPerspectiveFovLH(FOV, screenAspect, screenNear, screenDepth);

	/* 
	WorldMatrix -> this will translate vertices from our 3D models to 3D space
	also this will be used to scale, translate, rotate our object
	*/

	// Init the WorldMatrix to the indentity matrix
	m_worldMatrix = XMMatrixIdentity();

	/*
	Last thing is OrthographicMatrix -> this matrix will be used to render GUI and 2D elements
	so we can skip 3D rendering which will save resources
	*/

	m_orthographicMatrix = XMMatrixOrthographicLH(static_cast<float>(screenWidth), static_cast<float>(screenHeight), screenNear, screenDepth);

	return true;
}

void CDirect3D::Shutdown()
{
	// Release all pointers used in Init function.
	// !!! TODO: First we need to switch to the WINDOWED mode before releasing anything !!!

	// Set to windowed
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, nullptr);
	}

	if (m_rasterizerState)
	{
		m_rasterizerState->Release();
		m_rasterizerState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}

	return;
}

void CDirect3D::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];
	float modifier = (sin(static_cast<float>(timeGetTime())) / 2) + .5f;


	// Setup the color to clear the buffer to [Sets color buffer]
	color[0] = red + modifier;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer before rendering new image
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	return;
}

void CDirect3D::PresentScene()
{
	// Present the back buffer to the screen since rendering is complete
	if (m_VSyncEnabled)
	{
		// Lock the refresh rate if VSYNC
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present ASAP -> unlimited framerate / refresh rate
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* CDirect3D::GetDevice() const
{
	return m_device;
}

ID3D11DeviceContext* CDirect3D::GetDeviceContext() const
{
	return m_deviceContext;
}

/// Helper functions that give copies of projection, world, orthographic matrices
/// Most shaders will need these matrices for rendering so these helpers will come handy

void CDirect3D::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void CDirect3D::GetWorldMatrix(XMMATRIX& worldMatrix) 
{
	worldMatrix = m_worldMatrix;
	return;
}

void CDirect3D::GetOrthographicMatrix(XMMATRIX& orthographicMatrix)
{
	orthographicMatrix = m_orthographicMatrix;
	return;
}

/// This helper will return name and VRAM of the GPU by reference 
/// This will help if debugging is needed for different configs
void CDirect3D::GetGPUInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDesc);
	memory = m_videoCardMemory;
	return;
}
