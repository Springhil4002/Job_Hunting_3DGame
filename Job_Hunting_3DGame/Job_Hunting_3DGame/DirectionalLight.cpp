#include "DirectionalLight.h"

using namespace DirectX;

static DirectionalLight m_Instance;

DirectionalLight& DirectionalLight::Instance()
{
	return m_Instance;
}

DirectionalLight::DirectionalLight()
{
	m_LightData.lightDir = XMFLOAT3(0.5f, -1.0f, 0.15f);
	m_LightData.envStrength = 0.65f;
	m_LightData.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void DirectionalLight::UpdateLightFollowCamera(Camera* _camera)
{
	XMVECTOR camForward = XMVector3Normalize(_camera->GetForward());

	XMFLOAT3 lightDir;
	XMStoreFloat3(&lightDir, XMVectorNegate(camForward));

	SetLightDir(lightDir);
}

void DirectionalLight::SetLightDir(const XMFLOAT3& _dir)
{
	XMVECTOR vec = XMVector3Normalize(XMLoadFloat3(&_dir));
	XMStoreFloat3(&m_LightData.lightDir, vec);
}

void DirectionalLight::SetEnvStrength(float _strength)
{
	m_LightData.envStrength = _strength;
}

void DirectionalLight::SetLightColor(const XMFLOAT4& _color)
{
	m_LightData.lightColor = _color;
}

const DirectionalLightData& DirectionalLight::GetLightData() const
{
	return m_LightData;
}