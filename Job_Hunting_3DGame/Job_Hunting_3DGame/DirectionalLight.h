#pragma once
#include <DirectXMath.h>
#include "Camera.h"

struct DirectionalLightData {
	DirectX::XMFLOAT3 lightDir;		// ライト方向
	float envStrength;				// ライトの強さ
	DirectX::XMFLOAT4 lightColor;	// ライトカラー
};

class DirectionalLight
{
private:
	// ライトデータ
	DirectionalLightData m_LightData;
public:
	/// @brief コンストラクタ
	DirectionalLight();
	
	void UpdateLightFollowCamera(Camera* _camera);

	// インスタンス取得関数
	static DirectionalLight& Instance();
	
	// セッター関数
	void SetLightDir(const DirectX::XMFLOAT3& _dir);
	void SetEnvStrength(float _strength);
	void SetLightColor(const DirectX::XMFLOAT4& _color);
	
	// ゲッター関数
	const DirectionalLightData& GetLightData() const;
};

