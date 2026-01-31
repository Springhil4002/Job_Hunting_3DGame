#pragma once
#include "UI.h"
#include "UI_Flash.h"
#include "Shop.h"
#include <array>
#include <memory>

class UI_Shop : public UI
{
private:
	Shop* m_pShop = nullptr;
	
	struct ShopLine
	{
		std::unique_ptr<UI> statusNameUI;			// ステータス名テキスト
		std::unique_ptr<UI> levelTextUI;			// “レベル“テキスト
		std::array<std::unique_ptr<UI>, 2> levelUI;	// レベル数字UI
		std::unique_ptr<UI> costTextUI;				// “Cost“テキスト
		std::array<std::unique_ptr<UI>, 6> costUI;	// コスト数字UI
		std::unique_ptr<UI> upgradeUI;				// “強化“テキスト
	};
	// 4種類の強化項目
	std::array<ShopLine, 4> m_Lines;
	// 遷移先メニューのUI
	std::unique_ptr<UI> m_ReplayUI;
	std::unique_ptr<UI> m_TitleUI;
	// 選択中のカーソル
	std::unique_ptr<UI_Flash> m_Cursor;

	// 数字UIに値を設定する関数
	void UpdateDigits(std::unique_ptr<UI>* _uiArray,
		int _digitCount, int _value, float _baseX, float _y);
public:
	/// @brief クローン関数
	std::unique_ptr<Object> clone() const override;

	/// @brief 初期化
	/// @param _cameraUI カメラ2D
	/// @param _shop Shopクラス
	/// @return 初期化の成否
	bool Init(Camera2D* _cameraUI, Shop* _shop);
	/// @brief 更新関数
	void Update() override;
	/// @brief 描画関数
	void Draw() override;
	/// @brief 終了関数
	void UnInit() override;
};