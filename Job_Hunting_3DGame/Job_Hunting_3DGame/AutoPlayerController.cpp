#include "AutoPlayerController.h"
#include "Debug_New.h"
#include <cmath>
#include <algorithm>

using namespace DirectX;

bool AutoPlayerController::Init(Player* _player, WaterMesh* _waterMesh, Camera* _camera)
{
	if (!_player || !_waterMesh || !_camera) return false;

	m_Player = _player;
	m_WaterMesh = _waterMesh;
	m_Camera = _camera;

	Init_Param();
	Init_CameraSet();
	return true;
}

void AutoPlayerController::Update(float _deltaTime)
{
	Apply_SpeedControll(_deltaTime);
	Apply_Figure8(_deltaTime);
	Apply_WaterPhysics(_deltaTime);

	Update_Player();
	Update_Camera(_deltaTime);

	Update_WaterEffects(_deltaTime);
}

void AutoPlayerController::Draw()
{
	if (m_Emitter_Splash)
	{
		m_Emitter_Splash->Draw();
	}
}

void AutoPlayerController::Uninit()
{
	// エミッタの終了処理
	if (m_Emitter_Splash)
		m_Emitter_Splash.reset();

	m_Player = nullptr;
	m_WaterMesh = nullptr;
	m_Camera = nullptr;

	m_Position		= XMVectorZero();
	m_Rotation		= XMVectorZero();
	m_ForwardVec	= XMVectorZero();
	m_RightVec		= XMVectorZero();
	m_CamOffset		= XMVectorZero();
	m_InitCamOffset = XMVectorZero();
	m_Velocity		= XMVectorZero();
	m_VelocityY		= XMVectorZero();
	m_CameraVelocity	= XMVectorZero();
	m_CurrentCamPos		= XMVectorZero();
	m_CurrentCamTarget	= XMVectorZero();
}

void AutoPlayerController::Init_Param()
{
	if (!m_Player || !m_Camera) return;

	m_Speed = 0.5f;	
	m_CurrentSpeed = 0.0f;
	m_Acceleration = 0.2f;
	m_Friction = 0.1f;
	m_RadiusX = 100.0f;			
	m_RadiusZ = 50.0f;			
	m_Figure8Scale = 0.8f;    

	m_Position = m_Player->GetPos();
	m_Rotation = m_Player->GetRota();
	m_ForwardVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_RightVec = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	m_MaxSpeed = 40.0f;
	m_PlayerOffsetY = 3.0f;
	m_Gravity = -9.8f;
	m_Buoyancy = 10.0f;
	m_WaterDamping = 4.0f;
	m_FollowSpeed = 50.0f;

	XMVECTOR initOffset = XMVectorSet(0.0f, 1.5f, -1.5f, 0.0f);
	float initDistance = XMVectorGetX(XMVector3Length(initOffset));
	m_CurrentDistance = initDistance;
	m_InitCamOffset = XMVector3Normalize(initOffset);
	m_CamOffset = initOffset;

	m_SpringConstant = 350.0f;
	m_DampingConstant = 25.0f;
	m_TurnOffsetMax = 1.0f;
	m_TurnOffsetRate = 10.0f;

	m_CurrentRoll = 0.0f;
	m_MaxRollAngle = XMConvertToRadians(30.0f); 
	m_RollInterpRate = 5.0f;                  

	m_CurrentCamPos = XMVectorAdd(m_Position, m_CamOffset);
	m_CurrentCamTarget = m_Position + m_ForwardVec * 15.0f;

	m_Emitter_Splash = std::make_unique<ParticleEmitter_Splash>(m_Camera);
	m_Emitter_Splash->Init();
}

void AutoPlayerController::Init_CameraSet()
{
	if (m_Camera)
	{
		// カメラの理想的な初期位置を計算
		XMVECTOR idealCamPos = XMVectorAdd(m_Position, m_CamOffset);
		// カメラの注視点の理想的な初期位置を計算
		XMVECTOR idealTarget = m_Position + m_ForwardVec * 15.0f;

		// 水面の高さを考慮した注視点の調整
		if (m_WaterMesh)
		{
			float fx = static_cast<float>(XMVectorGetX(m_Position));
			float fz = static_cast<float>(XMVectorGetZ(m_Position));
			float waveHeight = m_WaterMesh->GetHeightFieldHeight(fx, fz);
			idealTarget = XMVectorSetY(idealTarget,
				XMVectorGetY(idealTarget) + waveHeight * 0.3f);
		}

		// カメラを理想位置に即座に設定 
		m_Camera->SetPos(idealCamPos);
		m_Camera->SetTarget(idealTarget);
		
		m_CurrentCamPos = idealCamPos;
		m_CurrentCamTarget = idealTarget;
	}
}

void AutoPlayerController::Apply_SpeedControll(float _deltaTime)
{
	// 目標速度に向かって加速する
	if (m_CurrentSpeed < m_Speed)
	{
		m_CurrentSpeed += m_Acceleration * _deltaTime;
		if (m_CurrentSpeed > m_Speed)
		{
			m_CurrentSpeed = m_Speed;
		}
	}
	else if (m_CurrentSpeed > m_Speed)
	{
		m_CurrentSpeed -= m_Friction * _deltaTime;
		if (m_CurrentSpeed < m_Speed)
		{
			m_CurrentSpeed = m_Speed;
		}
	}
}

void AutoPlayerController::Apply_Figure8(float _deltaTime)
{
	// 前フレームの位置を保存し、時間更新
	XMVECTOR prevPos = m_Position;
	m_Time += m_CurrentSpeed * _deltaTime;

	// 8の字軌道の計算
	float t = m_Time;
	float T = t * m_Figure8Scale;
	float newX = m_RadiusX * std::sin(T);
	float newZ = m_RadiusZ * std::sin(T) * std::cos(T);
	
	// プレイヤーの位置と速度を計算
	m_Position = XMVectorSet(newX, XMVectorGetY(m_Position), newZ, 0.0f);

	// 速度
	XMVECTOR currentVelocity = (m_Position - prevPos) / _deltaTime;
	m_Velocity = currentVelocity;

	float prevAngleY = XMVectorGetY(m_Rotation);

	// 回転の計算
	XMVECTOR movementVec = XMVectorSet(
		XMVectorGetX(currentVelocity), 0.0f, 
		XMVectorGetZ(currentVelocity), 0.0f);
	
	if (XMVectorGetX(XMVector3LengthSq(movementVec)) > 0.0001f)
	{
		// 前方向ベクトルを正規化
		m_ForwardVec = XMVector3Normalize(movementVec);

		// 新しい右方向ベクトルを計算
		m_RightVec = XMVector3Cross(XMVectorSet(0, 1, 0, 0), m_ForwardVec);
		m_RightVec = XMVector3Normalize(m_RightVec);

		// 回転角を前方向ベクトルから逆算
		float angleY = std::atan2(XMVectorGetX(m_ForwardVec), XMVectorGetZ(m_ForwardVec));
		m_Rotation = XMVectorSet(0, angleY, 0, 0);

		// 角速度の変化量を計算
		float deltaAngleY = angleY - prevAngleY;
		if (deltaAngleY > XM_PI)
			deltaAngleY -= XM_2PI;
		else if(deltaAngleY < -XM_PI)
			deltaAngleY += XM_2PI;
		// 旋回速度
		float angularVelocity = deltaAngleY / _deltaTime;
		// 旋回速度の最大値を正規化
		const float maxAngularVelocity = 1.0f;
		float targetRoll = -angularVelocity / maxAngularVelocity * m_MaxRollAngle;
		// 最大値にクランプ
		targetRoll = std::clamp(targetRoll, -m_MaxRollAngle, m_MaxRollAngle);

		// 補間
		float interpRate = (m_RollInterpRate * _deltaTime) / 5.0f;
		m_CurrentRoll = m_CurrentRoll + (targetRoll - m_CurrentRoll) * interpRate;
	}
}

void AutoPlayerController::Apply_WaterPhysics(float _deltaTime)
{
	if (!m_WaterMesh) return;

	float fx = static_cast<float>(XMVectorGetX(m_Position));
	float fz = static_cast<float>(XMVectorGetZ(m_Position));
	// プレイヤーの底面Y座標
	float y = XMVectorGetY(m_Position) - m_PlayerOffsetY;

	// 浮力の基準となる高さ場の高さを取得
	float waterHeight = m_WaterMesh->GetHeightFieldHeight(fx, fz);
	// 水面からの沈み込み深さ
	float depth = waterHeight - y;

	// 水中にいるなら
	if (depth > 0.0f)
	{
		// 浮力
		float buoyancyForce = m_Buoyancy * depth;
		// 水中減衰
		float damping = m_WaterDamping * XMVectorGetY(m_VelocityY);

		float vy = XMVectorGetY(m_VelocityY);
		// 垂直速度
		vy += (buoyancyForce + m_Gravity - damping) * _deltaTime;

		m_VelocityY = XMVectorSet(0, vy, 0, 0);
	}
	else
	{
		// 水面上なら重力のみ
		m_VelocityY += XMVectorSet(0, m_Gravity * _deltaTime, 0, 0);
	}

	// 垂直速度を位置に加算
	m_Position += m_VelocityY * _deltaTime;
}

void AutoPlayerController::Update_Player()
{
	if (!m_Player) return;
	
	XMVECTOR totalRotation = XMVectorSet(
		XMVectorGetX(m_Rotation),
		XMVectorGetY(m_Rotation),
		m_CurrentRoll,
		0.0f
	);

	m_Player->SetPos(m_Position);
	m_Player->SetRota(totalRotation);
}

void AutoPlayerController::Update_Camera(float _deltaTime)
{
	if (!m_Camera) return;
	XMVECTOR horizontalVelocity = XMVectorSet(
		XMVectorGetX(m_Velocity), 0, XMVectorGetZ(m_Velocity), 0);
	float speed = XMVectorGetX(XMVector3Length(horizontalVelocity));

	// 進行方向とボートの向きを求める
	XMVECTOR velocityDir = XMVector3Normalize(horizontalVelocity);
	XMVECTOR forwardDir =m_ForwardVec;

	if (speed < 0.1f)
	{
		velocityDir = forwardDir;
	}

	float rotationDiff = XMVectorGetY(XMVector3Cross(velocityDir, forwardDir));
	float idealTurnOffset = rotationDiff * (speed / m_MaxSpeed) * m_TurnOffsetMax;

	// m_CamOffsetは追尾のベースオフセット 
	XMMATRIX rotMat = XMMatrixRotationY(XMVectorGetY(m_Rotation));
	XMVECTOR rotateOffsetDir = XMVector3TransformNormal(m_InitCamOffset, rotMat);
	m_CamOffset = rotateOffsetDir * m_CurrentDistance;
	
	XMVECTOR baseIdealPos = XMVectorAdd(m_Position, m_CamOffset);
	float currentRightOffset = XMVectorGetX(XMVector3Dot(m_CurrentCamPos - baseIdealPos, m_RightVec));

	// 旋回オフセットを滑らかに補間
	float newRightOffset = currentRightOffset;
	newRightOffset = currentRightOffset + (idealTurnOffset - currentRightOffset) * m_TurnOffsetRate * _deltaTime;

	XMVECTOR turnOffsetVec = m_RightVec * newRightOffset;
	XMVECTOR targetPos = baseIdealPos + turnOffsetVec;

	// ばね・減衰によるカメラ位置の更新
	XMVECTOR displacement = m_CurrentCamPos - targetPos;
	XMVECTOR springForce = -m_SpringConstant * displacement;
	XMVECTOR dampingForce = -m_DampingConstant * m_CameraVelocity;
	XMVECTOR accelaration = springForce + dampingForce;

	m_CameraVelocity += accelaration * _deltaTime;
	m_CurrentCamPos += m_CameraVelocity * _deltaTime;

	m_Camera->SetPos(m_CurrentCamPos);

	// カメラ注視点更新
	XMVECTOR idealTarget = m_Position + m_ForwardVec * 15.0f;

	// 水面の高さを取得して注視点に加算
	float fx = static_cast<float>(XMVectorGetX(m_Position));
	float fz = static_cast<float>(XMVectorGetZ(m_Position));
	float waveHeight = m_WaterMesh->GetHeightFieldHeight(fx, fz);
	idealTarget = XMVectorSetY(idealTarget,
		XMVectorGetY(idealTarget) + waveHeight * 0.3f);

	float interp = 1.0f - expf(-m_FollowSpeed * _deltaTime);
	XMVECTOR currentTarget = m_Camera->GetTarget();
	XMVECTOR newCamTarget = XMVectorLerp(currentTarget, idealTarget, interp);
	m_Camera->SetTarget(newCamTarget);
	m_CurrentCamTarget = newCamTarget; // 次のフレームのために保存
}

void AutoPlayerController::Update_WaterEffects(float _deltaTime)
{
	if (!m_Emitter_Splash || !m_WaterMesh) return;

	bool isMoving = false;

	XMVECTOR horizontalVelocity = XMVectorSet(XMVectorGetX(m_Velocity), 0, XMVectorGetZ(m_Velocity), 0);
	float speed = XMVectorGetX(XMVector3Length(horizontalVelocity));
	if (speed > 0.1f) isMoving = true;

	const float splashOffsetY = -1.0f;
	// Playerの先頭部分(中心座標＋進行方向のオフセット)
	const float offsetLength = 2.0f;
	XMVECTOR spawnPos = m_Position + m_ForwardVec * offsetLength;
	spawnPos = XMVectorAdd(spawnPos, XMVectorSet(0, splashOffsetY, 0, 0));

	m_Emitter_Splash->Update(_deltaTime, spawnPos, m_RightVec, isMoving);

	// 移動時、波紋生成
	if (speed > 0.05f)
	{
		// プレイヤーのワールド座表
		float fx = XMVectorGetX(m_Position);
		float fz = XMVectorGetZ(m_Position);

		XMVECTOR forwardVec = m_ForwardVec;

		const float offset = 2.0f;

		fx += XMVectorGetX(forwardVec) * offset;
		fz += XMVectorGetZ(forwardVec) * offset;

		// 水面メッシュのグリッドサイズを取得
		const float gridSize = m_WaterMesh->GetGridSize();
		const float halfSize = gridSize * 0.5f;

		// ワールド座表をWaterMeshのUVに変換、Gridの中心がWaterMeshの中心になるように
		float u = (fx + halfSize) / gridSize;
		float v = (fz + halfSize) / gridSize;

		// UV座表をクランプ
		u = std::clamp(u, 0.0f, 1.0f);
		v = std::clamp(v, 0.0f, 1.0f);

		// 速度に応じたドロップの半径と強さを設定
		const float dropStrength = std::min(5.0f, speed * 0.05f);
		const float radius = 3.0f;

		// 波紋生成
		m_WaterMesh->ApplyDrop(XMFLOAT2(u, v), dropStrength, radius);
	}
}