#pragma once
#include <vector>
#include <string>
#include "GameStatus.h"

/// @brief 強化するステータスの種類
enum class UPGRADE_TYPE
{
	TYPE_SPEED,
	TYPE_ACCEL,
	TYPE_TIME,
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
	/// @brief 選択項目のインデックス
	int m_SelectIndex = 0;
	/// @brief 強化するステータスの配列
	std::vector<UpgradeData> m_Data;
public:
	/// @brief コンストラクタ
	Shop() = default;
	/// @brief デストラクタ
	~Shop() = default;

	/// @brief 初期化関数
	void Init();
	/// @brief 次の選択項目へ移動する関数
	void NextData();
	/// @brief 前の選択項目へ移動する関数
	void PrevData();
	/// @brief 選択項目を強化する関数
	/// @return 強化成功の成否
	bool BuyUpgradeData();

	/// @brief 選択項目のインデックスを取得する関数
	/// @return 選択項目のインデックス
	int GetSelectIndex() const { return m_SelectIndex; }
	/// @brief 選択項目のステータスを取得する関数
	/// @param _index 選択項目のインデックス
	/// @return 選択項目のステータス
	const UpgradeData& GetSelectData(int _index) const { return m_Data[_index]; }
	/// @brief 全てのステータスを取得する関数
	/// @return 全てのステータス
	const std::vector<UpgradeData>& GetAllData() const { return m_Data; }
};