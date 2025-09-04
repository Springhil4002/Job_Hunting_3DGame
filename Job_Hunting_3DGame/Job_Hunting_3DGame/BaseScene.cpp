#include "BaseScene.h"
#include "Object.h"
#include <map>
#include "Debug_New.h"

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

std::vector<std::unique_ptr<Object>>& BaseScene::GetAllObjects() 
{
	return objectInstance; 
}