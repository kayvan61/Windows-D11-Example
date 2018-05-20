#include "textureshaderclass.h"

TextureShaderClass::TextureShaderClass()
{
	m_layout = 0;
	m_matrixBuffer = 0;
	m_pixelShader = 0;
	m_samplerState = 0;
	m_vertexShader = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass &)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device *device, HWND window)
{
	return InitializeShader(device, window, L"./Engine/texture.ps", L"./Engine/texture.vs");
}

void TextureShaderClass::Shutdown()
{
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext *deviceContext, int indexCount, XMMATRIX &world, XMMATRIX &view, XMMATRIX &proj, ID3D11ShaderResourceView* texture)
{
	bool result = SetShaderParameters(deviceContext, world, view, proj, texture);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device *device, HWND window, WCHAR *pixelFileName, WCHAR *vertexFileName)
{
	HRESULT result;
	ID3D10Blob *errorMessage, *vertexShaderBuffer, *pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, NULL },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, NULL }
	};
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DCompileFromFile(vertexFileName, NULL, NULL, "TextureVertexShader", "vs_5_0", NULL, NULL, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderMessage(errorMessage, window, pixelFileName);
		}
		else
		{
			MessageBox(window, vertexFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	result = D3DCompileFromFile(pixelFileName, NULL, NULL, "TexturePixelShader", "ps_5_0", NULL, NULL, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderMessage(errorMessage, window, pixelFileName);
		}
		else
		{
			MessageBox(window, pixelFileName, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		MessageBox(window, L"Error with vert", NULL, MB_OK);
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		MessageBox(window, L"Error with pixel", NULL, MB_OK);
		return false;
	}
	numElements = sizeof(polygonDesc) / sizeof(polygonDesc[0]);

	result = device->CreateInputLayout(polygonDesc, numElements, vertexShaderBuffer->GetBufferPointer(),
										vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		MessageBox(window, L"Error with creating the layout", NULL, MB_OK);

		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		MessageBox(window, L"Error with creating matrix buffer", NULL, MB_OK);
		return false;
	}
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_samplerState);
	if (FAILED(result)) {
		MessageBox(window, L"Error with creating sampler state", NULL, MB_OK);
		return false;
	}

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	if (m_layout) {
		m_layout->Release();
		m_layout = 0;
	}
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}
	if (m_samplerState)
	{
		m_samplerState->Release();
		m_samplerState = 0;
	}
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void TextureShaderClass::OutputShaderMessage(ID3D10Blob *error, HWND hwnd, WCHAR *str)
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

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext *deviceContext, XMMATRIX &world, XMMATRIX &view, XMMATRIX &proj, ID3D11ShaderResourceView *texture)
{
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE ms;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	proj = XMMatrixTranspose(proj);

	res = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	if (FAILED(res))
		return false;

	dataPtr = (MatrixBufferType*)ms.pData;
	dataPtr->proj = proj;
	dataPtr->view = view;
	dataPtr->world = world;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);

	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 0, &m_samplerState);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
