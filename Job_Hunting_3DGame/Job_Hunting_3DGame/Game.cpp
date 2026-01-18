#include "Game.h"
#include "Debug_New.h"

using namespace DirectX;

Game::Game(int _createGoalNum)
	: m_CreateGoalCount(_createGoalNum)
{
}

bool Game::Init(Player* _player, std::vector<Goal*> _goals)
{
	m_CountDownTime = 3;
	m_ClearCount = 5;
	m_LimitTime = std::chrono::seconds(60);
	m_Player = _player;
	m_Goals = _goals;
	if (!m_Player || m_Goals.empty()) return false;
	
	m_InsideFlags.assign(m_Goals.size(), false);
	m_RemainingTime = m_LimitTime;
	m_CountdownStart = std::chrono::steady_clock::now();
	m_State = RACE_STATE::RACE_STATE_COUNTDOWN;

	printf("Game:初期化処理に成功\n");
	return true;
}

void Game::Update(float _deltaTime)
{
	auto now = std::chrono::steady_clock::now();

	switch (m_State)
	{
	case RACE_STATE::RACE_STATE_COUNTDOWN:
	{
		// カウントダウン開始
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
			now - m_CountdownStart).count();
		if (static_cast<int>(elapsed) >= m_CountDownTime) 
		{
			m_State = RACE_STATE::RACE_STATE_RUNNING;
			m_RaceStartTime = std::chrono::steady_clock::now();
		}
		break;
	}
	case RACE_STATE::RACE_STATE_RUNNING:
	{
		// 経過時間を計算
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_RaceStartTime);
		
		// 残り時間を計算
		m_RemainingTime = m_LimitTime - diff;
		if (m_RemainingTime.count() <= 0)
		{
			m_RemainingTime = std::chrono::milliseconds(0);
			m_TimeUp = true;
			m_State = RACE_STATE::RACE_STATE_GOAL;
		}

		// ゴール判定
		GoalCheck();
		
		break;
	}
	case RACE_STATE::RACE_STATE_GOAL:
		break;
	default:
		break;
	}
}

void Game::UnInit()
{
	m_Player = nullptr;
	m_Goals.clear();
	m_InsideFlags.clear();

	m_GoalCount = 0;
	m_GoalFlag = false;
	m_TimeUp = false;
	m_RemainingTime = std::chrono::milliseconds(0);
	m_State = RACE_STATE::RACE_STATE_COUNTDOWN;
}

void Game::GoalCheck()
{
	for (size_t i = 0; i < m_Goals.size(); i++)
	{
		auto* goal = m_Goals[i];
		if (!goal) continue;

		// プレイヤーとゴールの距離
		XMVECTOR pos = goal->GetSphere()->GetPos();
		XMFLOAT3 SpherePos;
		XMStoreFloat3(&SpherePos, pos);

		XMVECTOR playerPosVec = m_Player->GetPos();
		XMFLOAT3 playerPos;
		XMStoreFloat3(&playerPos, playerPosVec);

		float dx = playerPos.x - SpherePos.x;
		float dy = playerPos.y - SpherePos.y;
		float dz = playerPos.z - SpherePos.z;
		float distance = dx * dx + dy * dy + dz * dz;
		float radius = goal->GetSphere()->GetRadius();

		// ゴール内部チェック
		if (distance <= radius * radius)
		{
			m_InsideFlags[i] = true; // ゴール内にいる
		}
		else
		{
			if (m_InsideFlags[i])
			{
				m_GoalCount++;
				m_InsideFlags[i] = false;

				if (m_GoalCount >= m_ClearCount)
				{
					m_GoalFlag = true;
					m_State = RACE_STATE::RACE_STATE_GOAL;
				}
			}
		}
	}
}