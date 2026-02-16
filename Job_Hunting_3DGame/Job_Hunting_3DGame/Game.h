#pragma once
#include <algorithm>
#include <chrono>
#include "Player.h"
#include "Goal.h"
#include "GameStatus.h"

namespace
{
	using namespace DirectX;

	/// @brief 2つのOBBの特定の軸上での重なり判定
	/// @param _axis OBBの特定の軸
	/// @param _a 1つめのOBB
	/// @param _b 2つめのOBB
	/// @param _centerDiff 中心からの距離 
	/// @return 重なりの成否
	bool CollisionAxis(const XMVECTOR& _axis, const CollisionOBB& _a, 
		const CollisionOBB& _b, const XMVECTOR& _centerDiff)
	{
		// 軸がゼロベクトルの場合はスキップ
		if (XMVector3Less(XMVector3LengthSq(_axis), XMVectorSet(0.0001f, 0.0001f, 0.0001f, 0.0001f))) return true;
		// 各軸を正規化
		XMVECTOR normalizedAxis = XMVector3Normalize(_axis);
		// 中心間の距離の投影
		float dist = fabsf(XMVectorGetX(XMVector3Dot(_centerDiff, normalizedAxis)));

		// 各OBBの半径を計算
		float radiusA =
			fabsf(XMVectorGetX(XMVector3Dot(_a.axis[0] * XMVectorGetX(_a.radius), normalizedAxis))) +
			fabsf(XMVectorGetX(XMVector3Dot(_a.axis[1] * XMVectorGetY(_a.radius), normalizedAxis))) +
			fabsf(XMVectorGetX(XMVector3Dot(_a.axis[2] * XMVectorGetZ(_a.radius), normalizedAxis)));
		
		float radiusB =
			fabsf(XMVectorGetX(XMVector3Dot(_b.axis[0] * XMVectorGetX(_b.radius), normalizedAxis))) +
			fabsf(XMVectorGetX(XMVector3Dot(_b.axis[1] * XMVectorGetY(_b.radius), normalizedAxis))) +
			fabsf(XMVectorGetX(XMVector3Dot(_b.axis[2] * XMVectorGetZ(_b.radius), normalizedAxis)));

		return dist <= (radiusA + radiusB);
	}

	bool IntersectOBB(const CollisionOBB _a, const CollisionOBB _b)
	{
		XMVECTOR centerDiff = _b.center - _a.center;

		// 分離軸のチェック
		// a:1つめのOBBの各軸
		if (!CollisionAxis(_a.axis[0], _a, _b, centerDiff)) return false;
		if (!CollisionAxis(_a.axis[1], _a, _b, centerDiff)) return false;
		if (!CollisionAxis(_a.axis[2], _a, _b, centerDiff)) return false;

		// b:2つめのOBBの各軸
		if (!CollisionAxis(_b.axis[0], _a, _b, centerDiff)) return false;
		if (!CollisionAxis(_b.axis[1], _a, _b, centerDiff)) return false;
		if (!CollisionAxis(_b.axis[2], _a, _b, centerDiff)) return false;
	
		// 2つの各軸の外積
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (!CollisionAxis(XMVector3Cross(_a.axis[i], _b.axis[j]),
					_a, _b, centerDiff)) return false;
			}
		}
		return true;
	}
}

enum class RACE_STATE
{
	RACE_STATE_COUNTDOWN,	// 待機状態・カウントダウン
	RACE_STATE_RUNNING,		// 走行状態
};

class Game
{
private:
	Player* m_Player = nullptr;			// 参照用プレイヤーポインタ
	std::vector<Goal*> m_Goals;			// 参照用ゴールポインタ配列
	std::vector<bool> m_InsideFlags;	// 各ゴールのフラグ配列
	
	bool m_TimeUp = false;		// タイムアップフラグ 
	std::chrono::steady_clock::time_point m_CountdownStart;	// ゲーム開始前のカウントダウン
	std::chrono::steady_clock::time_point m_RaceStartTime;	// ゲーム開始時間
	std::chrono::milliseconds m_RemainingTime = std::chrono::milliseconds(0);	// 残り時間
	std::chrono::milliseconds m_LimitTime = std::chrono::seconds(0);			// 制限時間
	
	int m_CountDownTime = 0;		// ゲーム開始前のカウントダウン
	int m_CreateGoalCount = 0;		// 生成するゴールオブジェクト数
	int m_ComboCount = 0;			// コンボ数
	float m_ComboTimer = 0.0f;		// コンボ受付の残り時間
	float m_MaxComboTime = 0.0f;	// コンボ維持時間
	bool m_IsHitObstacle = false;	// 衝突判定フラグ
	DirectX::XMVECTOR m_LastHitObstaclePos = DirectX::XMVectorZero();	// 最後に衝突した位置
	RACE_STATE m_State = RACE_STATE::RACE_STATE_COUNTDOWN;	// 現在のレース状態
public:
	/// @brief デフォルトコンストラクタ
	Game() = default;

	/// @brief 初期化処理
	/// @param _player　プレイヤーのポインタ 
	/// @param _goal ゴールのポインタ
	/// @return 初期化成功の成否
	bool Init(Player* _player, std::vector<Goal*> _goals);
	/// @brief 更新処理
	/// @param _deltaTime 経過時間
	void Update(float _deltaTime);
	/// @brief ImGui描画関数
	void Draw_ImGui();
	/// @brief 終了処理
	void UnInit();

	/// @brief ゴール判定する関数
	void GoalCheck();
	
	/// @brief 生成するGoalオブジェクトを取得する関数
	/// @return Goalオブジェクトの生成数
	int GetCreateGoalCount() const { return m_CreateGoalCount; }
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
	/// @brief 衝突したかを確認する関数
	/// @return 衝突の成否
	bool CheckAndResetHitFlag()
	{
		bool hit = m_IsHitObstacle;
		m_IsHitObstacle = false;
		return hit;
	}
	/// @brief 衝突した場所を取得する関数
	/// @return 衝突した場所
	DirectX::XMVECTOR GetLastHitPos() const { return m_LastHitObstaclePos; }

	/// @brief 制限時間を設定する関数
	/// @param _time 制限時間
	void SetLimitTime(std::chrono::milliseconds _time) { m_LimitTime = _time; }
	/// @brief 生成するGoalオブジェクト数を設定する関数
	/// @param _count Goalオブジェクトの生成数
	void SetCreateGoalCount(int _count) { m_CreateGoalCount = _count; }

	void SetGameStatus(const GameStatus& _status)
	{
		m_CreateGoalCount = _status.createGoalCount;			// 鳥居オブジェクトの生成数
		m_LimitTime = std::chrono::seconds(_status.limitTime);	// 制限時間
	}
};