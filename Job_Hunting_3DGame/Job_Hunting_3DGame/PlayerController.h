#pragma once
#include "Player.h"
#include "Camera.h"
#include "WaterMesh.h"
#include "System/Input.h"
#include "ParticleEmitter_Splash.h"

class PlayerController
{
private:
	Player* m_Player;
	WaterMesh* m_WaterMesh;
	Camera* m_Camera;
	Input*	m_Input;
	std::unique_ptr<ParticleEmitter_Splash> m_Emitter_Splash;

	DirectX::XMVECTOR m_Position;	// 位置
	DirectX::XMVECTOR m_Rotation;	// 回転

	DirectX::XMVECTOR m_ForwardVec		= DirectX::XMVectorZero();	// 前方向ベクトル(移動用)
	DirectX::XMVECTOR m_RightVec		= DirectX::XMVectorZero();	// 右向きベクトル(左右回転用)
	DirectX::XMVECTOR m_UpVec			= DirectX::XMVectorZero();	// 上方向は固定
	DirectX::XMVECTOR m_CamOffset		= DirectX::XMVectorZero();	// カメラオフセット
	DirectX::XMVECTOR m_LastPlayerPos	= DirectX::XMVectorZero();	// 前フレームプレイヤー位置
	DirectX::XMVECTOR m_Velocity		= DirectX::XMVectorZero();	// 現在の移動速度ベクトル
	DirectX::XMVECTOR m_VelocityY		= DirectX::XMVectorZero();	// 縦方向の速度(重力用)		

	float m_RotateSpeed;	// 回転速度
	float m_Acceleration;	// 加速度
	float m_Friction;		// 摩擦力
	float m_MaxSpeed;		// 最大速度

	float m_Gravity;		// 重力
	float m_Buoyancy;		// 浮力
	float m_WaterDamping;	// 水中減衰
	float m_PlayerOffsetY;	// プレイヤーの高さオフセット
	float m_FollowSpeed;	// カメラの追従速度	

	/// @brief メンバ変数の初期化
	/// @param _player プレイヤー
	/// @param _waterMesh 水面メッシュ
	/// @param _camera カメラ
	/// @param _input 入力系統
	/// @return 初期化の成否
	bool Init_Prop(Player* _player, WaterMesh* _waterMesh, Camera* _camera, Input* _input);
	/// @brief パラメータの初期化
	void Init_Param();
	/// @brief 入力処理
	/// @param _deltaTime 経過時間
	void Update_Input(float _deltaTime);
	/// @brief 浮力処理
	/// @param _deltaTime 経過時間
	void Update_Buoyancy(float _deltaTime);
	/// @brief Playerとカメラの位置更新
	void Update_PlayerTransform(float _deltaTime);
	/// @brief エミッタの更新処理
	/// @param _deltaTime 経過時間
	void Update_Emitter(float _deltaTime);
	/// @brief 入力による回転処理
	/// @param _deltaTime 経過時間
	void Input_Rotate(float _deltaTime);
	/// @brief 入力による移動処理
	/// @param _deltaTime 経過時間
	void Input_Move(float _deltaTime);
	/// @brief パーティクルの描画処理
	void Draw_Particle();
public:
	/// @brief 初期化処理
	/// @param _player プレイヤーのポインタ変数 
	/// @param _camera カメラのポインタ変数
	/// @return 初期化処理の成否
	bool Init(Player* _player, WaterMesh* _waterMesh, Camera* _camera, Input* _input);
	/// @brief 更新処理
	/// @param _deltaTime 経過時間
	void Update(float _deltaTime);
	/// @brief 描画処理
	void Draw();

	// 各種ゲッター・セッター関数 
	DirectX::XMVECTOR GetPosition() const { return m_Position; }
	DirectX::XMVECTOR GetRotation() const { return m_Rotation; }
	DirectX::XMVECTOR GetForwardVec() const { return m_ForwardVec; }
	DirectX::XMVECTOR GetRightVec() const { return m_RightVec; }
	DirectX::XMVECTOR GetVelocity() const { return m_Velocity; }
	float GetMaxSpeed() const { return m_MaxSpeed; }
	float GetFollowSpeed() const { return m_FollowSpeed; }

	void SetMaxSpeed(float _maxSpeed) { m_MaxSpeed = _maxSpeed; }
	void SetFollowSpeed(float _followSpeed) { m_FollowSpeed = _followSpeed; }
};