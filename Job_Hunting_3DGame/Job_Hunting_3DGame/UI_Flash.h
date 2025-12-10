#pragma once
#include "UI.h"
#include "UI_FADE_STATE.h"

class UI_Flash : public UI
{
private:
	float m_FlashSpeed = 0.0f;	// 点滅速度
	bool m_IsFlashing = true;	// 点滅フラグ
	FADE_STATE m_State = FADE_STATE::FADE_STATE_OUT;
public:
	UI_Flash() = default;
	~UI_Flash() = default;

	std::unique_ptr<Object> clone() const override;

	bool Init(Camera2D* _cameraUI, float _width, float _height,
		const std::wstring& _filePath) override;
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void Update_Flash();
	void SetFlashIn();
	void SetFlashOut();
	void SetFlashNone();
	void SetFlashSpeed(float _speed);
	float GetAlpha() const { return m_Alpha; }
};