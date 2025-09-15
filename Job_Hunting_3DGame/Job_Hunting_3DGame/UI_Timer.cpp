#include "UI_Timer.h"
#include "Debug_New.h"

std::unique_ptr<Object> UI_Timer::clone() const
{
	auto newObj = std::make_unique<UI_Timer>();
	return newObj;
}

bool UI_Timer::Init(Camera2D* _cameraUI, float _width, float _height,
	const std::wstring& _filePath, Game* _game)
{
	m_Game = _game;
	if (!UI::Init(_cameraUI, _width, _height, _filePath))
	{
		printf("UI_Timer:‰Šú‰»ˆ—¸”s\n");
		return false;
	}

	const int totalNum = 11;
	for (int i = 0; i < totalNum; ++i)
	{
		float u0 = i / static_cast<float>(totalNum);
		float u1 = (i + 1) / static_cast<float>(totalNum);
		m_UVs[i] = { u0,0.0f,u1,1.0f };
	}

	printf("UI_Timer:UV•ªŠ„¬Œ÷\n");
	return true;
}

void UI_Timer::Update()
{
	UI::Update();
}

void UI_Timer::Draw()
{
	UI::Draw();
}

void UI_Timer::Uninit()
{
	m_Game = nullptr;
	UI::Uninit();
}

void UI_Timer::SetNum(int _num)
{
	if (_num < 0 || _num>10) return;

	/*auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	UV_Rect& uv = m_UVs[_num];
	ptr->uv = XMFLOAT4(uv.u0, uv.v0, uv.u1, uv.v1);*/
}