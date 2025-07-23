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

	return true;
}

void PlayerController::Update()
{
	Update_Input();
	Update_PlayerTransform();
}

void PlayerController::Update_Input()
{
	if (m_Input->GetKeyPress(VK_W))
		m_Position += m_ForwardVec * m_Speed;
	if (m_Input->GetKeyPress(VK_S))
		m_Position -= m_ForwardVec * m_Speed;
	if (m_Input->GetKeyPress(VK_A))
		m_Position -= m_RightVec * m_Speed;
	if (m_Input->GetKeyPress(VK_D))
		m_Position += m_RightVec * m_Speed;
}

void PlayerController::Update_PlayerTransform()
{
	m_Player->SetPos(m_Position);
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