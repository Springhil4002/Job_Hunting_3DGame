#include "ResultScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"

Object* ResultScene::CreateObj(const std::string& _objectID)
{
	auto obj = prototypeManager->Create(_objectID);
	if (obj)
	{
		// 所有権をシーン内管理vector配列に映す
		objectInstance.push_back(std::move(obj));
		return objectInstance.back().get();
	}
	return nullptr;
}

void ResultScene::Init(Camera* _camera, Camera2D* _uiCamera, HWND _hwnd)
{
	camera = _camera;
	uiCamera = _uiCamera;
	hwnd = _hwnd;
	printf("シーン名：ResultScene\n");
}

void ResultScene::Update(float _deltaTime)
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
	{
		SceneManager::ChangeScene(SCENE_ID_TITLE, camera, uiCamera, hwnd);
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