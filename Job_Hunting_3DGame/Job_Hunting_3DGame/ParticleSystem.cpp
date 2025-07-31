#include "ParticleSystem.h"

using namespace DirectX;

void ParticleSystem::Reset() 
{
	m_Position = XMVectorZero();
	m_Velocity = XMVectorZero();
	m_Color = m_StartColor;
	m_Time = 0.0f;
	m_Deg = 0.0f;
}

bool ParticleSystem::IsActive() const 
{
	return m_State == Active; 
}

float ParticleSystem::colorLerp(float _startColor, float _endColor, float _time)
{
	return _startColor + (_endColor - _startColor) * _time;
}