#include "ParticleEmitter.h"

using namespace DirectX;

float ParticleEmitter::Rand_f(float _min, float _max)
{
	std::uniform_real_distribution<float> dist(_min, _max);
	return dist(m_Mt);
}

int ParticleEmitter::Rand_Int(int _min, int _max)
{
	std::uniform_int_distribution<int> dist(_min, _max);
	return dist(m_Mt);
}