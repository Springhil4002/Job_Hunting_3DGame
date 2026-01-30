#include "UI_Score.h"
#include "SceneManager.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

using namespace DirectX;

std::unique_ptr<Object> UI_Score::clone() const
{
	auto newObj = std::make_unique<UI_Score>();
	return newObj;
}

bool UI_Score::Init(Camera2D* _cameraUI, float _width, float _height,
	const std::wstring& _numTexPath, const std::wstring& _scoreTexPath)
{
	// UV座標の計算(0,1,2,3,4,5,6,7,8,9,:)
	for (int i = 0; i < 11; ++i)
	{
		m_UV_Numbers[i].x0 = i / 11.0f;
		m_UV_Numbers[i].y0 = 0.0f;
		m_UV_Numbers[i].x1 = (i + 1) / 11.0f;
		m_UV_Numbers[i].y1 = 1.0f;
	}

	// 8桁スコアの作成
	for (int i = 0; i < 8; ++i)
	{
		m_ScoreDigits[i] = std::make_unique<UI>();
		if (!m_ScoreDigits[i]->Init(_cameraUI, _width, _height, 
			_numTexPath))return false;
		
		// 画面左上への配置計算
		float offsetX = -700.0f + i * (_width * 0.8f);
		m_ScoreDigits[i]->SetTransform(
			XMVectorSet(offsetX, 500.0f, 0.0f, 0.0f),
			XMVectorZero(),
			XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 1.0f);
	}
	m_ScoreUI = std::make_unique<UI>();
	// 単位表示UIの初期化
	if (!m_ScoreUI->Init(_cameraUI, _width * 5.0f, _height, _scoreTexPath)) return false;

	m_ScoreUI->SetTransform(
		XMVectorSet(-840.0f, 500.0f, 0.0f, 0.0f),
		XMVectorZero(),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 1.0f);

	DEBUG_LOG(L"UI_Score:初期化処理に成功");
	return true;
}

void UI_Score::Update()
{
	m_Score = SceneManager::GetGameStatus().score;
	m_Score = std::clamp(m_Score, 0, 99999999);

	// 各桁に数値をセット
	SetDigit(m_ScoreDigits[0].get(), (m_Score / 10000000) % 10);	
	SetDigit(m_ScoreDigits[1].get(), (m_Score / 1000000) % 10);
	SetDigit(m_ScoreDigits[2].get(), (m_Score / 100000) % 10);
	SetDigit(m_ScoreDigits[3].get(), (m_Score / 10000) % 10);
	SetDigit(m_ScoreDigits[4].get(), (m_Score / 1000) % 10);
	SetDigit(m_ScoreDigits[5].get(), (m_Score / 100) % 10);
	SetDigit(m_ScoreDigits[6].get(), (m_Score / 10) % 10);
	SetDigit(m_ScoreDigits[7].get(), m_Score % 10);			

	// UI更新
	for (auto& digit : m_ScoreDigits)
	{
		digit->Update();
	}
	m_ScoreUI->Update();
}

void UI_Score::Draw()
{
	for (auto& digit : m_ScoreDigits)
	{
		digit->Draw();
	}
	// 単位UIを描画
	m_ScoreUI->Draw();
}

void UI_Score::UnInit()
{
	for (auto& digit : m_ScoreDigits)
	{
		if (digit)
		{
			digit->UnInit();
			digit.reset();
		}
	}
	if (m_ScoreUI)
	{
		m_ScoreUI->UnInit();
		m_ScoreUI.reset();
	}
}

void UI_Score::SetDigit(UI* _digit, int _number)
{
	if (!_digit || _number < 0 || _number>10) return;
	const auto& uv = m_UV_Numbers[_number];
	_digit->SetUV({ uv.x0,uv.y0,uv.x1,uv.y1 });
}