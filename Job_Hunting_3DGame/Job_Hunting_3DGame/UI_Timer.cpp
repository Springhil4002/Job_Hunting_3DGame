#include "UI_Timer.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

using namespace DirectX;

std::unique_ptr<Object> UI_Timer::clone() const
{
	auto newObj = std::make_unique<UI_Timer>();
	return newObj;
}

bool UI_Timer::Init(Camera2D* _cameraUI, float _width, float _height,
	const std::wstring& _texPath)
{
	// UVを初期化
	for (int i = 0; i < 11; ++i)
	{
		m_UVs[i].x0 = i / 11.0f;
		m_UVs[i].y0 = 0.0f;
		m_UVs[i].x1 = (i + 1) / 11.0f;
		m_UVs[i].y1 = 1.0f;
	}

	// 時間計測用UIを作成
	for (int i = 0; i < m_TimeDigits.size(); ++i)
	{
		m_TimeDigits[i] = std::make_unique<UI>();
		m_TimeDigits[i]->Init(_cameraUI, _width, _height, _texPath);
		float offsetX = 630.0f + i * 40.0f;
		m_TimeDigits[i]->SetTransform(
			XMVectorSet(offsetX, 480.0f, 0.0f, 0.0f),
			XMVectorZero(),
			XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 1.0f);
	}

	// カウントダウン用UIの作成
	m_CountDownDigit = std::make_unique<UI>();
	m_CountDownDigit->Init(_cameraUI, _width, _height, _texPath);
	m_CountDownDigit->SetTransform(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorZero(),
		XMVectorSet(4.0f, 4.0f, 1.0f, 0.0f), 1.0f);

	// GoUI作成
	m_GoUI = std::make_unique<UI>();
	m_GoUI->Init(_cameraUI, _width, _height, L"Assets/Texture/UI_Go.png");
	m_GoUI->SetTransform(
		XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		XMVectorZero(),
		XMVectorSet(4.0f, 3.0f, 1.0f, 0.0f), 1.0f);

	DEBUG_LOG(L"UI_Timer:初期化処理に成功");
	return true;
}

void UI_Timer::Update()
{
	if (!m_Game) return;

	RACE_STATE state = m_Game->GetState();
	switch(state)
	{
	case RACE_STATE::RACE_STATE_COUNTDOWN:
	{
		int countDown = std::max(m_Game->GetCountDownRemaining(), 0);
		SetDigit(m_CountDownDigit.get(), countDown);
		m_CountDownDigit->Update();
		break;
	}
	case RACE_STATE::RACE_STATE_RUNNING:
	{
		m_GoUI->Update();
		m_GoAlpha -= m_GoFadeSpeed * (1.0f / 144.0f);
		m_GoUI->SetAlpha(std::max(0.0f, m_GoAlpha));

		SetTime(m_Game->GetRemainingTime());
		for (auto& digit : m_TimeDigits) digit->Update();
		break;
	}
	default:
		break;
	}
}

void UI_Timer::Draw()
{
	if (!m_Game) return;
	RACE_STATE state = m_Game->GetState();
	switch (state)
	{
	case RACE_STATE::RACE_STATE_COUNTDOWN:
	{
		m_CountDownDigit->Draw();
		break;
	}
	case RACE_STATE::RACE_STATE_RUNNING:
	{
		if(m_GoAlpha > 0.0f)
			m_GoUI->Draw();
		for (auto& digit : m_TimeDigits) 
			digit->Draw();
		break;
	}
	}
}

void UI_Timer::UnInit()
{
	for (auto& digit : m_TimeDigits)
	{
		digit->UnInit();
		digit.reset();	
	}
	m_CountDownDigit->UnInit();
	m_CountDownDigit.reset();
	m_GoUI->UnInit();
	m_GoUI.reset();
	m_Game = nullptr;
}

void UI_Timer::SetDigit(UI* _digit, int _number)
{
	if (!_digit) return;
	if (_number < 0 || _number > 10) return;

	const auto& uv = m_UVs[_number];
	_digit->SetUV({ uv.x0,uv.y0,uv.x1,uv.y1 });
}

void UI_Timer::SetTime(std::chrono::milliseconds _elapsed)
{
	// ミリ秒未満にならないように
	int totalMillis = std::max<int>(0, static_cast<int>(_elapsed.count()));
	// 分、秒、センチに変換
	int min = totalMillis / 60000;
	int sec = (totalMillis / 1000) % 60;
	int centi = (totalMillis % 1000) / 10;

	// フォーマット:mm:ss:㏄
	char buffer[9];
	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", min, sec, centi);

	for (int i = 0; i < 8; ++i)
	{
		if (buffer[i] == ':')
			SetDigit(m_TimeDigits[i].get(), 10);
		else
			SetDigit(m_TimeDigits[i].get(), buffer[i] - '0');
	}
}

void UI_Timer::SetGame(Game* _game)
{
	m_Game=_game;
}