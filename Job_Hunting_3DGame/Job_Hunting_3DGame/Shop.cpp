#include "Shop.h"
#include "SceneManager.h"
#include <algorithm>

void Shop::Init()
{
	auto& status = SceneManager::GetGameStatus();
	m_SelectIndex = 0;
	m_Data.clear();

	// 各ステータスの初期化(名前/消費コスト/上昇値/初期レベル/最大レベル)
	m_Data.push_back({ "Max Speed:",		100,  2.0f,  status.level_Speed, 10 });
	m_Data.push_back({ "Acceleration:",		100,  0.5f,  status.level_Accel, 10 });
	m_Data.push_back({ "Limit Time:",		1000, 10.0f, status.level_Time,  3 });
	m_Data.push_back({ "Goal Create Count:",100,  2.0f,  status.level_Goal,  10 });

	// 現在のレベルに合わせてコストを再計算
	for (auto& data : m_Data) 
	{
		for (int i = 0; i < data.currentLevel; ++i) 
		{
			data.cost = static_cast<int>(data.cost * 2.0f);
		}
	}
}

void Shop::NextData()
{
	m_SelectIndex = (m_SelectIndex + 1) % static_cast<int>(UPGRADE_TYPE::TYPE_COUNT);
}

void Shop::PrevData()
{
	int count = static_cast<int>(UPGRADE_TYPE::TYPE_COUNT);
	m_SelectIndex = (m_SelectIndex - 1 + count) % count;
}

bool Shop::BuyUpgradeData()
{
	auto& status = SceneManager::GetGameStatus();
	UpgradeData& data = m_Data[m_SelectIndex];

	// 所持スコア確認、レベル上限確認
	if (status.score < data.cost || data.currentLevel >= data.maxLevel)
	{
		return false;	// 強化不可
	}

	// スコア消費してステータスを強化
	status.score -= data.cost;
	data.currentLevel++;

	// ステータス反映
	switch (static_cast<UPGRADE_TYPE>(m_SelectIndex))
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
	case UPGRADE_TYPE::TYPE_TIME:
	{
		status.limitTime += static_cast<int>(data.value);
		status.level_Time = data.currentLevel;
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