#pragma once
#include <DirectXMath.h>
#include <vector>
#include <random>
#include "ParticleSystem.h"
#include "Camera.h"

class ParticleEmitter
{
protected:
	std::unique_ptr<ParticleSystem> m_pParticleSystem;
	std::vector<Particle>* m_pParticles;	// パーティクルの管理配列
	int m_MaxParticles;			// パーティクルの上限数
	int m_Min_CreateParticles;	// 最小パーティクル生成数
	int m_Max_CreateParticles;	// 最大パーティクル生成数
	float m_SpawnParticlesRate;	// パーティクル生成間隔
	float m_Time;				// 経過時間

	std::random_device m_Rd;	// 乱数生成器のシード
	std::mt19937 m_Mt;			// メルセンヌツイスター乱数生成器
	/// @brief float型の乱数を生成する関数
	/// @param _min 最低値
	/// @param _max 最大値
	/// @return float型の乱数
	float Rand_f(float _min, float _max);
	/// @brief int型の乱数を生成する関数
	/// @param _min 最低値
	///	@param _max 最大値
	/// @return int型の乱数
	int Rand_Int(int _min, int _max);
public:
	DirectX::XMVECTOR m_Position;		// 位置
	DirectX::XMVECTOR m_Velocity;		// 速度
	DirectX::XMFLOAT4 m_StartColor;		// 開始色
	DirectX::XMFLOAT4 m_EndColor;		// 終了色
	float m_ColorChangeRate;	// 色変化率
	float m_Gravity;		// 重力	
	float m_Min_LifeTime;	// 最小寿命
	float m_Max_LifeTime;	// 最大寿命
	ParticleState m_State;
	Camera* m_Camera;		// カメラ

	virtual bool Init() = 0;
	virtual void Update(float deltaTime, DirectX::XMVECTOR _pos, DirectX::XMVECTOR _rightVec, bool _moving) = 0;
	virtual void Draw() = 0;
	virtual void CreateParticles(DirectX::XMVECTOR _pos, DirectX::XMVECTOR _rigjtVec) = 0;

	int GetMaxParticles() { return m_MaxParticles; }
	int GetMinCreateParticles() const { return m_Min_CreateParticles; }
	int GetMaxCreateParticles() const { return m_Max_CreateParticles; }

	void SetParticles(std::vector<Particle>* _particles) { m_pParticles = _particles; }
	void SetMaxParticles(int _max) { m_MaxParticles = _max; }
	void SetCreateParticleNum(int _min, int _max)
	{
		m_Min_CreateParticles = _min;
		m_Max_CreateParticles = _max;
	}
	void SetCreateParticleRate(float _rate) { m_SpawnParticlesRate = _rate; }
	void SetMinCreateParticles(int _min) { m_Min_CreateParticles = _min; }
	void SetMaxCreateParticles(int _max) { m_Max_CreateParticles = _max; }
};