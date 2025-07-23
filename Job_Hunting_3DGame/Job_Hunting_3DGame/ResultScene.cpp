#include "ResultScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"

Object* ResultScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void ResultScene::Init(Camera* _camera,HWND _hwnd)
{
	camera = _camera;
	hwnd = _hwnd;
	printf("ƒV[ƒ“–¼FResultScene\n");
}

void ResultScene::Update(float _deltaTime)
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
	{
		SceneManager::ChangeScene(SCENE_ID_TITLE, camera, hwnd);
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

void ResultScene::Draw_ImGui()
{
	ImGui::Begin("SceneName:ResultScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::Text("Press Enter to Title!");
	ImGui::End();
}