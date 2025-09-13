#include "ResultScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"

using namespace DirectX;

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
	printf("シーン名：ResultScene\n");

	camera = _camera;
	uiCamera = _uiCamera;
	hwnd = _hwnd;

	camera->SetPos(XMVectorSet(0.0f, 2.5f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	uiCamera->Init(screenWidth, screenHeight);

	// プロトタイプ登録
	prototypeManager->AddPrototype("UI", std::make_unique<UI>());
	prototypeManager->AddPrototype("UI_Fade", std::make_unique<UI_Fade>());

	UI* ui_test = dynamic_cast<UI*>(CreateObj("UI"));
	ui_test->Init(uiCamera, 480.0f, 270.0f, L"Assets/Texture/hogehoge.png");
	ui_test->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	ui_test->SetRota(XMVectorZero());
	ui_test->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_test->m_tags.AddTag("UI");

	UI_Fade* ui_fade = dynamic_cast<UI_Fade*>(CreateObj("UI_Fade"));
	ui_fade->Init(uiCamera, 1920.0f, 1080.0f);
	ui_fade->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	ui_fade->SetRota(XMVectorZero());
	ui_fade->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_fade->SetFadeOut();
	ui_fade->m_tags.AddTag("UI_Fade");
}

void ResultScene::Update(float _deltaTime)
{
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	auto ui_fade = FindByTag<UI_Fade>("UI_Fade");
	if (ui_fade && ui_fade->GetState() == FADE_STATE::FADE_STATE_NONE)
	{
		if (input.GetKeyTrigger(VK_RETURN))
		{
			ui_fade->SetFadeIn();
		}
	}

	if (ui_fade && ui_fade->IsFadeFinished() && ui_fade->GetAlpha() >= 1.0f)
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