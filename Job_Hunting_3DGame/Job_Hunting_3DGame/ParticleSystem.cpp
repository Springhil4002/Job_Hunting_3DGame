#include "ParticleSystem.h"

using namespace DirectX;

void Particle::Reset()
{
	position = XMVectorZero();
	velocity = XMVectorZero();
	color = startColor;
	elapsedTime = 0.0f;
}

bool Particle::IsActive() const
{
	return particleState == Active;
}

XMFLOAT4 Particle::LerpColor(const XMFLOAT4& _startColor, const XMFLOAT4& _endColor, float _time)
{
	return XMFLOAT4(
		_startColor.x + (_endColor.x - _startColor.x) * _time,
		_startColor.y + (_endColor.y - _startColor.y) * _time,
		_startColor.z + (_endColor.z - _startColor.z) * _time,
		_startColor.w + (_endColor.w - _startColor.w) * _time
	);
}