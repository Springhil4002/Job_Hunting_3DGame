#pragma once
#include "Enum_Scene.h"
#include "BaseScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

class SceneFactory
{
public:
	BaseScene* CreateScene(const SCENE_ID& _sceneName)
	{
		switch (_sceneName)
		{
			// タイトルシーン
		case SCENE_ID::SCENE_ID_TITLE:
			return new TitleScene();
			// ゲームシーン
		case SCENE_ID::SCENE_ID_GAME:
			return new GameScene();
			// リザルトシーン
		case SCENE_ID::SCENE_ID_RESULT:
			return new ResultScene();
			// 該当しない
		default:
			return nullptr;
		}
	}
};