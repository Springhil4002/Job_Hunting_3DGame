#pragma once
#include "System/Input.h"
#include "SceneFactory.h"
#include "SceneResource.h"
#include "SoundManager.h"
#include "GameStatus.h"

class SceneManager
{
private:
	/// @brief 現在のシーン
	static std::unique_ptr<BaseScene> currentScene;
	/// @brief シーン生成ファクトリー
	static SceneFactory sceneFactory;
	// サウンドマネージャーのインスタンス
	static SoundManager soundManager;
	// ゲームステータスのインスタンス
	static GameStatus gameStatus;
	// 入力インスタンス
	static Input m_Input;
	// ImGuiの表示切り替えフラグ
	static bool m_ShowImGui;
public:
	SceneManager() = default;
	/// @brief コンストラクタ
	SceneManager(Camera* _camera, Camera2D* _uiCamera, HWND _hwnd);
	/// @brief デストラクタ
	~SceneManager() = default;

	/// @brief シーン遷移関数
	/// @param _scene_ID 遷移したいシーンのID
	static void ChangeScene(
		SCENE_ID _scene_ID, Camera* _camera, Camera2D* _uiCamera, HWND _hwnd);
	/// @brief コンソール画面のログをクリア
	static void ClearConsole();

	/// @brief サウンドマネージャーを取得する関数
	/// @return サウンドマネージャー
	static SoundManager& GetSound() { return soundManager; }
	/// @brief ゲームステータスを取得する関数
	/// @return ゲームステータス
	static GameStatus& GetGameStatus() { return gameStatus; }
	/// @brief ImGui表示切り替えフラグの変更関数
	static void ChangeImGui() { m_ShowImGui = !m_ShowImGui; }
	/// @brief ImGui表示切り替えフラグを取得する関数
	/// @return ImGui表示切り替えフラグ
	static bool GetShowImGui() { return m_ShowImGui; }

	/// @brief 初期化処理
	void Init();
	/// @brief 更新処理
	void Update(float _deltaTime);
	/// @brief 描画処理
	void Draw();
	/// @brief 終了処理
	void Uninit();
	/// @brief ImGuiの描画処理
	void Draw_ImGui();
};