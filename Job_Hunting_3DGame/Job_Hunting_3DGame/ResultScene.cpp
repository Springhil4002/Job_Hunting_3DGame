#include "ResultScene.h"
#include "SceneManager.h"

Object* ResultScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void ResultScene::Init()
{
	printf("シーン名：ResultScene\n");
}

void ResultScene::Update()
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
	{
		SceneManager::ChangeScene(SCENE_ID_TITLE);
	}
}

void ResultScene::Draw()
{
	for (auto& obj : objectInstance)
	{
		obj->Draw();
	}
}

void ResultScene::Uninit()
{
	for (auto& obj : objectInstance)
	{
		obj->Uninit();
	}
}