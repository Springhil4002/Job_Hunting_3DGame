#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Camera.h"

enum ParticleState
{
	Stop,
	Active
};

struct Particle
{
	DirectX::XMVECTOR position;		// 位置
	DirectX::XMVECTOR velocity;		// 速度
	DirectX::XMFLOAT4 color;		// 色
	DirectX::XMFLOAT4 startColor;	// 開始色
	DirectX::XMFLOAT4 endColor;		// 終了色
	float colorChangeRate;			// 色変化率
	float lifeTime;					// 寿命
	float elapsedTime;				// 経過時間
	float gravity;					// 重力
	ParticleState particleState;	// 状態管理
	
	/// @brief パーティクルのパラメータリセット
	void Reset();
	/// @brief パーティクルのアクティブ状態を取得する関数
	/// @return パーティクルのアクティブ状態
	bool IsActive() const;
	/// @brief 
	/// @param _startColor 
	/// @param _endColor 
	/// @param _time 
	/// @return 
	DirectX::XMFLOAT4 LerpColor(const DirectX::XMFLOAT4& _startColor, const DirectX::XMFLOAT4& _endColor, float _time);
};

class ParticleSystem
{
protected:
	std::vector<Particle> m_Particles;	// パーティクルの管理配列
	int m_MaxParticles;				// パーティクルの上限数
public:
	/// @brief 初期化処理
	virtual bool Init(Camera* _camera) = 0;
	/// @brief 更新処理
	virtual void Update(float deltaTime) = 0;
	/// @brief 描画処理
	virtual void Draw() = 0;
	/// @brief 終了処理
	virtual void Uninit() = 0;

	std::vector<Particle>& GetParticles() { return m_Particles; }
	int GetMaxParticles() { return m_MaxParticles; }
	void SetMaxParticles(int _maxParticle) { m_MaxParticles = _maxParticle; }
};