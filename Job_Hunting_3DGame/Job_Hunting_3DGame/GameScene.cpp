#include "GameScene.h"
#include "SceneManager.h"

Object* GameScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void GameScene::Init()
{
	printf("ƒV[ƒ“–¼FGameScene\n");
}

void GameScene::Update()
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
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