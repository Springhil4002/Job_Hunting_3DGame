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
	m_Rotation = XMVectorZero();

	m_CamOffset = XMVectorSubtract(_camera->GetPos(), _player->GetPos());
	m_LastPlayerPos = m_Position;

	m_Speed = 1.0f;
	m_RotateSpeed = XMConvertToRadians(1.5f);

	return true;
}

void PlayerController::Update(float _deltaTime)
{
	Update_Input(_deltaTime);
	Update_PlayerTransform();
}

void PlayerController::Update_Input(float _deltaTime)
{
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