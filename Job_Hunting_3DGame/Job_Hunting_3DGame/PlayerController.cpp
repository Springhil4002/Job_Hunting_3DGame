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
	
	m_RotateSpeed = XMConvertToRadians(45.0f);	// 回転速度
	m_Acceleration = 5.0f;	// 加速度
	m_Friction = 3.0f;		// 摩擦力
	m_MaxSpeed = 10.0f;		// 最大速度

	return true;
}

void PlayerController::Update(float _deltaTime)
{
	Update_Input(_deltaTime);
	Update_PlayerTransform();
}

void PlayerController::Update_Input(float _deltaTime)
{
	Input_Rotate(_deltaTime);
	Input_Move(_deltaTime);
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

void PlayerController::Input_Rotate(float _deltaTime)
{
	// 回転(Q:左回転、E:右回転)
	if (m_Input->GetKeyPress(VK_A))
		m_Rotation -= XMVectorSet(0, m_RotateSpeed * _deltaTime, 0, 0);
	if (m_Input->GetKeyPress(VK_D))
		m_Rotation += XMVectorSet(0, m_RotateSpeed * _deltaTime, 0, 0);

	// 回転度から方向ベクトルを更新
	XMMATRIX rotMat = XMMatrixRotationY(XMVectorGetY(m_Rotation));
	m_ForwardVec = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMat);
	m_RightVec = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMat);
	m_CamOffset = XMVector3TransformCoord(m_CamOffset, rotMat);
}

void PlayerController::Input_Move(float _deltaTime)
{
	XMVECTOR accelVec = XMVectorZero();

	// 移動(W:前進、S:後退)
	if (m_Input->GetKeyPress(VK_W))
		accelVec += m_ForwardVec;
	if (m_Input->GetKeyPress(VK_S))
		accelVec -= m_ForwardVec;

	if (!XMVector3Equal(accelVec, XMVectorZero()))
	{
		// 複数キー入力で動きが速くならないよう正規化
		accelVec = XMVector3Normalize(accelVec);

		// 加速度を加算
		m_Velocity += accelVec * m_Acceleration * _deltaTime;
	}
	else
	{
		// 入力が無いときは減速
		XMVECTOR fricVec = XMVector3Normalize(m_Velocity) * -1.0f;
		float speed = XMVectorGetX(XMVector3Length(m_Velocity));
		float friction = m_Friction * _deltaTime;

		if (friction > speed)
			m_Velocity = XMVectorZero();
		else
			m_Velocity += fricVec * friction;
	}

	// 最高速度制限
	float curSpeed = XMVectorGetX(XMVector3Length(m_Velocity));
	if (curSpeed > m_MaxSpeed)
	{
		m_Velocity = XMVector3Normalize(m_Velocity) * m_MaxSpeed;
	}

	// 位置に速度を加算
	m_Position += m_Velocity * _deltaTime;
}

XMVECTOR PlayerController::GetPosition() const
{
	return m_Position;
}

XMVECTOR PlayerController::GetRotation() const
{
	return m_Rotation;
}

XMVECTOR PlayerController::GetForwardVec() const
{
	return m_ForwardVec;
}

XMVECTOR PlayerController::GetRightVec() const
{
	return m_RightVec;
}

XMVECTOR PlayerController::GetVelocity() const
{
	return m_Velocity;
}