#include "ParticleEmitter_Splash.h"
#include <algorithm>
#include "Debug_New.h"

using namespace DirectX;

ParticleEmitter_Splash::ParticleEmitter_Splash(Camera* _camera)
{
	m_Camera = _camera;
	m_Mt = std::mt19937(m_Rd());
}

bool ParticleEmitter_Splash::Init()
{
	if (!Init_Prop()) return false;
	Init_Param();
	return true;
}

void ParticleEmitter_Splash::Update(float _deltaTime, 
	XMVECTOR _pos, XMVECTOR _rightVec, XMVECTOR _velocity, bool _moving, float _state)
{
	m_Time += _deltaTime;

	if (_moving && m_Time >= m_SpawnParticlesRate)
	{
		CreateParticles(_pos, _rightVec, _velocity, _state);
		m_Time = 0.0f;
	}
	if (m_pParticleSystem)
	{
		m_pParticleSystem->Update(_deltaTime);
	}
}

void ParticleEmitter_Splash::Draw()
{
	if (m_pParticleSystem)
	{
		m_pParticleSystem->Draw();
	}
}

bool ParticleEmitter_Splash::Init_Prop()
{
	m_pParticleSystem = std::make_unique<ParticleSystem_Splash>(m_Camera);
	if (!m_pParticleSystem) return false;
	
	SetParticles(&m_pParticleSystem->GetParticles());

	return true;
}

void ParticleEmitter_Splash::Init_Param()
{
	SetCreateParticleNum(2000, 2000);
	SetCreateParticleRate(0.005f);
	SetMaxParticles(m_pParticleSystem->GetMaxParticles());
	m_StartColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_EndColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	m_ColorChangeRate = 0.8f;
	m_Time = 0.0f;
	m_Min_LifeTime = 1.0f;
	m_Max_LifeTime = 5.0f;
	m_Gravity = -9.8f;
	m_State = Active;
}

void ParticleEmitter_Splash::CreateParticles(XMVECTOR _centerPos, 
	XMVECTOR _rightVec, XMVECTOR _velocity, float _state)
{
	if (!m_pParticles) return;

	// パーティクルの生成を決定
	int num = Rand_Int(m_Min_CreateParticles, m_Max_CreateParticles);
	// ボートの現在速度取得
	float speed = XMVectorGetX(XMVector3Length(_velocity));
	// 旋回しているか
	bool isTurning = (fabsf(_state) > 0.3f);

	for (int i = 0; i < num; ++i)
	{
		// 奇数:左、偶数:右
		XMVECTOR basePos;
		float sidePower;
		
		if (!isTurning)
		{
			// 直進時、左右から生成
			sidePower = (i % 2 == 0) ? 1.0f : -1.0f;
			basePos = _centerPos + _rightVec * (sidePower * 1.0f);
		}
		else
		{
			// 旋回時、外側のみ生成
			sidePower = (_state > 0) ? -1.0f : 1.0f;
			// 旋回時は後方に生成して水を掻き出す感じに
			XMVECTOR backOffset = XMVector3Normalize(_velocity) * -1.0f;
			basePos = _centerPos + backOffset + _rightVec * (sidePower * 1.4f);
		}
		// 生成位置を決定
		XMVECTOR spawnPos = basePos + XMVectorSet(Rand_f(-0.2f, 0.2f), Rand_f(-0.1f, 0.1f), Rand_f(-0.2f, 0.2f), 0);
		XMVECTOR particleVelocity = _velocity * 0.6f;
		
		// 旋回時
		if (isTurning)
		{
			particleVelocity += _rightVec * sidePower * (speed * 0.6f + 2.0f);
			particleVelocity += XMVectorSet(0.0f, Rand_f(2.0f, 4.0f) * (speed * 0.2f + 1.0f), 0, 0);
		}
		else // 直進時
		{
			particleVelocity += _rightVec * sidePower * (speed * 0.1f + 0.8f);
			particleVelocity += XMVectorSet(0, Rand_f(0.5f, 1.5f) * (speed * 0.1f + 0.5f), 0, 0);
		}
		
		// 再利用できるパーティクルを探す
		bool recycle = false;
		for (auto& p : *m_pParticles)
		{
			if (p.particleState == Stop)
			{
				p.position = spawnPos;
				p.velocity = particleVelocity;
				p.color = m_StartColor;
				p.startColor = m_StartColor;
				p.endColor = m_EndColor;
				p.colorChangeRate = m_ColorChangeRate;
				p.lifeTime = Rand_f(m_Min_LifeTime, m_Max_LifeTime);
				p.elapsedTime = 0.0f;
				p.gravity = m_Gravity;
				p.particleState = m_State;
				recycle = true;
				break;
			}
		}
		// 再利用できなければ新規作成(上限超えないように)
		if (!recycle && m_pParticles->size() < m_MaxParticles)
		{
			Particle p;
			p.position = spawnPos;
			p.velocity = particleVelocity;
			p.color = m_StartColor;
			p.startColor = m_StartColor;
			p.endColor = m_EndColor;
			p.colorChangeRate = m_ColorChangeRate;
			p.lifeTime = Rand_f(m_Min_LifeTime, m_Max_LifeTime);
			p.elapsedTime = 0.0f;
			p.gravity = m_Gravity;
			p.particleState = m_State;

			m_pParticles->push_back(p);
		}
	}
}