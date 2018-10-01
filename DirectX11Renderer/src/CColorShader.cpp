#include "CColorShader.h"

CColorShader::CColorShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_inputLayout = nullptr;
	m_matrixBuffer = nullptr;

}

CColorShader::CColorShader(const CColorShader& other)
{
}

CColorShader::~CColorShader()
{
}

bool CColorShader::Init(ID3D11Device* device, HWND hWnd)
{
	bool result;

	// Init Vertex and Pixel shaders
	result = InitShader(device, hWnd, (WCHAR*)"../res/color.vs", (WCHAR*)"../res/color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void CColorShader::Shutdown()
{
	// Shutdown Vertex and Pixel shader and related objects
	ShutdownShader();

	return;
}

bool CColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	// Set the shader params that it will use for rendering
	result = SetShaderParams(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Render prepared buffers with the shaders
	RenderShader(deviceContext, indexCount);

	return true;
}

bool CColorShader::InitShader(ID3D11Device* device, HWND hWnd, WCHAR* vertexSource, WCHAR* pixelSource)
{
	HRESULT result;
	ID3D10Blob* errMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC elementLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errMsg = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// Compile the VertexShader source code
	result = D3DCompileFromFile(vertexSource, nullptr, nullptr, "ColorVertexSource", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errMsg);
	if (FAILED(result))
	{
		// If the shader compilation failed log error message
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg, hWnd, vertexSource);
		}
		else
		{
			// If there was no errors - file was not found
			MessageBox(hWnd, (LPSTR)vertexSource, "Vertex shader file missing!", MB_OK);
		}
		return false;
	}

	result = D3DCompileFromFile(pixelSource, nullptr, nullptr, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errMsg);
	if (FAILED(result))
	{
		if (errMsg)
		{
			OutputShaderErrorMsg(errMsg, hWnd, pixelSource);
		}
		else
		{
			MessageBox(hWnd, (LPSTR)pixelSource, "Pixel shader file missing!", MB_OK);
		}

		return false;
	}

	// Create the VertexShader from the buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the PixelShader from the buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the VertexInputLayout description -> this needs to match the VertexType struct in the ModelClass and in the shader
	elementLayout[0].SemanticName = "POSITION";
	elementLayout[0].SemanticIndex = 0;
	elementLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	elementLayout[0].InputSlot = 0;
	elementLayout[0].AlignedByteOffset = 0;
	elementLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	elementLayout[0].InstanceDataStepRate = 0;

	elementLayout[1].SemanticName = "COLOR";
	elementLayout[1].SemanticIndex = 0;
	elementLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	elementLayout[1].InputSlot = 0;
	elementLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // Automatic offset detection
	elementLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; 
	elementLayout[1].InstanceDataStepRate = 0;
}

