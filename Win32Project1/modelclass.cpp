#include "modelclass.h"

ModelClass::ModelClass()
{
	m_indexBuffer = 0;
	m_vertexBuffer = 0;
	m_texture = 0;
}

ModelClass::~ModelClass()
{

}

ModelClass::ModelClass(const ModelClass&) 
{

}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	result = InitializeBuffers(device);
	if (!result)
		return false;
	result = LoadTexture(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	ShutdownBuffers();
	ReleaseTexture();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView * ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	cVertex* verts;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertBuffDesc, IndexBuffDesc;
	D3D11_SUBRESOURCE_DATA vertData, indexData;
	HRESULT res;

	ZeroMemory(&vertBuffDesc, sizeof(D3D11_BUFFER_DESC));	
	ZeroMemory(&IndexBuffDesc, sizeof(D3D11_BUFFER_DESC));

	m_vertexCount = 8;

	m_indexCount = 36;

	verts = new cVertex[m_vertexCount];
	indices = new unsigned long[m_indexCount];

	verts[0].position = XMFLOAT3(-5.0f, -5.0f, 0.0f);  // Bottom left front.
	verts[0].texture = XMFLOAT2(0.0f, 1.0f);

	verts[1].position = XMFLOAT3(-5.0f, 5.0f, 0.0f);  // Top left front.
	verts[1].texture = XMFLOAT2(0.0f, 0.0f);

	verts[2].position = XMFLOAT3(5.0f, -5.0f, 0.0f);  // Bottom right front.
	verts[2].texture = XMFLOAT2(1.0f, 1.0f);

	verts[3].position = XMFLOAT3(5.0f, 5.0f, 0.0f);  // Top right front.
	verts[3].texture = XMFLOAT2(1.0f, 0.0f);

	verts[4].position = XMFLOAT3(5.0f, 5.0f, 10.0f);  // Top right back.
	verts[4].texture = XMFLOAT2(0.0f, 0.0f);

	verts[5].position = XMFLOAT3(5.0f, -5.0f, 10.0f);  // bottom right back.
	verts[5].texture = XMFLOAT2(0.0f, 1.0f);

	verts[6].position = XMFLOAT3(-5.0f, 5.0f, 10.0f);  // Top left back.
	verts[6].texture = XMFLOAT2(1.0f, 0.0f);

	verts[7].position = XMFLOAT3(-5.0f, -5.0f, 10.0f);  // Bottom left back.
	verts[7].texture = XMFLOAT2(1.0f, 1.0f);

	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 1; indices[4] = 3; indices[5] = 2;
	indices[6] = 3; indices[7] = 5; indices[8] = 2;
	indices[9] = 3; indices[10] = 4; indices[11] = 5;
	indices[12] = 4; indices[13] = 3; indices[14] = 1;
	indices[15] = 1; indices[16] = 6; indices[17] = 4;
	indices[18] = 1; indices[19] = 0; indices[20] = 7;
	indices[21] = 1; indices[22] = 7; indices[23] = 6;
	indices[24] = 6; indices[25] = 5; indices[26] = 4;
	indices[27] = 6; indices[28] = 7; indices[29] = 5;


	vertBuffDesc.ByteWidth = sizeof(cVertex) * m_vertexCount;
	vertBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertBuffDesc.CPUAccessFlags = 0;
	vertBuffDesc.MiscFlags = 0;
	vertBuffDesc.StructureByteStride = 0;

	vertData.pSysMem = verts;
	vertData.SysMemPitch = 0;
	vertData.SysMemSlicePitch = 0;
	
	res = device->CreateBuffer(&vertBuffDesc, &vertData, &m_vertexBuffer);
	if (FAILED(res))
	{
		return false;
	}

	IndexBuffDesc.ByteWidth = sizeof(indices[0])*m_indexCount;
	IndexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBuffDesc.CPUAccessFlags = 0;
	IndexBuffDesc.MiscFlags = 0;
	IndexBuffDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&IndexBuffDesc, &indexData, &m_indexBuffer);
	if (FAILED(res))
	{
		return false;
	}

	delete[] verts;
	verts = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	UINT offset;
	UINT strides;

	offset = 0;
	strides = sizeof(cVertex);
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &strides, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, offset);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device *device, ID3D11DeviceContext *deviceContext, char *filename)
{
	bool result;

	m_texture = new TextureClass();
	if (!m_texture)
	{
		return false;
	}

	result = m_texture->Initialize(device, deviceContext, filename);
	if (!result) { MessageBox(NULL, L"Error with texture init", NULL, MB_OK); return false; }

	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
}

void ModelClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}
