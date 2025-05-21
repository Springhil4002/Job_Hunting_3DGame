#include "ResultScene.h"
#include "SceneManager.h"

Object* ResultScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void ResultScene::Init()
{
	printf("ƒV[ƒ“–¼FResultScene\n");
}

void ResultScene::Update()
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
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