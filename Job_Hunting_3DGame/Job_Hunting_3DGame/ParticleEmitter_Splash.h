#pragma once
#include "ParticleEmitter.h"
#include "ParticleSystem_Splash.h"

class ParticleEmitter_Splash : public ParticleEmitter
{
public:
	ParticleEmitter_Splash(Camera* _camera);
	bool Init() override;
	void Update() {};
	void Update(float _deltaTime, 
		DirectX::XMVECTOR _pos, DirectX::XMVECTOR _rightVec, 
		DirectX::XMVECTOR _velocity, bool _moving, float _state);
	void Draw() override;
	
	bool Init_Prop();
	void Init_Param();
	void CreateParticles() {};
	void CreateParticles(DirectX::XMVECTOR _centerPos,
		DirectX::XMVECTOR _rightVec, DirectX::XMVECTOR _velocity,float _state);
};