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
