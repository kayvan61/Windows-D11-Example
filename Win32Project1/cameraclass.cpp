#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_posX = 0;
	m_posY = 0;
	m_posZ = 0;

	m_rotX = 0;
	m_rotY = 0;
	m_rotZ = 0;
}

CameraClass::CameraClass(const CameraClass &)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
}

void CameraClass::SetPosition(XMFLOAT3 pos)
{
	float yaw, pitch, roll;
	XMVECTOR moveVector;
	XMMATRIX rotMatrix;

	pitch = m_rotX * 0.0174532925f;
	roll = m_rotZ * 0.0174532925f;
	yaw = m_rotY * 0.0174532925f;

	moveVector = XMLoadFloat3(&pos);

	rotMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	moveVector = XMVector3TransformCoord(moveVector, rotMatrix);
	

	m_posX += XMVectorGetX(moveVector);
	m_posY += XMVectorGetY(moveVector);
	m_posZ += XMVectorGetZ(moveVector);
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotX = x;
	m_rotY = y;
	m_rotZ = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_posX, m_posY, m_posZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotX, m_rotY, m_rotZ);
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt, forward;
	XMVECTOR upVect, Trans, Rot, frontVect;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	forward = { 1.,0.,0. };
	frontVect = XMLoadFloat3(&forward);

	up = { 0,1.0f,0 };
	upVect = XMLoadFloat3(&up);

	position = { m_posX, m_posY , m_posZ };
	Trans = XMLoadFloat3(&position);

	lookAt = { 0 , 0 , 1 };
	Rot = XMLoadFloat3(&lookAt); 

	//Rot = XMVectorAdd(Trans, frontVect);

	pitch = m_rotX * 0.0174532925f;
	roll = m_rotZ * 0.0174532925f;
	yaw = m_rotY * 0.0174532925f;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	Rot = XMVector3TransformCoord(Rot, rotationMatrix);
	upVect = XMVector3TransformCoord(upVect, rotationMatrix);
	//frontVect = XMVector3TransformCoord(frontVect, rotationMatrix);

	Rot = XMVectorAdd(Rot, Trans);

	m_viewMatrix = XMMatrixLookAtLH(Trans, Rot, upVect);

}

void CameraClass::GetViewMatrix(XMMATRIX &m)
{
	m = m_viewMatrix;
}
