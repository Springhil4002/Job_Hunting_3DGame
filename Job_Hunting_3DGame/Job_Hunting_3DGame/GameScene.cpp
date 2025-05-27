#include "GameScene.h"
#include "SceneManager.h"

Object* GameScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void GameScene::Init(Camera* _camera)
{
	camera = _camera;
	printf("ƒV[ƒ“–¼FGameScene\n");
}

void GameScene::Update()
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
	{
		SceneManager::ChangeScene(SCENE_ID_RESULT, camera);
	}
}

void GameScene::Draw()
{
	for (auto& obj : objectInstance)
	{
		obj->Draw();
	}
}

void GameScene::Uninit()
{
	for (auto& obj : objectInstance)
	{
		obj->Uninit();
	}
}