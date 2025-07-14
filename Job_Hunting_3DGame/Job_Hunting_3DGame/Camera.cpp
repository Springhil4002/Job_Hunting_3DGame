#include "Camera.h"

using namespace DirectX;

Camera::Camera()
	: m_eye(XMVectorSet(0.0f,15.0f,0.0f,1.0f)),
	  m_target(XMVectorSet(0.0f,0.0f,50.0f,0.0f)),
	  m_up(XMVectorSet(0.0f,1.0f,0.0f,0.0f))
{
	UpdateViewMatrix();
	SetPerspective(XMConvertToRadians(45.0f), 1.0f, 0.1f, 1000.0f);
}

void Camera::SetPos(const XMVECTOR& _eye)
{
	m_eye = _eye;
	UpdateViewMatrix();
}

void Camera::SetTarget(const XMVECTOR& _target)
{
	m_target = _target;
	UpdateViewMatrix();
}

void Camera::SetUp(const XMVECTOR& _up)
{
	m_up = _up;
	UpdateViewMatrix();
}

void Camera::SetPerspective(float _fovY, float _aspect, float _nearZ, float _farZ)
{
	m_proj = XMMatrixPerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
}

const XMVECTOR& Camera::GetPos() const 
{
	return m_eye; 
}

const XMVECTOR& Camera::GetTarget() const
{
	return m_target; 
}

const XMVECTOR& Camera::GetUp() const
{
	return m_up; 
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return m_view;
}

const XMMATRIX& Camera::GetProjMatrix() const
{
	return m_proj;
}

const XMVECTOR& Camera::GetForward() const
{
	return XMVector3Normalize(XMVectorSubtract(m_target, m_eye));
}

void Camera::UpdateViewMatrix()
{
	m_view = XMMatrixLookAtLH(m_eye, m_target, m_up);
}

void Camera::Translate(const DirectX::XMVECTOR& _offset)
{
	m_eye = XMVectorAdd(m_eye, _offset);
	m_target = XMVectorAdd(m_target, _offset);
	UpdateViewMatrix();
}

void Camera::MoveForward(float _distance)
{
	XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(m_target, m_eye));
	Translate(XMVectorScale(forward, _distance));
}

void Camera::MoveBack(float _distance)
{
	MoveForward(-_distance);
}

void Camera::MoveRight(float _distance)
{
	XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(m_target, m_eye));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(m_up, forward));
	Translate(XMVectorScale(right, _distance));
}

void Camera::MoveLeft(float _distance)
{
	MoveRight(-_distance);
}

void Camera::MoveUp(float _distance)
{
	XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	Translate(XMVectorScale(worldUp, _distance));
}

void Camera::MoveDown(float _distance)
{
	MoveUp(-_distance);
}

void Camera::Rotate_Yaw(float _angle)
{
	// カメラ位置から注視点への方向ベクトル
	XMVECTOR forward = XMVectorSubtract(m_target, m_eye);
	
	// ワールドのY軸を基準に
	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX rot = XMMatrixRotationAxis(worldUp, _angle);
	
	// 視点ベクトルを回転
	forward = XMVector3TransformNormal(forward, rot);
	
	// 視点を基準に注視点を再設定
	m_target = XMVectorAdd(m_eye, forward); 
	
	// 上ベクトルも再計算
	m_up = XMVector3TransformNormal(m_up, rot);
	
	UpdateViewMatrix();
}
void Camera::Rotate_Pitch(float _angle)
{
	XMVECTOR forward = XMVectorSubtract(m_target, m_eye);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(m_up, forward)); 
	XMMATRIX rot = XMMatrixRotationAxis(right, _angle);
	forward = XMVector3TransformNormal(forward, rot);
	// 視点を基準に注視点を再設定
	m_target = XMVectorAdd(m_eye, forward); 

	// upベクトルも回転（チルト）
	m_up = XMVector3TransformNormal(m_up, rot);

	UpdateViewMatrix();
}