#include "TitleScene.h"
#include "SceneManager.h"

Object* TitleScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void TitleScene::Init()
{
	printf("ƒV[ƒ“–¼FTitleScene\n");
}

void TitleScene::Update()
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
	{
		SceneManager::ChangeScene(SCENE_ID_GAME);
	}
}

void TitleScene::Draw()
{
	for (auto& obj : objectInstance)
	{
		obj->Draw();
	}
}

void TitleScene::Uninit()
{
	for (auto& obj : objectInstance)
	{
		obj->Uninit();
	}
}