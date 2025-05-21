#include "BaseScene.h"
#include "Object.h"
#include <map>

BaseScene* BaseScene::sceneInstance;
Input BaseScene::input;

void BaseScene::Input()
{
	input.Update();
}

BaseScene* BaseScene::GetInstance()
{
	return sceneInstance;
}

std::set<Object*>* BaseScene::GetObjects()
{
	return &objectInstance;
}