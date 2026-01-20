#pragma once

struct GameStatus
{
	int score;				// 現在のスコア
	int createGoalCount;	// 生成するゴール数
	int limitTime;			// 制限時間(秒数)
	float maxSpeed;			// 上限速度
	float accelSpeed;		// 加速度

	/// @brief コンストラクタ
	GameStatus();
};