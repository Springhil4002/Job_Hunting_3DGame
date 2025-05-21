#include "BaseScene.h"
#include "Object.h"
#include <map>

BaseScene* BaseScene::sceneInstance;
Input BaseScene::input;

void BaseScene::Input()
{
	input.Update();
}

void BaseScene::Draw()
{
	for (auto& obj : objectInstance)
	{
		obj->Draw();
	}
}

void BaseScene::Uninit()
{
	for (auto& obj : objectInstance)
	{
		obj->Uninit();
	}
}

BaseScene* BaseScene::GetInstance()
{
	return sceneInstance;
}

std::set<Object*>* BaseScene::GetObjects()
{
	return &objectInstance;
}