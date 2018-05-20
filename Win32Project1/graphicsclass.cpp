////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"



GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
	m_Texture = 0;
	m_Camera = 0;
	m_Model = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_Direct3D = new D3DClass;

	if (!m_Direct3D)
	{
		return false;
	}

	m_Texture = new TextureShaderClass;
	m_Camera = new CameraClass;
	m_Model = new ModelClass;
	if (!m_Texture)
		return false;
	if (!m_Camera)
		return false;
	if (!m_Model)
		return false;

	bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Couldnt init DirectX", L"ERROR", MB_OK);
		return false;
	}
	result = m_Texture->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Error getting Texture", NULL, MB_OK);
		return false;
	}

	m_Camera->SetPosition(0, 0, -5);

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "./Engine/data/stone01.tga");
	if (!result)
	{
		MessageBox(hwnd, L"Error getting Texture", NULL, MB_OK);
		return false;
	}
	return true;
}


void GraphicsClass::Shutdown()
{
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	// Release the color shader object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

CameraClass * GraphicsClass::GetCamera()
{
	return m_Camera;
}


bool GraphicsClass::Render()
{
	XMMATRIX world, view, proj;
	bool result;

	m_Direct3D->BeginScene(0, 0, 0, 1);

	m_Camera->Render();

	m_Direct3D->GetWorldMatrix(world);
	m_Direct3D->GetProjectionMatrix(proj);
	m_Camera->GetViewMatrix(view);

	m_Model->Render(m_Direct3D->GetDeviceContext());

	//m_Camera->SetPosition(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z );

	result = m_Texture->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), world, view, proj, m_Model->GetTexture());
	if (!result)
		return false;

	m_Direct3D->EndScene();

	return true;
}