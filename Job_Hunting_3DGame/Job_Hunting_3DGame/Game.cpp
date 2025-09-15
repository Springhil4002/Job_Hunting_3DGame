#include "Game.h"
#include "Debug_New.h"

using namespace DirectX;

bool Game::Init(Player* _player, std::vector<Goal*> _goals)
{
	goalFlag = false;
	clearCount = 5;
	player = _player;
	goals = _goals;
	if (!player || goals.empty()) return false;
	
	insideFlags.resize(goals.size(), false);
	countdownStart = std::chrono::steady_clock::now();
	
	printf("Game:初期化処理に成功\n");
	return true;
}

void Game::Update(float _deltaTime)
{
	auto now = std::chrono::steady_clock::now();

	switch (state)
	{
	case RACE_STATE::RACE_STATE_COUNTDOWN:
	{
		// カウントダウン開始
		auto countDownElapsed = std::chrono::duration_cast<std::chrono::seconds>(
			now - countdownStart).count();
		int remaingTime = countDownTime - static_cast<int>(countDownElapsed);

		if (remaingTime <= 0)
		{
			state = RACE_STATE::RACE_STATE_RUNNING;
			raceStartTime = std::chrono::steady_clock::now();
		}
		break;
	}
	case RACE_STATE::RACE_STATE_RUNNING:
	{
		// タイマー計測
		elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - raceStartTime);
		
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

void Game::Uninit()
{
	player = nullptr;
	goals.clear();
	goalFlag = false;
	state = RACE_STATE::RACE_STATE_COUNTDOWN;
}

void Game::GoalCheck()
{
	for (size_t i = 0; i < goals.size(); i++)
	{
		auto* goal = goals[i];
		if (!goal) continue;

		// プレイヤーとゴールの距離
		XMVECTOR pos = goal->GetSphere()->GetPos();
		XMFLOAT3 SpherePos;
		XMStoreFloat3(&SpherePos, pos);

		XMVECTOR playerPosVec = player->GetPos();
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
			insideFlags[i] = true; // ゴール内にいる
		}
		else
		{
			if (insideFlags[i])
			{
				goalCount++;
				insideFlags[i] = false;

				if (goalCount >= clearCount)
				{
					goalFlag = true;
					state = RACE_STATE::RACE_STATE_GOAL;
				}
			}
		}
	}
}