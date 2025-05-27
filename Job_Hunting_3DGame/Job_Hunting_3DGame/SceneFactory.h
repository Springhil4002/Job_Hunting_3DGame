#pragma once
#include "Enum_Scene.h"
#include "BaseScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

class SceneFactory
{
public:
	BaseScene* CreateScene(const SCENE_ID& _sceneName,Camera* _camera)
	{
		switch (_sceneName)
		{
			// タイトルシーン
		case SCENE_ID::SCENE_ID_TITLE:
			return new TitleScene(_camera);
			// ゲームシーン
		case SCENE_ID::SCENE_ID_GAME:
			return new GameScene(_camera);
			// リザルトシーン
		case SCENE_ID::SCENE_ID_RESULT:
			return new ResultScene(_camera);
			// 該当しない
		default:
			return nullptr;
		}
	}
};