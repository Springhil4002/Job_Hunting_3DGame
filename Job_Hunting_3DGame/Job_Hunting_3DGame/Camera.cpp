#include "Camera.h"

Camera::Camera()
	: m_eye(DirectX::XMVectorSet(0.0f,0.0f,-5.0f,1.0f)),
	  m_target(DirectX::XMVectorZero()),
	  m_up(DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f))
{
	UpdateViewMatrix();
	SetPerspective(DirectX::XMConvertToRadians(60.0f), 1.0f, 0.1f, 1000.0f);
}

void Camera::SetPos(const DirectX::XMVECTOR& _eye)
{
	m_eye = _eye;
	UpdateViewMatrix();
}

void Camera::SetTarget(const DirectX::XMVECTOR& _target)
{
	m_target = _target;
	UpdateViewMatrix();
}

void Camera::SetUp(const DirectX::XMVECTOR& _up)
{
	m_up = _up;
	UpdateViewMatrix();
}

void Camera::SetPerspective(float _fovY, float _aspect, float _nearZ, float _farZ)
{
	m_proj = DirectX::XMMatrixPerspectiveFovRH(_fovY, _aspect, _nearZ, _farZ);
}

const DirectX::XMVECTOR& Camera::GetPos() const 
{
	return m_eye; 
}

const DirectX::XMVECTOR& Camera::GetTarget() const
{
	return m_target; 
}

const DirectX::XMVECTOR& Camera::GetUp() const
{
	return m_up; 
}

const DirectX::XMMATRIX& Camera::GetViewMatrix() const
{
	return m_view;
}

const DirectX::XMMATRIX& Camera::GetProjMatrix() const
{
	return m_proj;
}

void Camera::UpdateViewMatrix()
{
	m_view = DirectX::XMMatrixLookAtRH(m_eye, m_target, m_up);
}

void Camera::Translate(const DirectX::XMVECTOR& _offset)
{
	m_eye = DirectX::XMVectorAdd(m_eye, _offset);
	m_target = DirectX::XMVectorAdd(m_target, _offset);
	UpdateViewMatrix();
}

void Camera::MoveForward(float _distance)
{
	DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_target, m_eye));
	Translate(DirectX::XMVectorScale(forward, _distance));
}

void Camera::MoveBack(float _distance)
{
	MoveForward(-_distance);
}

void Camera::MoveRight(float _distance)
{
	DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_target, m_eye));
	DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_up, forward));
	Translate(DirectX::XMVectorScale(right, _distance));
}

void Camera::MoveLeft(float _distance)
{
	MoveRight(-_distance);
}