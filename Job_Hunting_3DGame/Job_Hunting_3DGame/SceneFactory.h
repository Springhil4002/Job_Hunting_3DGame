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
			// �^�C�g���V�[��
		case SCENE_ID::SCENE_ID_TITLE:
			return new TitleScene(_camera);
			// �Q�[���V�[��
		case SCENE_ID::SCENE_ID_GAME:
			return new GameScene(_camera);
			// ���U���g�V�[��
		case SCENE_ID::SCENE_ID_RESULT:
			return new ResultScene(_camera);
			// �Y�����Ȃ�
		default:
			return nullptr;
		}
	}
};