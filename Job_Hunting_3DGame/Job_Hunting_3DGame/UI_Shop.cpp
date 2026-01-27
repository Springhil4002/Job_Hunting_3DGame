#include "UI_Shop.h"
#include "SceneManager.h"
#include "Debug_New.h"

namespace ShopLayout 
{
	static const int STATUS_COUNT = static_cast<int>(UPGRADE_TYPE::TYPE_COUNT);
	
	static const float BASE_POS_Y = 350.0f;		// 1行目の高さ基準
	static const float LINE_SPACE = 150.0f;		// 行間隔
	static const float MENU_POS_Y = -250.0f;	// 遷移先メニューの高さ基準

	// 各UIのX座標オフセット
	static const float UI_NAME		= -500.0f;
	static const float UI_LV_TEXT	= -220.0f;
	static const float UI_LV_NUM	= -120.0f;
	static const float UI_COST		= 150.0f;
	static const float UI_COST_NUM	= 240.0f;
	static const float UI_UPGRADE	= 600.0f;
	static const float UI_REPLAY	= -300.0f;
	static const float UI_TITLE		= 300.0f;

	// 数字UIの設定
	static const int DIGIT_LEVEL = 2;
	static const int DIGIT_COST = 6;
	static const float DIGIT_SPACE = 30.0f;
}

std::unique_ptr<Object> UI_Shop::clone() const
{
	auto newObj = std::make_unique<UI_Shop>();
	return newObj;
}

bool UI_Shop::Init(Camera2D* _cameraUI, Shop* _shop)
{
	if (!_shop) return false;
	m_pShop = _shop;

	// 各ステータスのテクスチャパス配列
	std::wstring statusTexturePaths[] = {
		L"Assets/Texture/UI_MaxSpeed.png",
		L"Assets/Texture/UI_Acceleration.png",
		L"Assets/Texture/UI_ScoreUp.png",
		L"Assets/Texture/UI_GoalCreate.png",
	};

	for (int i = 0; i < ShopLayout::STATUS_COUNT; ++i)
	{
		float currentPosY = ShopLayout::BASE_POS_Y - (i * ShopLayout::LINE_SPACE);
		auto& line = m_Lines[i];

		// ステータス名
		line.statusNameUI = std::make_unique<UI>();
		line.statusNameUI->Init(
			_cameraUI, 400.0f, 80.0f, statusTexturePaths[i].c_str());
		line.statusNameUI->SetPos(
			DirectX::XMVectorSet(ShopLayout::UI_NAME, currentPosY, 0.0f, 0.0f));

		// レべル
		line.levelTextUI = std::make_unique<UI>();
		line.levelTextUI->Init(
			_cameraUI, 150.0f, 80.0f, L"Assets/Texture/UI_Level.png");
		line.levelTextUI->SetPos(
			DirectX::XMVectorSet(ShopLayout::UI_LV_TEXT, currentPosY, 0.0f, 0.0f));
		for (auto& lv : line.levelUI)
		{
			lv = std::make_unique<UI>();
			lv->Init(_cameraUI, 48.0f, 80.0f, L"Assets/Texture/UI_Number.png");
		}

		// コスト
		line.costTextUI = std::make_unique<UI>();
		line.costTextUI->Init(
			_cameraUI, 150.0f, 80.0f, L"Assets/Texture/UI_Cost.png");
		line.costTextUI->SetPos(
			DirectX::XMVectorSet(ShopLayout::UI_COST, currentPosY, 0.0f, 0.0f));
		for(auto& c : line.costUI)
		{
			c = std::make_unique<UI>();
			c->Init(_cameraUI, 48.0f, 80.0f, L"Assets/Texture/UI_Number.png");
		}

		// 強化テキスト
		line.upgradeUI = std::make_unique<UI>();
		line.upgradeUI->Init(
			_cameraUI, 120.0f, 60.0f, L"Assets/Texture/UI_Upgrade_Idle.png");
		line.upgradeUI->SetPos(
			DirectX::XMVectorSet(ShopLayout::UI_UPGRADE, currentPosY, 0.0f, 0.0f));
	}

	m_ReplayUI = std::make_unique<UI>();
	m_ReplayUI->Init(_cameraUI, 300.0f, 100.0f, L"Assets/Texture/UI_Replay.png");
	m_ReplayUI->SetPos(DirectX::XMVectorSet(ShopLayout::UI_REPLAY, ShopLayout::MENU_POS_Y, 0, 0));
	
	m_TitleUI = std::make_unique<UI>();
	m_TitleUI->Init(_cameraUI, 300.0f, 100.0f, L"Assets/Texture/UI_Title.png");
	m_TitleUI->SetPos(DirectX::XMVectorSet(ShopLayout::UI_TITLE, ShopLayout::MENU_POS_Y, 0, 0));

	// 選択中の枠UI
	m_Cursor = std::make_unique<UI>();
	if (!m_Cursor->Init(_cameraUI, 160.0f, 80.0f, L"Assets/Texture/UI_CurrentSelect.png"))
	{
		return false;
	}

	return true;
}

void UI_Shop::Update()
{
	auto focus = m_pShop->GetFocus();
	auto& allData = m_pShop->GetAllData();

	if (focus == SHOP_FOCUS::FOCUS_UPGRADE)
	{
		int index = m_pShop->GetUpgradeIndex();
		float cursorPosY = ShopLayout::BASE_POS_Y - (index * ShopLayout::LINE_SPACE);
		m_Cursor->SetPos(DirectX::XMVectorSet(ShopLayout::UI_UPGRADE, cursorPosY, 0.0f, 0.0f));
		m_Cursor->SetScale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)); 
	}
	else
	{
		int index = m_pShop->GetMenuIndex();
		float menuPosX;
		if (index == 0)
		{
			menuPosX = ShopLayout::UI_REPLAY;
			m_Cursor->SetPos(DirectX::XMVectorSet(menuPosX, ShopLayout::MENU_POS_Y, 0, 0));
			m_Cursor->SetScale(DirectX::XMVectorSet(2.0f, 1.3f, 1.0f, 0.0f));
		}
		else
		{
			menuPosX = ShopLayout::UI_TITLE;
			m_Cursor->SetPos(DirectX::XMVectorSet(menuPosX, ShopLayout::MENU_POS_Y, 0, 0));
			m_Cursor->SetScale(DirectX::XMVectorSet(2.0f, 1.3f, 1.0f, 0.0f));
		}
	}

	m_Cursor->Update();
	m_ReplayUI->Update();
	m_TitleUI->Update();

	for (int i = 0; i < ShopLayout::STATUS_COUNT; ++i)
	{
		auto& line = m_Lines[i];
		const auto& data = allData[i];
		float currentPosY = ShopLayout::BASE_POS_Y - (i * ShopLayout::LINE_SPACE);
		
		// 選択中の行のみ拡大表示
		bool select = (focus == SHOP_FOCUS::FOCUS_UPGRADE && i == m_pShop->GetUpgradeIndex());

		if (select)
			line.upgradeUI->SetScale(DirectX::XMVectorSet(1.3f, 1.3f, 1.0f, 0.0f));
		else 
			line.upgradeUI->SetScale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));

		// レベル数字の更新
		UpdateDigits(line.levelUI.data(), ShopLayout::DIGIT_LEVEL, 
			data.currentLevel, ShopLayout::UI_LV_NUM, currentPosY);
		// コスト数字の更新
		UpdateDigits(line.costUI.data(), ShopLayout::DIGIT_COST,
			data.cost, ShopLayout::UI_COST_NUM, currentPosY);

		// 各UIの更新
		line.statusNameUI->Update();
		line.levelTextUI->Update();
		for (auto& lv : line.levelUI) lv->Update();
		line.costTextUI->Update();
		for (auto& c : line.costUI) c->Update();
		line.upgradeUI->Update();
	}
}

void UI_Shop::Draw()
{
	m_Cursor->Draw();
	m_ReplayUI->Draw();
	m_TitleUI->Draw();
	for (auto& line : m_Lines)
	{
		line.statusNameUI->Draw();
		line.levelTextUI->Draw();
		for (auto& lv : line.levelUI) lv->Draw();
		line.costTextUI->Draw();
		for (auto& c : line.costUI) c->Draw();
		line.upgradeUI->Draw();
	}
}

void UI_Shop::UnInit()
{
	m_Cursor->UnInit();
	m_ReplayUI->UnInit();
	m_TitleUI->UnInit();
	for (auto& line : m_Lines)
	{
		line.statusNameUI->UnInit();
		line.levelTextUI->UnInit();
		for (auto& lv : line.levelUI) lv->UnInit();
		line.costTextUI->UnInit();
		for (auto& c : line.costUI) c->UnInit();
		line.upgradeUI->UnInit();
	}
	m_pShop = nullptr;
}

void UI_Shop::UpdateDigits(std::unique_ptr<UI>* _uiArray,
	int _digitCount, int _value, float _baseX, float _y)
{
	int temp = _value;
	const float count = 11.0f;

	for (int i = _digitCount - 1; i >= 0; --i)
	{
		int number = temp % 10;
		temp /= 10;
		float posX = _baseX + (i * ShopLayout::DIGIT_SPACE);

		float x0 = number / count;
		float x1 = (number + 1) / count;

		_uiArray[i]->SetPos(DirectX::XMVectorSet(posX, _y, 0.0f, 0.0f));
		_uiArray[i]->SetUV(DirectX::XMFLOAT4(x0, 0.0f, x1, 1.0f));
		_uiArray[i]->Update();
	}
}