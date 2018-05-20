#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_layout = 0;
	m_matrixBuffer = 0;
	m_pixelShader = 0;
	m_vertexShader = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&){}

ColorShaderClass::~ColorShaderClass(){}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND window)
{
	bool result;

	result = InitializeShader(device, window, L"./Engine/color.vs", L"./Engine/color.ps");

	return result;
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext *deviceContext, int indexCount, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
		return false;

	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device *device, HWND window, WCHAR *vsFilename, WCHAR *psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,NULL },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,NULL }
	};
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutPutShaderErrorMessage(errorMessage, window, vsFilename);
		}
		else
		{
			MessageBox(window, vsFilename, L"Missing Shader File", MB_OK);
		}
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutPutShaderErrorMessage(errorMessage, window, vsFilename);
		}
		else
		{
			MessageBox(window, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[1]);
	device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;
	
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(cBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	return;
}

void ColorShaderClass::OutPutShaderErrorMessage(ID3D10Blob *error, HWND hwnd, WCHAR *str)
{
	char* compileErrors;
	unsigned long long bufferSize;
	ofstream out;

	compileErrors = (char*)error->GetBufferPointer();
	bufferSize = error->GetBufferSize();

	out.open("shader-error.txt");

	for (int i = 0; i < bufferSize; i++)
	{
		out << compileErrors[i];
	}

	out.close();

	error->Release();
	error = 0;

	MessageBox(hwnd, L"go check shader-error.txt", str, MB_OK);
	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext *deviceContext, XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE ms;
	cBuffer* dataPtr;
	unsigned int bufferNumber;

	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	proj = XMMatrixTranspose(proj);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (cBuffer*)ms.pData;
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;
	
	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext *deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
