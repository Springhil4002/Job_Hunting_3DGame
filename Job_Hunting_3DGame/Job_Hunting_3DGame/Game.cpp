#include "Game.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

using namespace DirectX;

bool Game::Init(Player* _player, std::vector<Goal*> _goals)
{
	m_Player = _player;
	m_Goals = _goals;
	if (!m_Player || m_Goals.empty()) return false;

	m_InsideFlags.assign(m_Goals.size(), false);			// 各鳥居の内部判定フラグ
	m_TimeUp = false;										// 時間切れのフラグ
	m_RemainingTime = m_LimitTime;							// 残り時間
	m_CountdownStart = std::chrono::steady_clock::now();	// カウントダウンの開始
	m_CountDownTime = 3;									// カウントダウンの秒数
	m_MaxComboTime = 3.0f;									// コンボ維持時間
	m_State = RACE_STATE::RACE_STATE_COUNTDOWN;				// ゲームの状態管理

	DEBUG_LOG("Game:初期化処理に成功");
	return true;
}

void Game::Update(float _deltaTime)
{
	auto now = std::chrono::steady_clock::now();

	switch (m_State)
	{
	case RACE_STATE::RACE_STATE_COUNTDOWN:
	{
		// カウントダウン開始
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
			now - m_CountdownStart).count();
		if (static_cast<int>(elapsed) >= m_CountDownTime) 
		{
			m_State = RACE_STATE::RACE_STATE_RUNNING;
			m_RaceStartTime = std::chrono::steady_clock::now();
		}
		break;
	}
	case RACE_STATE::RACE_STATE_RUNNING:
	{
		// 経過時間を計算
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_RaceStartTime);
		
		// 残り時間を計算
		m_RemainingTime = m_LimitTime - diff;
		if (m_RemainingTime.count() <= 0)
		{
			m_RemainingTime = std::chrono::milliseconds(0);
			m_TimeUp = true;
		}

		// コンボシステムの更新
		if (m_ComboTimer > 0.0f)
		{

			m_ComboTimer -= _deltaTime;
			if (m_ComboTimer <= 0.0f)
			{
				// 時間切れでコンボリセット
				m_ComboCount = 0;
				m_ComboTimer = 0.0f;
			}
		}

		// ゴール判定
		GoalCheck();
		
		break;
	}
	default:
		break;
	}
}

void Game::Draw_ImGui()
{
	ImGui::Begin("Game");
	if (ImGui::CollapsingHeader("Game Status"))
	{
		auto& m_Status = SceneManager::GetGameStatus();
		ImGui::Text("Score: %d", m_Status.score);
		ImGui::Text("CreateGoalCount: %d", m_Status.createGoalCount);
		ImGui::Text("LimitTime: %d", m_Status.limitTime);
		ImGui::Text("MaxSpeed: %f", m_Status.maxSpeed);
		ImGui::Text("AccelSpeed: %f", m_Status.accelSpeed);
	}
	ImGui::End();
}

void Game::UnInit()
{
	m_Player = nullptr;
	m_Goals.clear();
	m_InsideFlags.clear();

	m_TimeUp = false;
	m_RemainingTime = std::chrono::milliseconds(0);
	m_State = RACE_STATE::RACE_STATE_COUNTDOWN;
}

void Game::GoalCheck()
{
	for (size_t i = 0; i < m_Goals.size(); i++)
	{
		auto* goal = m_Goals[i];
		if (!goal) continue;

		// プレイヤーとゴールの距離
		XMVECTOR pos = goal->GetSphere()->GetPos();
		XMFLOAT3 SpherePos;
		XMStoreFloat3(&SpherePos, pos);

		XMVECTOR playerPosVec = m_Player->GetPos();
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
			m_InsideFlags[i] = true; // ゴール内にいる
		}
		else
		{
			if (m_InsideFlags[i])
			{
				auto& status = SceneManager::GetGameStatus();

				// コンボ加算と受付時間のリセット
				m_ComboCount++;
				m_ComboTimer = m_MaxComboTime;

				// コンボ倍率の計算
				float combo = 1.0f + (std::max(0, m_ComboCount - 1) * 0.1f);
				float baseScore = 100.0f * status.scoreUp;
				int finalScore = static_cast<int>(baseScore * combo);
				// スコア加算
				status.score += finalScore;
				// フラグを戻す
				m_InsideFlags[i] = false;
			}
		}
	}
}