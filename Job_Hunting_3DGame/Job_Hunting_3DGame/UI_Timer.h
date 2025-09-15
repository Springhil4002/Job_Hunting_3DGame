#pragma once
#include "UI.h"
#include "Game.h"

struct UV_Rect
{
	float u0;
	float v0;
	float u1;
	float v1;
};

class UI_Timer : public UI
{
private:
	UV_Rect m_UVs[11] = {};
	Game* m_Game;
public:
	UI_Timer() = default;
	~UI_Timer() = default;

	std::unique_ptr<Object> clone() const override;

	bool Init(Camera2D* _cameraUI, float _width, float _height,
		const std::wstring& _filePath, Game* _game);
	void Update() override;
	void Draw() override;
	void Uninit() override;
 	void SetNum(int _num);
};