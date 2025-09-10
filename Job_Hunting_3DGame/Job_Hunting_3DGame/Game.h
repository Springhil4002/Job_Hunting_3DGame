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
	bool goalFlag = false;
	int countDownTime = 3;
	Player* player = nullptr;
	Goal* goal = nullptr;
	RACE_STATE state = RACE_STATE::RACE_STATE_COUNTDOWN;
	std::chrono::steady_clock::time_point countdownStart;
	std::chrono::steady_clock::time_point raceStartTime;
	std::chrono::milliseconds elapsedTime = std::chrono::milliseconds(0);
public:
	/// @brief 初期化処理
	/// @param _player　プレイヤーのポインタ 
	/// @param _goal ゴールのポインタ
	/// @return 初期化成功の成否
	bool Init(Player* _player, Goal* _goal);
	/// @brief 更新処理
	/// @param _deltaTime 経過時間
	void Update(float _deltaTime);
	/// @brief 終了処理
	void Uninit();
	/// @brief ゴール判定する関数
	void GoalCheck();
	/// @brief ゴールフラグの取得する関数
	/// @return ゴールフラグ
	bool GetGoalFlag() const { return goalFlag; }
	/// @brief Playerのポインタを取得する関数
	/// @return Playerのポインタ
	Player* GetPlayer() const { return player; }
	/// @brief Goalのポインタを取得する関数
	/// @return Goalのポインタ
	Goal* GetGoal() const { return goal; }
	/// @brief ゴールフラグを設定する関数
	/// @param _flag true Or false
	void SetGoalFlag(bool _flag) { goalFlag = _flag; }
	/// @brief プレイヤーオブジェクトを設定します。
	/// @param _player Player型のポインタ。
	void SetPlayer(Player* _player) { player = _player; }
	/// @brief ゴールオブジェクトを設定します。
	/// @param _goal Goal型のポインタ。
	void SetGoal(Goal* _goal) { goal = _goal; }
	/// @brief 現在のレース状態を取得する関数
	/// @return レースの現在の状態を返します
	RACE_STATE GetState() const { return state; }
	/// @brief レースが進行中か取得する関数
	/// @return レースが進行中かの成否を返します
	bool IsRunning() const { return state == RACE_STATE::RACE_STATE_RUNNING; }
	/// @brief レースをゴールしてるか取得する関数
	/// @return レースをゴールしてるかの成否を返します
	bool IsGoal() const { return state == RACE_STATE::RACE_STATE_GOAL; }
	/// @brief レース走行中の経過時間を取得する関数
	/// @return 走行中の現在の経過時間を返します
	std::chrono::milliseconds GetElapsedTime() const { return elapsedTime; }
	/// @brief カウントダウンが残り何秒かを取得する関数
	/// @return カウントダウンが残っている秒数
	int GetCountDownRemaining() const
	{
		if (GetState() != RACE_STATE::RACE_STATE_COUNTDOWN) return 0;
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - countdownStart).count();
		return std::max<int>(countDownTime - static_cast<int>(elapsed), 0);
	}
	/// @brief タイマーをリセットする関数
	void ResetTimer()
	{
		elapsedTime = std::chrono::milliseconds(0);
		countdownStart = std::chrono::steady_clock::now();
		state = RACE_STATE::RACE_STATE_COUNTDOWN;
	}
};