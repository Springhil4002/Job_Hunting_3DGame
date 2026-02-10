#pragma once
#include "UI.h"

class UI_ActiveLine : public UI
{
private:
	float m_Timer = 0.0f;		
	float m_SpeedRatio = 0.0f;	// 最高速度の割合
	float m_AlphaStart = 0.0f;	// 透明度可視化の基準値
	float m_AlphaRatio = 0.0f;	// 透明度の割合
	float m_AnimSpeed = 0.0f;	// UVが切り替わる間隔
	int m_TileX = 0;			// UVのXの数
	int m_TileY = 0;			// UVのYの数
public:
	UI_ActiveLine() = default;
	~UI_ActiveLine() = default;

	/// @brief クローン関数
	std::unique_ptr<Object> clone() const override;

	bool Init(Camera2D* _cameraUI, float _width, float _height,
		const std::wstring& _filePath,float _speedRatio);
	/// @brief 更新処理
	void Update(float _deltaTime);
	/// @brief 描画処理
	void Draw() override;
	/// @brief 終了処理
	void UnInit() override;

	/// @brief UVアニメーション更新関数
	void Update_AnimUV(float _deltaTime);
	/// @brief 最高速度の割合取得関数
	/// @param _speedRatio 最大速度の割合
	void SetSpeedRatio(float _speedRatio) { m_SpeedRatio = _speedRatio; }
};