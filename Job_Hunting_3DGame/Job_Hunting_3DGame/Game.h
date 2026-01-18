#pragma once
#include <algorithm>
#include <chrono>
#include "Player.h"
#include "Goal.h"

enum class RACE_STATE
{
	RACE_STATE_COUNTDOWN,	// 待機状態・カウントダウン
	RACE_STATE_RUNNING,		// 走行状態
	RACE_STATE_GOAL			// ゴール状態
};

class Game
{
private:
	Player* m_Player = nullptr;			// 参照用プレイヤーポインタ
	std::vector<Goal*> m_Goals;			// 参照用ゴールポインタ配列
	std::vector<bool> m_InsideFlags;	// 各ゴールのフラグ配列

	std::chrono::steady_clock::time_point m_CountdownStart;	// 開始前のカウントダウン
	std::chrono::steady_clock::time_point m_RaceStartTime;	// ゲーム開始時間

	std::chrono::milliseconds m_LimitTime = std::chrono::seconds(0);			// 制限時間
	std::chrono::milliseconds m_RemainingTime = std::chrono::milliseconds(0);	// 残り時間
	bool m_TimeUp = false;			// タイムアップフラグ 

	bool m_GoalFlag = false;		// ゴール判定フラグ
	int m_CountDownTime = 0;		// ゲーム開始前のカウントダウン
	int m_CreateGoalCount = 0;		// 生成するゴールオブジェクト数
	int m_GoalCount = 0;			// 現在のゴール回数
	int m_ClearCount = 0;			// 通過クリア数
	RACE_STATE m_State = RACE_STATE::RACE_STATE_COUNTDOWN;	// 現在のレース状態
public:
	/// @brief デフォルトコンストラクタ
	Game() = default;
	/// @brief コンストラクタ
	/// @param _createGoalCount 生成するGoalオブジェクトの数 
	Game(int _createGoalCount);

	/// @brief 初期化処理
	/// @param _player　プレイヤーのポインタ 
	/// @param _goal ゴールのポインタ
	/// @return 初期化成功の成否
	bool Init(Player* _player, std::vector<Goal*> _goals);
	/// @brief 更新処理
	/// @param _deltaTime 経過時間
	void Update(float _deltaTime);
	/// @brief 終了処理
	void UnInit();

	/// @brief ゴール判定する関数
	void GoalCheck();
	
	/// @brief 生成するGoalオブジェクトを取得する関数
	/// @return Goalオブジェクトの生成数
	int GetCreateGoalCount() const { return m_CreateGoalCount; }
	/// @brief ゴールフラグを取得する関数
	/// @return ゴールフラグの成否
	bool GetGoalFlag() const { return m_GoalFlag; }
	/// @brief 現在のレース状態を取得する関数
	/// @return レースの現在の状態を返します
	RACE_STATE GetState() const { return m_State; }
	/// @brief 残り時間を取得する関数
	/// @return 残り時間
	std::chrono::milliseconds GetRemainingTime() const { return m_RemainingTime; }
	/// @brief 時間切れか確認する関数
	/// @return 時間切れかのフラグ
	bool GetTimeUpFlag() const { return m_TimeUp; }
	/// @brief カウントダウンが残り何秒かを取得する関数
	/// @return カウントダウンが残っている秒数
	int GetCountDownRemaining() const
	{
		if (GetState() != RACE_STATE::RACE_STATE_COUNTDOWN) return 0;
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_CountdownStart).count();
		return std::max<int>(m_CountDownTime - static_cast<int>(elapsed), 0);
	}

	/// @brief 制限時間を設定する関数
	/// @param _time 制限時間
	void SetLimitTime(std::chrono::milliseconds _time) { m_LimitTime = _time; }
	/// @brief 生成するGoalオブジェクト数を設定する関数
	/// @param _count Goalオブジェクトの生成数
	void SetCreateGoalCount(int _count) { m_CreateGoalCount = _count; }
};