#include "Game.h"
#include "Debug_New.h"

using namespace DirectX;

bool Game::Init(Player* _player, Goal* _goal)
{
	goalFlag = false;
	player = _player;
	goal = _goal;
	if (!player || !goal) return false;
	
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
	goal = nullptr;
	goalFlag = false;
	state = RACE_STATE::RACE_STATE_COUNTDOWN;
}

void Game::GoalCheck()
{
	if (!player || !goal) return;

	// プレイヤーの座標取得
	XMVECTOR pos = player->GetPos();
	XMFLOAT3 playerPos;
	XMStoreFloat3(&playerPos, pos);

	// ゴール内部の当たり判定用球体の座標取得
	pos = goal->GetSphere()->GetPos();
	XMFLOAT3 SpherePos;
	XMStoreFloat3(&SpherePos, pos);

	// 当たり判定用球体の半径取得
	float sphereRadius = goal->GetSphere()->GetRadius();

	float dx = playerPos.x - SpherePos.x;
	float dy = playerPos.y - SpherePos.y;
	float dz = playerPos.z - SpherePos.z;
	float distance = dx * dx + dy * dy + dz * dz;

	// ゴール判定
	if (distance <= sphereRadius * sphereRadius)
	{
		goalFlag = true;
		state = RACE_STATE::RACE_STATE_GOAL;
	}
}