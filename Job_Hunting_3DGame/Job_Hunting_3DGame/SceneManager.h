#pragma once
#include "SceneFactory.h"

class SceneManager
{
private:
	/// @brief 現在のシーン
	static BaseScene* currentScene;
	/// @brief シーン生成ファクトリー
	static SceneFactory sceneFactory;
	/// @brief シーン切り替えフラグ
	static bool change;
	/// @brief 生成したオブジェクトを格納する配列
	static std::set<Object*> createObjects;
	/// @brief 生成したオブジェクトを格納する配列
	static std::set<Object*> deleteObjects;
public:
	SceneManager() = default;
	/// @brief コンストラクタ
	SceneManager(Camera* _camera)
	{
		currentScene = sceneFactory.CreateScene(SCENE_ID_TITLE,_camera);
	}
	/// @brief デストラクタ
	~SceneManager() = default;

	/// @brief 初期化処理
	void Init();
	/// @brief 更新処理
	void Update();
	/// @brief 描画処理
	void Draw();
	/// @brief 終了処理
	void Uninit();

	/// @brief シーン遷移関数
	/// @param _scene_ID 遷移したいシーンのID
	static void ChangeScene(SCENE_ID _scene_ID,Camera* _camera);

	/// @brief 追加されたオブジェクトの生成
	void Create();
	/// @brief 追加されたオブジェクトの削除
	void Delete();
};
