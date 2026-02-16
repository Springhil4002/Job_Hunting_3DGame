#pragma once
#include <vector>
#include <string>
#include "GameStatus.h"

enum class SHOP_FOCUS
{
	FOCUS_UPGRADE,	// 強化項目
	FOCUS_MENU,		// 遷移先メニュー
};

/// @brief 強化するステータスの種類
enum class UPGRADE_TYPE
{
	TYPE_SPEED,
	TYPE_ACCEL,
	TYPE_SCOREUP,
	TYPE_GOAL,
	TYPE_COUNT,	// 種類の総数
};

/// @brief 強化するステータスの情報
struct UpgradeData
{
	std::string name;	// ステータス名
	int cost;			// 強化するのに必要なコスト
	float value;		// 上昇値
	int currentLevel;	// 現在のレベル
	int maxLevel;		// 最大レベル
};

class Shop
{
private: 
	/// @brief 強化項目のインデックス
	int m_UpgradeIndex = 0;
	/// @brief 遷移先メニューのインデックス
	int m_MenuIndex = 0;
	/// @brief 選択している項目
	SHOP_FOCUS m_Focus = SHOP_FOCUS::FOCUS_UPGRADE;
	/// @brief 強化するステータスの配列
	std::vector<UpgradeData> m_Data;
public:
	/// @brief コンストラクタ
	Shop() = default;
	/// @brief デストラクタ
	~Shop() = default;

	/// @brief 初期化関数
	void Init();

	// 操作用関数
	void Input_Up();
	void Input_Down();
	void Input_Left();
	void Input_Right();

	/// @brief 選択項目を強化する関数
	/// @return 強化成功の成否
	bool BuyUpgradeData();

	/// @brief 強化項目のインデックスを取得する関数
	/// @return 強化項目のインデックス
	int GetUpgradeIndex() const { return m_UpgradeIndex; }
	/// @brief 遷移先メニューのインデックスを取得する関数
	/// @return 遷移先メニューのインデックス
	int GetMenuIndex() const { return m_MenuIndex; }
	/// @brief 選択している項目を取得する関数
	/// @return 選択している項目
	SHOP_FOCUS GetFocus() const { return m_Focus; }
	/// @brief 全てのステータスを取得する関数
	/// @return 全てのステータス
	const std::vector<UpgradeData>& GetAllData() const { return m_Data; }
};