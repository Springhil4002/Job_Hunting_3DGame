#include "PlayerController.h"
#include "Debug_New.h"

using namespace DirectX;

bool PlayerController::Init(
	Player* _player, WaterMesh* _waterMesh,
	Camera* _camera, Input* _input)
{
	if (!_player || !_waterMesh || !_camera || !_input) 
		return false;
	m_Player = _player;
	m_WaterMesh = _waterMesh;
	m_Camera = _camera;
	m_Input = _input;
		
	Init_Param();
	return true;
}

void PlayerController::Update(float _deltaTime)
{
	Update_Input(_deltaTime);
	Update_Buoyancy(_deltaTime);
	Update_PlayerTransform(_deltaTime);
	Update_Emitter(_deltaTime);
}

void PlayerController::Draw()
{
	if (m_Emitter_Splash)
	{
		m_Emitter_Splash->Draw();
	}
}

void PlayerController::Uninit()
{
	// エミッタの終了処理
	if (m_Emitter_Splash)
		m_Emitter_Splash.reset();

	// ポインタクリア
	m_Player = nullptr;
	m_WaterMesh = nullptr;
	m_Camera = nullptr;
	m_Input = nullptr;

	// ベクトル初期化
	m_Position = XMVectorZero();
	m_Rotation = XMVectorZero();
	m_ForwardVec = XMVectorZero();
	m_RightVec = XMVectorZero();
	m_UpVec = XMVectorZero();
	m_CamOffset = XMVectorZero();
	m_LastPlayerPos = XMVectorZero();
	m_Velocity = XMVectorZero();
	m_VelocityY = XMVectorZero();
}

void PlayerController::Init_Param()
{
	if (!m_Player || !m_Camera) return;

	m_Position = m_Player->GetPos();
	m_Rotation = m_Player->GetRota();

	m_ForwardVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_RightVec = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	m_UpVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_CamOffset = XMVectorSubtract(m_Camera->GetPos(), m_Player->GetPos());
	m_InitCamOffset = m_CamOffset;
	m_LastPlayerPos = m_Position;

	m_RotateSpeed = XMConvertToRadians(45.0f);	// 回転速度
	m_Acceleration = 5.0f;	// 加速度
	m_Friction = 3.0f;		// 摩擦力
	m_MaxSpeed = 40.0f;		// 最大速度

	m_Gravity = -9.8f;		// 重力
	m_Buoyancy = 30.0f;		// 浮力
	m_WaterDamping = 4.0f;	// 水中減衰
	m_PlayerOffsetY = 3.0f; // プレイヤーの高さオフセット
	m_FollowSpeed = 40.0f;	// カメラの追従速度

	m_Emitter_Splash = std::make_unique<ParticleEmitter_Splash>(m_Camera);
	m_Emitter_Splash->Init();
}

void PlayerController::Update_Input(float _deltaTime)
{
	if (!m_Played) return;
	Input_Rotate(_deltaTime);
	Input_Move(_deltaTime);
}

void PlayerController::Update_Buoyancy(float _deltaTime)
{
	if (!m_WaterMesh) return;

	float fx = static_cast<float>(XMVectorGetX(m_Position));
	float fz = static_cast<float>(XMVectorGetZ(m_Position));
	float y = XMVectorGetY(m_Position) - m_PlayerOffsetY;
	

	// 水面の波の高さを取得
	float waterHeight = m_WaterMesh->GetWaveHeight(fx, fz, _deltaTime);
	float depth = waterHeight - y;

	// 水中にいるなら
	if (depth > 0.0f)
	{
		float buoyancyForce = m_Buoyancy * depth;
		float damping = m_WaterDamping * XMVectorGetY(m_VelocityY);
		
		float vy = XMVectorGetY(m_VelocityY);
		vy += (buoyancyForce + m_Gravity - damping) * _deltaTime;
		
		m_VelocityY = XMVectorSet(0, vy, 0, 0);
	}
	else
	{
		// 水面上なら重力のみ
		m_VelocityY += XMVectorSet(0, m_Gravity * _deltaTime, 0, 0);
	}

	m_Position += m_VelocityY * _deltaTime;
}

void PlayerController::Update_PlayerTransform(float _deltaTime)
{
	// プレイヤーの位置更新
	m_Player->SetPos(m_Position);
	m_Player->SetRota(m_Rotation);

	// カメラの理想位置
	XMVECTOR idealCamPos = XMVectorAdd(m_Position, m_CamOffset);
	// 現在のカメラ位置
	XMVECTOR currentCamPos = m_Camera->GetPos();

	// 補間率
	//float interp = std::clamp(m_FollowSpeed * _deltaTime, 0.0f, 1.0f);

	float interp = 1.0f - expf(-m_FollowSpeed * _deltaTime);

	// カメラ位置を補間
	XMVECTOR newCamPos = XMVectorLerp(currentCamPos, idealCamPos, interp);
	m_Camera->SetPos(newCamPos);

	// カメラの注視点更新
	XMVECTOR newCamTarget = XMVectorAdd(m_Position, m_ForwardVec * 15.0f);
	m_Camera->SetTarget(newCamTarget);
}

void PlayerController::Update_Emitter(float _deltaTime)
{
	if (!m_Emitter_Splash) return;

	bool isMoving = false;

	XMVECTOR horizontalVelocity = XMVectorSet(XMVectorGetX(m_Velocity), 0, XMVectorGetZ(m_Velocity), 0);
	if (XMVectorGetX(XMVector3Length(horizontalVelocity))> 0.1f)
	{
		isMoving = true;
	}
	
	const float splashOffsetY = -1.0f;
	// Playerの先頭部分(中心座標＋進行方向のオフセット)
	const float offsetLength = 2.0f;
	XMVECTOR spawnPos = m_Position + m_ForwardVec * offsetLength;
	spawnPos = XMVectorAdd(spawnPos, XMVectorSet(0, splashOffsetY, 0, 0));

	m_Emitter_Splash->Update(_deltaTime, spawnPos, m_RightVec, isMoving);
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
	m_CamOffset = XMVector3TransformCoord(m_InitCamOffset, rotMat);
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
		float speed = XMVectorGetX(XMVector3Length(m_Velocity));
		if (speed > 0.0001f)
		{
			XMVECTOR fricVec = XMVector3Normalize(m_Velocity) * -1.0f;
			float friction = m_Friction * _deltaTime;
			if (friction > speed)
				m_Velocity = XMVectorZero();
			else
				m_Velocity += fricVec * friction;
		}
		else
		{
			m_Velocity = XMVectorZero();
		}
	}

	// 慣性旋回,速度ベクトルを補間しながら向きに合わせる
	if (!XMVector3Equal(m_Velocity, XMVectorZero()))
	{
		XMVECTOR dir = XMVector3Normalize(m_Velocity);
		XMVECTOR targetDir = XMVector3Normalize(m_ForwardVec);

		// 回転の補間
		float turnRate = 2.0f * _deltaTime;
		XMVECTOR newDir = XMVectorLerp(dir, targetDir, turnRate);
		newDir = XMVector3Normalize(newDir);

		// 速度を維持して、方向を更新
		float speed = XMVectorGetX(XMVector3Length(m_Velocity));
		m_Velocity = newDir * speed;
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