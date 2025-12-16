#include "PlayerController.h"
#include "System/ImGui/imgui.h"
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
	Init_CameraSet();

	return true;
}

void PlayerController::Update(float _deltaTime)
{
	// 操作不能であれば処理しない
	if (!m_Played) return;

	// 回転処理
	Apply_Rotate(_deltaTime);
	// 移動処理
	Apply_Move(_deltaTime);
	// 物理演算処理
	Apply_WaterPhysics(_deltaTime);

	// Playerの更新
	Update_Player();

	// カメラ処理
	Apply_CameraZoom(_deltaTime);
	Update_Camera(_deltaTime);

	// 水のエフェクト処理
	Update_WaterEffects(_deltaTime);
}

void PlayerController::Draw()
{
	Draw_ImGui();
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

void PlayerController::Draw_ImGui()
{
	ImGui::Begin("Player Controller");
	if (ImGui::CollapsingHeader("Player"))
	{
		float speed = XMVectorGetX(XMVector3Length(m_Velocity));
		ImGui::Text("Current Speed: %.2f", speed);
		ImGui::Text("Max Speed: %.2f", m_MaxSpeed);
	}
	ImGui::End();
}

void PlayerController::Init_Param()
{
	if (!m_Player || !m_Camera) return;

	m_Position = m_Player->GetPos();
	m_Rotation = m_Player->GetRota();

	m_ForwardVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_RightVec = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	m_UpVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_LastPlayerPos = m_Position;

	m_RotateSpeed = XMConvertToRadians(45.0f);	// 回転速度
	m_Acceleration = 5.0f;	// 加速度
	m_Friction = 3.0f;		// 摩擦力
	m_MaxSpeed = 40.0f;		// 最大速度

	m_Gravity = -9.8f;		// 重力
	m_Buoyancy = 10.0f;		// 浮力
	m_WaterDamping = 4.0f;	// 水中減衰
	m_PlayerOffsetY = 3.0f; // プレイヤーの高さオフセット
	m_FollowSpeed = 100.0f;	// カメラの追従速度

	XMVECTOR initOffset = XMVectorSet(0.0f, 1.5f, -1.5f, 0.0f);

	float initDistance = XMVectorGetX(XMVector3Length(initOffset));

	m_ZoomSpeed = 10.0f;
	m_MinDistance = initDistance;
	m_MaxDistance = 20.0f;
	
	m_CurrentDistance = initDistance;
	m_InitCamOffset = XMVector3Normalize(initOffset);
	m_CamOffset = initOffset;

	m_SpringConstant = 350.0f;  
	m_DampingConstant = 25.0f;
	m_TurnOffsetMax = 1.0f;
	m_TurnOffsetRate = 10.0f;

	m_CurrentRoll = 0.0f;
	m_MaxRollAngle = XMConvertToRadians(30.0f);
	m_RollInterpRate = 8.0f;

	m_CurrentCamPos = XMVectorAdd(m_Position, m_CamOffset);
	m_CurrentCamTarget = m_Position + m_ForwardVec * 15.0f;

	m_Emitter_Splash = std::make_unique<ParticleEmitter_Splash>(m_Camera);
	m_Emitter_Splash->Init();
}

void PlayerController::Init_CameraSet()
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
	}
}

void PlayerController::Apply_Rotate(float _deltaTime)
{
	float turnInput = 0.0f;
	// 回転
	if (m_Input->GetKeyPress(VK_A))
	{
		m_Rotation -= XMVectorSet(0, m_RotateSpeed * _deltaTime, 0, 0);
		turnInput = 1.0f;
	}
	if (m_Input->GetKeyPress(VK_D))
	{
		m_Rotation += XMVectorSet(0, m_RotateSpeed * _deltaTime, 0, 0);
		turnInput = -1.0f;
	}

	// 回転角度から方向ベクトルを更新
	XMMATRIX rotMat = XMMatrixRotationY(XMVectorGetY(m_Rotation));
	m_ForwardVec = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMat);
	m_RightVec = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMat);

	// カメラオフセットの更新
	XMVECTOR rotateOffsetDir = XMVector3TransformNormal(m_InitCamOffset, rotMat);
	m_CamOffset = rotateOffsetDir * m_CurrentDistance;

	// 旋回時に船体が傾くように
	float targetRoll = turnInput * m_MaxRollAngle;
	float rollChange = ((targetRoll - m_CurrentRoll) * m_RollInterpRate * _deltaTime ) / 5.0f;
	m_CurrentRoll += rollChange;
}

void PlayerController::Apply_Move(float _deltaTime)
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

void PlayerController::Apply_WaterPhysics(float _deltaTime)
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

void PlayerController::Update_Player()
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

void PlayerController::Apply_CameraZoom(float _deltaTime)
{
	if (!m_Input) return;

	float zoomDirection = 0.0f;

	if (m_Input->GetKeyPress(VK_E))
		zoomDirection = 1.0f;
	if (m_Input->GetKeyPress(VK_Q))
		zoomDirection = -1.0f;

	if (zoomDirection != 0.0f)
	{
		float zoomAmount = zoomDirection * m_ZoomSpeed * _deltaTime;

		m_CurrentDistance += zoomAmount;

		if (m_CurrentDistance < m_MinDistance)
		{
			m_CurrentDistance = m_MinDistance;
		}
		else if (m_CurrentDistance > m_MaxDistance)
		{
			m_CurrentDistance = m_MaxDistance;
		}
	}

	XMMATRIX rotMat = XMMatrixRotationY(XMVectorGetY(m_Rotation));
	XMVECTOR rotateOffsetDir = XMVector3TransformNormal(m_InitCamOffset, rotMat);
	m_CamOffset = rotateOffsetDir * m_CurrentDistance;
}

void PlayerController::Update_Camera(float _deltaTime)
{
	if (!m_Camera) return;

	// 旋回オフセットの計算
	XMVECTOR horizontalVelocity = XMVectorSet(
		XMVectorGetX(m_Velocity), 0, XMVectorGetZ(m_Velocity), 0);
	float speed = XMVectorGetX(XMVector3Length(horizontalVelocity));

	// 進行方向とボートの向きを求める
	XMVECTOR velocityDir = XMVector3Normalize(horizontalVelocity);
	XMVECTOR forwardDir = XMVector3Normalize(m_ForwardVec);

	// 速度と前方向の外積で左右旋回を判定
	float rotationDiff = XMVectorGetY(XMVector3Cross(velocityDir, forwardDir));

	float idealTurnOffset = rotationDiff * (speed / m_MaxSpeed) * m_TurnOffsetMax;
	float currentRightOffset = XMVectorGetX(XMVector3Dot(m_CamOffset, m_RightVec));
	
	// 旋回オフセットを滑らかに補間
	float newRightOffset = currentRightOffset;
	newRightOffset = currentRightOffset + (idealTurnOffset - currentRightOffset) * m_TurnOffsetRate * _deltaTime;

	// 新しいカメラオフセットを計算
	XMVECTOR initialOffsetDirection = XMVector3Normalize(m_CamOffset - m_Position);
	XMVECTOR idealCamPos = XMVectorAdd(m_Position, m_CamOffset);

	// 右ベクトル方向の成分を取り除き、新しい右オフセットを加算
	idealCamPos = idealCamPos - m_RightVec * currentRightOffset;
	idealCamPos = idealCamPos + m_RightVec * newRightOffset;

	// 理想的なカメラ位置
	XMVECTOR targetPosition = idealCamPos;

	// ばね・減衰によるカメラ位置の更新
	XMVECTOR displacement = m_CurrentCamPos - targetPosition;
	XMVECTOR springForce = -m_SpringConstant * displacement;
	XMVECTOR dampingForce = -m_DampingConstant * m_CameraVelocity;
	XMVECTOR accelaration = springForce + dampingForce;
	
	m_CameraVelocity += accelaration * _deltaTime;
	m_CurrentCamPos += m_CameraVelocity * _deltaTime;

	m_Camera->SetPos(m_CurrentCamPos);

	XMVECTOR idealTarget = m_Position + m_ForwardVec * 15.0f;

	// 水面の高さを取得して注視点に加算
	float fx = static_cast<float>(XMVectorGetX(m_Position));
	float fz = static_cast<float>(XMVectorGetZ(m_Position));
	float waveHeight = m_WaterMesh->GetHeightFieldHeight(fx, fz);
	idealTarget = XMVectorSetY(idealTarget,
		XMVectorGetY(idealTarget) + waveHeight * 0.3f);

	// カメラ注視点更新
	float interp = 1.0f - expf(-m_FollowSpeed * _deltaTime);
	XMVECTOR currentTarget = m_Camera->GetTarget();
	XMVECTOR newCamTarget = XMVectorLerp(currentTarget, idealTarget, interp);
	m_Camera->SetTarget(newCamTarget);
	m_CurrentCamTarget = newCamTarget; // 次のフレームのために保存
}

void PlayerController::Update_WaterEffects(float _deltaTime)
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
		const float radius = 2.5f;

		// 波紋生成
		m_WaterMesh->ApplyDrop(XMFLOAT2(u, v), dropStrength, radius);
	}
}