#pragma once
#include <DirectXMath.h>

class Camera2D
{
private:
	float m_X;
	float m_Y;
	float m_ScreenWidth;
	float m_ScreenHeight;

	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projMatrix;
	DirectX::XMMATRIX m_viewProjMatrix;

	void UpdateViewMatrix();
public:
	Camera2D();
	~Camera2D() = default;
	/// @brief 初期化処理
	/// @param _width 横幅
	/// @param _height 縦幅
	void Init(float _width, float _height);
	/// @brief 位置設定
	/// @param _x X座表
	/// @param _y Y座表
	void SetPos(float _x, float _y);
	/// @brief 各種行列取得関数
	const DirectX::XMMATRIX& GetViewMatrix() const { return m_viewMatrix; }
	const DirectX::XMMATRIX& GetProjMatrix() const { return m_projMatrix; }
	const DirectX::XMMATRIX& GetViewProjMatrix() const { return m_viewProjMatrix; }
};

