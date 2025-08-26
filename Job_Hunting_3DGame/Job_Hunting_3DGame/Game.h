#pragma once
#include "Player.h"
#include "Goal.h"

class Game
{
private:
	Player* player;
	Goal* goal;
	bool goalFlag = false;
public:
	/// @brief 初期化処理
	/// @param _player　プレイヤーのポインタ 
	/// @param _goal ゴールのポインタ
	/// @return 初期化成功の成否
	bool Init(Player* _player, Goal* _goal);
	/// @brief 更新処理
	/// @param _deltaTime 経過時間
	void Update(float _deltaTime);
	/// @brief ゴールフラグの取得する関数
	/// @return ゴールフラグ
	bool GetGoalFlag() const { return goalFlag; }
};