#pragma once
#include "BaseScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

/// @brief 列挙型:シーンID
enum SCENE_ID
{
	SCENE_ID_TITLE = 0,	// 0:タイトルシーン
	SCENE_ID_GAME,		// 1:ゲームシーン
	SCENE_ID_RESULT,	// 2:リザルトシーン

	SCENE_ID_NUM		// シーン総数
};;

class SceneFactory
{
public:
	BaseScene* CreateScene(const SCENE_ID& _sceneName, Camera* _camera, HWND _hwnd);
};