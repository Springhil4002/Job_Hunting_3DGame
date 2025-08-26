#include "Game.h"

using namespace DirectX;

bool Game::Init(Player* _player, Goal* _goal)
{
	player = _player;
	if (!player) return false;
	goal = _goal;
	if (!goal) return false;
	goalFlag = false;
	
	printf("Game:èâä˙âªê¨å˜\n");
	return true;
}

void Game::Update(float _deltaTime)
{
	XMVECTOR pos = player->GetPos();
	XMFLOAT3 playerPos;
	XMStoreFloat3(&playerPos, pos);
	
	pos = goal->GetPos();
	XMFLOAT3 goalPos;
	XMStoreFloat3(&goalPos, pos);
	
	// ÉSÅ[ÉãîªíË
	if (goalPos.z <= playerPos.z)
	{
		goalFlag = true;
	}
}