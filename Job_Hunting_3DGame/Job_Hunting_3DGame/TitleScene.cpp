#include "TitleScene.h"
#include "SceneManager.h"

Object* TitleScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void TitleScene::Init()
{
	printf("�V�[�����FTitleScene\n");
}

void TitleScene::Update()
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
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