#include "SceneFactory.h"
#include "Debug_New.h"

BaseScene* SceneFactory::CreateScene(const SCENE_ID& _sceneName, Camera* _camera, HWND _hwnd)
{
	switch (_sceneName)
	{
		// タイトルシーン
	case SCENE_ID::SCENE_ID_TITLE:
		return new TitleScene(_camera, _hwnd);
		// ゲームシーン
	case SCENE_ID::SCENE_ID_GAME:
		return new GameScene(_camera, _hwnd);
		// リザルトシーン
	case SCENE_ID::SCENE_ID_RESULT:
		return new ResultScene(_camera, _hwnd);
		// 該当しない
	default:
		return nullptr;
	}
}