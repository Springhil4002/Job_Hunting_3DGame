#pragma once

struct GameStatus
{
	int score;				// 現在のスコア
	int limitTime;			// 制限時間(秒数)
	float maxSpeed;			// 上限速度
	float accelSpeed;		// 加速度
	float scoreUp;			// スコア増加率
	int createGoalCount;	// 生成するゴール数

	int level_Speed;		// ステータスレベル(上限速度)
	int level_Accel;		// ステータスレベル(加速度)
	int level_ScoreUp;		// ステータスレベル(スコア増加)
	int level_Goal;			// ステータスレベル(ゴール生成数)

	/// @brief コンストラクタ
	GameStatus();
};