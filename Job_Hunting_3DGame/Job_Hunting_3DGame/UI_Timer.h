#pragma once
#include "UI.h"
#include "Game.h"
#include <array>

class UI_Timer : public UI
{
private:
	struct UV_Rect
	{
		float x0;
		float y0;
		float x1;
		float y1;
	};
	std::array<UV_Rect, 11> m_UVs = {};	
	std::array<std::unique_ptr<UI>, 8> m_TimeDigits;
	std::unique_ptr<UI> m_CountDownDigit;
	std::unique_ptr<UI> m_GoUI;
	Game* m_Game = nullptr;
	float m_GoFadeSpeed = 1.0f;
	float m_GoAlpha = 1.0f;
public:
	std::unique_ptr<Object> clone() const override;

	bool Init(Camera2D* _cameraUI, float _width, float _height, 
		const std::wstring& _texPath);
	void Update() override;
	void Draw() override;
	void Uninit() override;

	void SetTime(std::chrono::milliseconds _elapsed);
	void SetDigit(UI* _digit, int number);
	void SetGame(Game* _game);
};