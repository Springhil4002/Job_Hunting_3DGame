#include "UI_Speed.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> UI_Speed::clone() const
{
	auto newObj = std::make_unique<UI_Speed>();
	return newObj;
}

bool UI_Speed::Init(Camera2D* _cameraUI, float _width, float _height,
	const std::wstring& _numTexPath, const std::wstring& _unitTexPath)
{
	// 数字テクスチャのUV(0,1,2,3,4,5,6,7,8,9,:)に合わせて計算
	for (int i = 0; i < 11; ++i)
	{
		m_UV_Numbers[i].x0 = i / 11.0f;
		m_UV_Numbers[i].y0 = 0.0f;
		m_UV_Numbers[i].x1 = (i + 1) / 11.0f;
		m_UV_Numbers[i].y1 = 1.0f;
	}

	// 3桁速度表示用UIの作成
	for (int i = 0; i < 3; ++i)
	{
		m_SpeedDigits[i] = std::make_unique<UI>();
		// UIの初期化
		if (!m_SpeedDigits[i]->Init(_cameraUI, _width, _height, _numTexPath)) return false;
		
		// 画面右下への配置計算
		float offsetX = 650.0f + i * (_width * 0.8f);
		m_SpeedDigits[i]->SetTransform(
			XMVectorSet(offsetX, -450.0f, 0.0f, 0.0f),
			XMVectorZero(),
			XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 1.0f);
	}

	// 速度単位表示UIの作成
	m_UnitUI = std::make_unique<UI>();
	// 単位表示UIの初期化
	if (!m_UnitUI->Init(_cameraUI, _width * 2.0f, _height * 0.8f, _unitTexPath)) return false;
	
	m_UnitUI->SetTransform(
		XMVectorSet(820.0f, -460.0f, 0.0f, 0.0f),
		XMVectorZero(),
		XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), 1.0f);
	
	return true;
}

void UI_Speed::Update()
{
	if (!m_PlayerController) return;
	
	// プレイヤーの速度ベクトルからスカラー値を取得
	XMVECTOR velocity = m_PlayerController->GetVelocity();
	float speed = XMVectorGetX(XMVector3Length(velocity));

	// ゲーム内速度をスケーリング
	int displaySpeed = static_cast<int>(speed);
	displaySpeed = std::clamp(displaySpeed, 0, 999);

	// 3桁の数字に分解してセット
	SetDigit(m_SpeedDigits[0].get(), (displaySpeed / 100) % 10);
	SetDigit(m_SpeedDigits[1].get(), (displaySpeed / 10) % 10);
	SetDigit(m_SpeedDigits[2].get(), displaySpeed % 10);
	
	// UI更新
	for (auto& digit : m_SpeedDigits)
	{
		digit->Update();
	}
	m_UnitUI->Update();
}

void UI_Speed::Draw()
{
	// 数字3桁を描画
	for (auto& digit : m_SpeedDigits)
	{
		digit->Draw();
	}
	// 単位UIを描画
	m_UnitUI->Draw();
}

void UI_Speed::UnInit()
{
	for (auto& d : m_SpeedDigits) 
	{
		if (d)
		{
			d->UnInit();
			d.reset();
		}
	}
	if (m_UnitUI)
	{
		m_UnitUI->UnInit();
		m_UnitUI.reset();
	}
}

void UI_Speed::Set_PlayerController(PlayerController* _playerController)
{
	m_PlayerController = _playerController;
}

void UI_Speed::SetDigit(UI* _digit, int _number)
{
	if (!_digit || _number < 0 || _number>10) return;
	const auto& uv = m_UV_Numbers[_number];
	_digit->SetUV({ uv.x0,uv.y0,uv.x1,uv.y1 });
}