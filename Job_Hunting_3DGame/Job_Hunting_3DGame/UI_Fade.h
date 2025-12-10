#pragma once
#include "UI.h"
#include "UI_FADE_STATE.h"

class UI_Fade : public UI
{
private:
	float m_FadeSpeed = 0.0f;		// フェード速度	
	bool m_FadeFinished = false;	// フェード完了フラグ
	FADE_STATE m_State = FADE_STATE::FADE_STATE_NONE;
public:
	UI_Fade() = default;
	~UI_Fade() = default;

	std::unique_ptr<Object> clone() const override;

	bool Init(Camera2D* _cameraUI, float _width,float _height);
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void Update_Fade();
	void SetFadeIn();
	void SetFadeOut();
	void SetFadeNone();
	void SetFadeSpeed(float _speed);
	bool IsFadeFinished() const;
	FADE_STATE GetState() const;
};