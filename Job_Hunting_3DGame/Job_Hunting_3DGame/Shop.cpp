#include "Shop.h"
#include "SceneManager.h"
#include <algorithm>

void Shop::Init()
{
	auto& status = SceneManager::GetGameStatus();
	m_UpgradeIndex = 0;
	m_MenuIndex = 0;
	m_Focus = SHOP_FOCUS::FOCUS_UPGRADE;
	m_Data.clear();

	// 各ステータスの初期化(名前/消費コスト/上昇値/初期レベル/最大レベル)
	m_Data.push_back({ "Max Speed:",		50,  5.0f,  status.level_Speed,		10 });
	m_Data.push_back({ "Acceleration:",		50,  1.0f,  status.level_Accel,		10 });
	m_Data.push_back({ "Score Up:",			100, 0.1f,  status.level_ScoreUp,	10 });
	m_Data.push_back({ "Goal Create Count:",100, 5.0f,  status.level_Goal,		10 });

	// 現在のレベルに合わせてコストを再計算
	for (auto& data : m_Data) 
	{
		for (int i = 1; i < data.currentLevel; ++i) 
		{
			data.cost = static_cast<int>(data.cost * 2.0f);
		}
	}
}

void Shop::Input_Up()
{
	if (m_Focus == SHOP_FOCUS::FOCUS_UPGRADE) 
	{
		int num = static_cast<int>(UPGRADE_TYPE::TYPE_COUNT);
		m_UpgradeIndex = (m_UpgradeIndex - 1 + num) % num;
	}
	else 
	{
		m_Focus = SHOP_FOCUS::FOCUS_UPGRADE; // 下から上へ戻る
	}
}

void Shop::Input_Down()
{
	if (m_Focus == SHOP_FOCUS::FOCUS_UPGRADE) {
		if (m_UpgradeIndex == 3) m_Focus = SHOP_FOCUS::FOCUS_MENU; // 一番下ならメニューへ
		else m_UpgradeIndex++;
	}
}

void Shop::Input_Left()
{
	if (m_Focus == SHOP_FOCUS::FOCUS_MENU) m_MenuIndex = 0; // リプレイ
}

void Shop::Input_Right()
{
	if (m_Focus == SHOP_FOCUS::FOCUS_MENU) m_MenuIndex = 1; // タイトル
}

bool Shop::BuyUpgradeData()
{
	auto& status = SceneManager::GetGameStatus();
	UpgradeData& data = m_Data[m_UpgradeIndex];

	// 選択項目確認、所持スコア確認、レベル上限確認
	if (m_Focus != SHOP_FOCUS::FOCUS_UPGRADE ||
		status.score < data.cost || 
		data.currentLevel >= data.maxLevel) return false;	// 強化不可

	// スコア消費してステータスを強化
	status.score -= data.cost;
	data.currentLevel++;

	// ステータス反映
	switch (static_cast<UPGRADE_TYPE>(m_UpgradeIndex))
	{
	// ボートの最高速度
	case UPGRADE_TYPE::TYPE_SPEED:
	{
		status.maxSpeed += data.value;
		status.level_Speed = data.currentLevel;
		break;
	}
	// ボートの加速度
	case UPGRADE_TYPE::TYPE_ACCEL:
	{
		status.accelSpeed += data.value;
		status.level_Accel = data.currentLevel;
		break;
	}
	// 制限時間
	case UPGRADE_TYPE::TYPE_SCOREUP:
	{
		status.scoreUp += data.value;
		status.level_ScoreUp = data.currentLevel;
		break;
	}
	// ゴール生成数
	case UPGRADE_TYPE::TYPE_GOAL:
	{
		status.createGoalCount += static_cast<int>(data.value);
		status.level_Goal = data.currentLevel;
		break;
	}
	}

	// 次レベルのコスト上昇
	data.cost = static_cast<int>(data.cost * 2.0f);

	// 強化成功
	return true;
}