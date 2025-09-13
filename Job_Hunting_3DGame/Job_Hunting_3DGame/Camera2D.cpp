#include "Camera2D.h"
#include "Debug_New.h"

using namespace DirectX;

Camera2D::Camera2D()
	: m_X(0), m_Y(0), m_ScreenWidth(0), m_ScreenHeight(0)
{
	m_viewMatrix = XMMatrixIdentity();
	m_projMatrix = XMMatrixIdentity();
	m_viewProjMatrix = XMMatrixIdentity();
}

void Camera2D::Init(float _width, float _height)
{
	m_ScreenWidth = _width;
	m_ScreenHeight = _height;

	// ïΩçsìäâeçsóÒ(âÊñ íÜêSÇäÓèÄ)	
	m_projMatrix = XMMatrixOrthographicOffCenterLH(
		-m_ScreenWidth * 0.5f, m_ScreenWidth * 0.5f,
		-m_ScreenHeight * 0.5f, m_ScreenHeight * 0.5f,
		0.0f, 1.0f
	);
	UpdateViewMatrix();
}

void Camera2D::SetPos(float _x, float _y)
{
	m_X = _x;
	m_Y = _y;
	UpdateViewMatrix();
}

void Camera2D::UpdateViewMatrix()
{
	XMVECTOR eyePos = XMVectorSet(m_X, m_Y, -1.0f, 0.0f);
	XMVECTOR target = XMVectorSet(m_X, m_Y, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_viewMatrix = XMMatrixLookAtLH(eyePos, target, up);
	m_viewProjMatrix = m_viewMatrix * m_projMatrix;
}