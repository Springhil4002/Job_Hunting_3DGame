#pragma once
#include "Player.h"
#include "Camera.h"
#include "System/Input.h"

class PlayerController
{
private:
	Player* m_Player;
	Camera* m_Camera;
	Input*	m_Input;
	
	DirectX::XMVECTOR m_Position;	// 位置
	DirectX::XMVECTOR m_Rotation;	// 回転

	DirectX::XMVECTOR m_ForwardVec		= DirectX::XMVectorZero();	// 前方向ベクトル(移動用)
	DirectX::XMVECTOR m_RightVec		= DirectX::XMVectorZero();	// 右向きベクトル(左右回転用)
	DirectX::XMVECTOR m_UpVec			= DirectX::XMVectorZero();	// 上方向は固定
	DirectX::XMVECTOR m_CamOffset		= DirectX::XMVectorZero();	// カメラオフセット
	DirectX::XMVECTOR m_LastPlayerPos	= DirectX::XMVectorZero();	// 前フレームプレイヤー位置
		
	float m_Yaw;			
	float m_Speed;			// 速度
	float m_RotateSpeed;	// 回転速度
	
	/// @brief 入力処理
	void Update_Input(float _deltaTime);
	/// @brief Playerとカメラの位置更新
	void Update_PlayerTransform();
public:
	/// @brief 初期化処理
	/// @param _player プレイヤーのポインタ変数 
	/// @param _camera カメラのポインタ変数
	/// @return 初期化処理の成否
	bool Init(Player* _player, Camera* _camera, Input* _input);
	/// @brief 更新処理
	void Update(float _deltaTime);

	// 各種ゲッター関数 
	DirectX::XMVECTOR GetPosition();
	DirectX::XMVECTOR GetRotation();
	DirectX::XMVECTOR GetForwardVec();
	DirectX::XMVECTOR GetRightVec();
};