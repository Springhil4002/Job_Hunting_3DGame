#pragma once
#include "Player.h"
#include "Camera.h"
#include "WaterMesh.h"
#include "System/Input.h"
#include "ParticleEmitter_Splash.h"

class AutoPlayerController
{
private:
	Player* m_Player = nullptr;
	WaterMesh* m_WaterMesh = nullptr;
	Camera* m_Camera = nullptr;
	std::unique_ptr<ParticleEmitter_Splash> m_Emitter_Splash;

	float m_Time = 0.0f;			// 経過時間カウンター
	float m_Speed = 0.0f;			// 軌道を周回する速さ
	float m_CurrentSpeed = 0.0f;	// 現在の軌道速度
	float m_Acceleration = 0.0f;	// 加速度
	float m_Friction = 0.0f;		// 摩擦力
	float m_RadiusX = 0.0f;			// 8の字のX方向の半径
	float m_RadiusZ = 0.0f;			// 8の字のZ方向の半径
	float m_Figure8Scale = 0.0f;    // 8の字の広がりを制御する係数

	DirectX::XMVECTOR m_Position = DirectX::XMVectorZero();		// 位置
	DirectX::XMVECTOR m_Rotation = DirectX::XMVectorZero();		// 回転
	DirectX::XMVECTOR m_ForwardVec = DirectX::XMVectorZero();	// 前方向ベクトル
	DirectX::XMVECTOR m_RightVec = DirectX::XMVectorZero();		// 右向きベクトル

	// プレイヤーの移動速度
	DirectX::XMVECTOR m_Velocity = DirectX::XMVectorZero();		// 現在の移動速度
	DirectX::XMVECTOR m_VelocityY = DirectX::XMVectorZero();    // 縦方向の速度(重力用)

	// 水上物理演算パラメータ
	float m_MaxSpeed = 0.0f;
	float m_PlayerOffsetY = 0.0f;
	float m_Gravity = 0.0f;
	float m_Buoyancy = 0.0f;
	float m_WaterDamping = 0.0f;

	// カメラ制御パラメータ
	DirectX::XMVECTOR m_CamOffset = DirectX::XMVectorZero();
	DirectX::XMVECTOR m_InitCamOffset = DirectX::XMVectorZero();
	DirectX::XMVECTOR m_CameraVelocity = DirectX::XMVectorZero();
	DirectX::XMVECTOR m_CurrentCamPos = DirectX::XMVectorZero();
	DirectX::XMVECTOR m_CurrentCamTarget = DirectX::XMVectorZero();

	float m_FollowSpeed = 0.0f;
	float m_CurrentDistance = 0.0f;
	float m_SpringConstant = 0.0f;
	float m_DampingConstant = 0.0f;
	float m_TurnOffsetMax = 0.0f;
	float m_TurnOffsetRate = 0.0f;

	/// @brief パラメータの初期化
	void Init_Param();
	/// @brief カメラ初期位置設定関数
	void Init_CameraSet();
	
	/// @brief 速度を調整する関数
	/// @param _deltaTime 経過時間
	void Apply_SpeedControll(float _deltaTime);
		
	// @brief 8の字の軌道に基づきPlayerの位置と回転を更新
	void Apply_Figure8(float _deltaTime);
	/// @brief 水の処理関係を更新する関数
	void Apply_WaterPhysics(float _deltaTime);

	/// @brief Playerの位置と回転の更新関数
	void Update_Player();
	/// @brief Playerとカメラの位置更新
	void Update_Camera(float _deltaTime);

	/// @brief エミッタの更新処理
	/// @param _deltaTime 経過時間
	void Update_WaterEffects(float _deltaTime);
public:
	/// @brief 初期化処理
	bool Init(Player* _player, WaterMesh* _waterMesh, Camera* _camera);
	/// @brief 更新処理
	void Update(float _deltaTime);
	/// @brief 描画処理
	void Draw();
	/// @brief 終了処理
	void Uninit();
};