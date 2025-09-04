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
	XMVECTOR _pos, XMVECTOR _rightVec, bool _moving)
{
	m_Time += _deltaTime;

	if (_moving && m_Time >= m_SpawnParticlesRate)
	{
		CreateParticles(_pos, _rightVec);
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
	SetCreateParticleRate(0.01f);
	SetMaxParticles(m_pParticleSystem->GetMaxParticles());
	m_StartColor = XMFLOAT4(0.3f, 0.8f, 1.0f, 1.0f);
	m_EndColor = XMFLOAT4(0.3f, 0.8f, 1.0f, 0.0f);
	m_ColorChangeRate = 0.8f;
	m_Time = 0.0f;
	m_Min_LifeTime = 1.0f;
	m_Max_LifeTime = 5.0f;
	m_Gravity = -9.8f;
	m_State = Active;
}

void ParticleEmitter_Splash::CreateParticles(XMVECTOR _centerPos, XMVECTOR _rightVec)
{
	if (!m_pParticles) return;

	int num = Rand_Int(m_Min_CreateParticles, m_Max_CreateParticles);
	
	// 生成範囲のスケール
	const float areaX = 0.5f;
	const float areaY = 0.1f;
	const float areaZ = 0.5f;

	// 左右のオフセット位置
	const float sideOffset = 1.0f;
	XMVECTOR leftPos = _centerPos - _rightVec * sideOffset;
	XMVECTOR rightPos = _centerPos + _rightVec * sideOffset;
	
	for (int i = 0; i < num; ++i)
	{
		// 奇数:左、偶数:右
		XMVECTOR basePos = (i % 2 == 0) ? rightPos : leftPos;

		XMVECTOR randOffset = XMVectorSet(
			Rand_f(-areaX, areaX),
			Rand_f(-areaY, areaY),
			Rand_f(-areaZ, areaZ),
			0.0f
		);

		XMVECTOR spawnPos = basePos + randOffset;

		// 再利用できるパーティクルを探す
		bool recycle = false;
		for (auto& p : *m_pParticles)
		{
			if (p.particleState == Stop)
			{
				p.position = spawnPos;
				p.velocity = XMVectorSet(Rand_f(-0.5f, 0.5f), Rand_f(0.8f, 1.2f), Rand_f(-0.5f, 0.5f), 0.0f);
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
			p.velocity = XMVectorSet(Rand_f(-0.5f, 0.5f), Rand_f(0.8f, 1.2f), Rand_f(-0.5f, 0.5f), 0.0f);
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