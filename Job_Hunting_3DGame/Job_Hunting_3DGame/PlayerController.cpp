#include "PlayerController.h"

using namespace DirectX;

bool PlayerController::Init(Player* _player, Camera* _camera, Input* _input)
{
	m_Player = _player;
	if (m_Player == nullptr)
		return false;
	m_Camera = _camera;
	if (m_Camera == nullptr)
		return false;
	m_Input = _input;

	m_Position = m_Player->GetPos();
	m_Rotation = m_Player->GetRota();

	m_ForwardVec	= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_RightVec		= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);		
	m_UpVec			= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);		
	m_CamOffset		= XMVectorSubtract(_camera->GetPos(), _player->GetPos());
	m_LastPlayerPos = m_Position;

	m_Yaw = 0.0f;
	m_Speed = 1.0f;
	m_RotateSpeed = XMConvertToRadians(45.0f);

	return true;
}

void PlayerController::Update(float _deltaTime)
{
	Update_Input(_deltaTime);
	Update_PlayerTransform();
}

void PlayerController::Update_Input(float _deltaTime)
{
	// 回転(Q:左回転、E:右回転)
	if (m_Input->GetKeyPress(VK_Q))
		m_Rotation -= XMVectorSet(0, m_RotateSpeed * _deltaTime, 0, 0);
	if (m_Input->GetKeyPress(VK_E))
		m_Rotation += XMVectorSet(0, m_RotateSpeed * _deltaTime, 0, 0);

	// 回転度から方向ベクトルを更新
	XMMATRIX rotMat = XMMatrixRotationY(XMVectorGetY(m_Rotation));
	m_ForwardVec	= XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMat);
	m_RightVec		= XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMat);
	m_CamOffset		= XMVector3TransformCoord(m_CamOffset, rotMat);

	// 移動(W:前進、S:後退、A:左移動、D:右移動)
	if (m_Input->GetKeyPress(VK_W))
		m_Position += m_ForwardVec * m_Speed * _deltaTime;
	if (m_Input->GetKeyPress(VK_S))
		m_Position -= m_ForwardVec * m_Speed * _deltaTime;
	if (m_Input->GetKeyPress(VK_A))
		m_Position -= m_RightVec * m_Speed * _deltaTime;
	if (m_Input->GetKeyPress(VK_D))
		m_Position += m_RightVec * m_Speed * _deltaTime;
}

void PlayerController::Update_PlayerTransform()
{
	// プレイヤーの位置更新
	m_Player->SetPos(m_Position);
	m_Player->SetRota(m_Rotation);

	// カメラの視点更新
	XMVECTOR newCamPos = XMVectorAdd(m_Position, m_CamOffset);
	m_Camera->SetPos(newCamPos);

	// カメラの注視点更新
	XMVECTOR newCamTarget = XMVectorAdd(m_Position, m_ForwardVec * 15.0f);
	m_Camera->SetTarget(newCamTarget);
}

XMVECTOR PlayerController::GetPosition()
{
	return m_Position;
}

XMVECTOR PlayerController::GetRotation()
{
	return m_Rotation;
}

XMVECTOR PlayerController::GetForwardVec()
{
	return m_ForwardVec;
}

XMVECTOR PlayerController::GetRightVec()
{
	return m_RightVec;
}