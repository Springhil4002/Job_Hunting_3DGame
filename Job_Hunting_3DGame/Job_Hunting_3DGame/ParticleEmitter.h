#pragma once
#include <DirectXMath.h>
#include <vector>
#include <random>
#include "ParticleSystem.h"

class ParticleEmitter
{
protected:
	std::vector<std::unique_ptr<ParticleSystem>> m_Particles;	// パーティクルの管理配列
	int m_MaxParticles;			// パーティクルの上限数
	int m_Min_CreateParticles;	// 最小パーティクル生成数
	int m_Max_CreateParticles;	// 最大パーティクル生成数
	float m_SpawnPaticlesRate;	// パーティクル生成間隔
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
	DirectX::XMFLOAT4 m_StartColor;		// 開始色
	DirectX::XMFLOAT4 m_EndColor;		// 終了色
	float colorChangeRate;	// 色変化率
	float m_Gravity;		// 重力	
	float m_Min_LifeTime;	// 最小寿命
	float m_Max_LifeTime;	// 最大寿命
	float m_Min_Deg;		// 最小回転角度
	float m_Max_Deg;		// 最大回転角度

	virtual void Init() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void CreateParticles() = 0;

	DirectX::XMVECTOR GetPosition() const { return m_Position; }
	DirectX::XMFLOAT4 GetStartColor() const { return m_StartColor; }
	DirectX::XMFLOAT4 GetEndColor() const { return m_EndColor; }
	float GetColorChangeRate() const { return colorChangeRate; }
	
	void SetPosition(const DirectX::XMVECTOR& _pos) { m_Position = _pos; }
	void SetStartColor(const DirectX::XMFLOAT4& _startColor) { m_StartColor = _startColor; }
	void SetEndColor(const DirectX::XMFLOAT4& _endColor) { m_EndColor = _endColor; }
	void SetColorChangeRate(float _rate) { colorChangeRate = _rate; }
};

