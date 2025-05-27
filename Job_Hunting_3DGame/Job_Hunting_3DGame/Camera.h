#pragma once
#include <DirectXMath.h>

class Camera
{
private:
	DirectX::XMVECTOR m_eye;
	DirectX::XMVECTOR m_target;
	DirectX::XMVECTOR m_up;
	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX m_proj;

	void UpdateViewMatrix();

public:
	Camera();
	void SetPos(const DirectX::XMVECTOR& _eye);
	void SetTarget(const DirectX::XMVECTOR& _target);
	void SetUp(const DirectX::XMVECTOR& _up);
	void SetPerspective(float _fovY, float _aspect, float _nearZ, float _farZ);

	const DirectX::XMMATRIX& GetViewMatrix() const;
	const DirectX::XMMATRIX& GetProjMatrix() const;
};

