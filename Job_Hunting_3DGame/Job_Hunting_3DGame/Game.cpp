#include "Game.h"

using namespace DirectX;

bool Game::Init(Player* _player, Goal* _goal)
{
	player = _player;
	if (!player) return false;
	goal = _goal;
	if (!goal) return false;
	goalFlag = false;
	
	printf("Game:初期化成功\n");
	return true;
}

void Game::Update(float _deltaTime)
{
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
	}
}