#pragma once
#include "Player.h"
#include "Camera.h"
#include "WaterMesh.h"
#include "SeaMesh.h"
#include "System/Input.h"
#include "ParticleEmitter_Splash.h"

class PlayerController
{
private:
	Player* m_Player = nullptr;
	WaterMesh* m_WaterMesh = nullptr;
	SeaMesh* m_SeaMesh = nullptr;
	Camera* m_Camera = nullptr;
	Input* m_Input = nullptr;
	std::unique_ptr<ParticleEmitter_Splash> m_Emitter_Splash;

	DirectX::XMVECTOR m_Position = DirectX::XMVectorZero();	// 位置
	DirectX::XMVECTOR m_Rotation = DirectX::XMVectorZero();	// 回転

	DirectX::XMVECTOR m_ForwardVec		= DirectX::XMVectorZero();	// 前方向ベクトル(移動用)
	DirectX::XMVECTOR m_RightVec		= DirectX::XMVectorZero();	// 右向きベクトル(左右回転用)
	DirectX::XMVECTOR m_UpVec			= DirectX::XMVectorZero();	// 上方向は固定
	DirectX::XMVECTOR m_CamOffset		= DirectX::XMVectorZero();	// カメラオフセット
	DirectX::XMVECTOR m_InitCamOffset   = DirectX::XMVectorZero();	// 初期カメラオフセット
	DirectX::XMVECTOR m_LastPlayerPos	= DirectX::XMVectorZero();	// 前フレームプレイヤー位置
	DirectX::XMVECTOR m_Velocity		= DirectX::XMVectorZero();	// 現在の移動速度ベクトル
	DirectX::XMVECTOR m_VelocityY		= DirectX::XMVectorZero();	// 縦方向の速度(重力用)		
	
	DirectX::XMVECTOR m_CameraVelocity = DirectX::XMVectorZero();	// カメラの速度
	DirectX::XMVECTOR m_CurrentCamTarget = DirectX::XMVectorZero();	// 現在のカメラ注視点
	DirectX::XMVECTOR m_CurrentCamPos = DirectX::XMVectorZero();	// 現在のカメラ位置

	bool m_Played = true;			// 操作可能フラグ

	float m_RotateSpeed = 0.0f;		// 回転速度
	float m_Acceleration = 0.0f;	// 加速度
	float m_Friction = 0.0f;		// 摩擦力
	float m_MaxSpeed = 0.0f;		// 最大速度

	float m_Gravity = 0.0f;			// 重力
	float m_Buoyancy = 0.0f;		// 浮力
	float m_WaterDamping = 0.0f;	// 水中減衰
	float m_PlayerOffsetY = 0.0f;	// プレイヤーの高さオフセット
	float m_FollowSpeed = 0.0f;		// カメラの追従速度	

	float m_CurrentDistance = 0.0f; // 現在のカメラとPlayerの距離
	float m_MinDistance = 0.0f;		// 最小カメラ距離
	float m_MaxDistance = 0.0f;		// 最大カメラ距離
	float m_ZoomSpeed = 0.0f;		// ズーム速度

	float m_SpringConstant = 0.0f;  // ばね定数
	float m_DampingConstant = 0.0f; // 減衰定数
	float m_TurnOffsetMax = 0.0f;	// 最大旋回オフセット
	float m_TurnOffsetRate = 0.0f;	// 旋回オフセットの追従率

	float m_CurrentRoll = 0.0f;		// 現在のロール角(Z軸回転)
	float m_MaxRollAngle = 0.0f;	// 最大傾斜角度(ラジアン)
	float m_RollInterpRate = 0.0f;	// ロール角の補間追従速度

	/// @brief パラメータの初期化
	void Init_Param();
	/// @brief カメラ初期位置設定関数
	void Init_CameraSet();

	/// @brief Playerの回転と方向ベクトルの更新関数
	/// @param _deltaTime 経過時間
	void Apply_Rotate(float _deltaTime);
	/// @brief Playerの移動処理系を更新する関数
	/// @param _deltaTime 経過時間
	void Apply_Move(float _deltaTime);
	/// @brief 水の処理関係を更新する関数
	/// @param _deltaTime 経過時間
	void Apply_WaterPhysics(float _deltaTime);
	
	/// @brief Playerの位置と回転の更新関数
	void Update_Player();
	/// @brief Playerとカメラの位置更新
	void Update_Camera(float _deltaTime);
	/// @brief カメラズーム関数
	/// @param _deltaTime 経過時間
	void Apply_CameraZoom(float _deltaTime);

	/// @brief エミッタの更新処理
	/// @param _deltaTime 経過時間
	void Update_WaterEffects(float _deltaTime);

	// 波紋生成関数
	void Apply_DropToMesh(const DirectX::XMFLOAT2& _uv, float _strength, float _radius);
	// 高さ取得関数
	float Get_HeightFromMesh(float _x, float _z);
	// メッシュのサイズ取得関数
	float Get_GridSize();
public:
	/// @brief 初期化処理(WaterMesh)
	/// @param _player プレイヤー
	/// @param _waterMesh 水面メッシュ
	/// @param _camera カメラ
	/// @param _input 入力系統
	/// @return 初期化の成否
	bool Init(Player* _player, WaterMesh* _waterMesh, Camera* _camera, Input* _input);
	/// @brief 初期化処理(SeaMesh)
	/// @param _player プレイヤー
	/// @param _seaMesh 海メッシュ
	/// @param _camera カメラ
	/// @param _input 入力系統
	/// @return 初期化の成否
	bool Init(Player* _player, SeaMesh* _seaMesh, Camera* _camera, Input* _input);
	/// @brief 更新処理
	/// @param _deltaTime 経過時間
	void Update(float _deltaTime);
	/// @brief 描画処理
	void Draw();
	/// @brief 終了処理
	void UnInit();
	// ImGui描画関数
	void Draw_ImGui();

	// 各種ゲッター・セッター関数 
	DirectX::XMVECTOR GetPosition() const { return m_Position; }
	DirectX::XMVECTOR GetRotation() const { return m_Rotation; }
	DirectX::XMVECTOR GetForwardVec() const { return m_ForwardVec; }
	DirectX::XMVECTOR GetRightVec() const { return m_RightVec; }
	DirectX::XMVECTOR GetVelocity() const { return m_Velocity; }
	bool IsPlayed() const { return m_Played; }
	float GetMaxSpeed() const { return m_MaxSpeed; }
	float GetFollowSpeed() const { return m_FollowSpeed; }

	void SetPlayed(bool _played) { m_Played = _played; }
	void SetPosition(DirectX::XMVECTOR _pos) { m_Position = _pos; }
	void SetMaxSpeed(float _maxSpeed) { m_MaxSpeed = _maxSpeed; }
	void SetAcceleration(float _accel) { m_Acceleration = _accel; }
	void SetFollowSpeed(float _followSpeed) { m_FollowSpeed = _followSpeed; }
};