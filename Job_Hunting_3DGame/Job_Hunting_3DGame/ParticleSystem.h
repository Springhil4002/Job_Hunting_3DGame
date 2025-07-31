#pragma once
#include <DirectXMath.h>
#include <vector>

class ParticleSystem
{
public:
	enum ParticleState
	{
		Stop,
		Active
	};
protected:
	DirectX::XMVECTOR m_Position;		// 位置
	DirectX::XMVECTOR m_Velocity;		// 速度
	DirectX::XMFLOAT4 m_Color;			// 色
	DirectX::XMFLOAT4 m_StartColor;		// 開始色
	DirectX::XMFLOAT4 m_EndColor;		// 終了色
	float m_colorChangeRate;	// 色変化率
	float m_Gravity;			// 重力
	float m_LifeTime;			// 寿命
	float m_Time;				// 経過時間
	float m_Deg;				// 回転角度
	ParticleState m_State;
public:
	/// @brief 初期化処理
	virtual void Init() = 0;
	/// @brief 更新処理
	virtual void Update(float deltaTime) = 0;
	/// @brief 描画処理
	virtual void Draw() = 0;
	/// @brief パーティクルのパラメータリセット
	void Reset();
	/// @brief パーティクルのアクティブ状態を取得する関数
	/// @return パーティクルのアクティブ状態
	bool IsActive() const;
	/// @brief パーティクルのカラーを補間する関数
	/// @param _startColor 開始色
	/// @param _endColor 終了色
	/// @param _time 補間時間
	/// @return 補間後の色
	float colorLerp(float _startColor, float _endColor, float _time);

	// パーティクルのパラメータのゲッターとセッター 
	DirectX::XMVECTOR GetPosition() const { return m_Position; }
	DirectX::XMVECTOR GetVelocity() const { return m_Velocity; }
	DirectX::XMFLOAT4 GetColor() const { return m_Color; }
	DirectX::XMFLOAT4 GetStartColor() const { return m_StartColor; }
	DirectX::XMFLOAT4 GetEndColor() const { return m_EndColor; }
	float GetLifeTime() const { return m_LifeTime; }
	ParticleState GetState() const { return m_State; }
	void SetPosition(const DirectX::XMVECTOR& _pos) { m_Position = _pos; }
	void SetVelocity(const DirectX::XMVECTOR& _vel) { m_Velocity = _vel; }
	void SetColor(const DirectX::XMFLOAT4& _color) { m_Color = _color; }
	void SetStartColor(const DirectX::XMFLOAT4& _startColor) { m_StartColor = _startColor; }
	void SetEndColor(const DirectX::XMFLOAT4& _endColor) { m_EndColor = _endColor; }
	void SetLifeTime(float _lifeTime) { m_LifeTime = _lifeTime; }
	void SetState(ParticleState _state) { m_State = _state; }
};

