#pragma once
#include "SCENE_ID.h"
#include "BaseScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

class SceneFactory
{
public:
	BaseScene* CreateScene(const SCENE_ID& _sceneName, Camera* _camera, HWND _hwnd);
};