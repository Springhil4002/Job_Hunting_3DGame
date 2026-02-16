#include "ResultScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

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
	DEBUG_LOG(L"シーン名:ResultScene");

	hwnd = _hwnd;
	camera = _camera;
	uiCamera = _uiCamera;

	camera->SetPos(XMVectorSet(0.0f, 2.5f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	camera->SetRoll(0.0f);
	uiCamera->Init(screenWidth, screenHeight);

	// プロトタイプ登録
	prototypeManager->AddPrototype("Sky", std::make_unique<SkyBox>());
	prototypeManager->AddPrototype("SeaMesh", std::make_unique<SeaMesh>());
	prototypeManager->AddPrototype("UI", std::make_unique<UI>());
	prototypeManager->AddPrototype("UI_Fade", std::make_unique<UI_Fade>());
	prototypeManager->AddPrototype("UI_Score", std::make_unique<UI_Score>());
	prototypeManager->AddPrototype("UI_Shop", std::make_unique<UI_Shop>());

	SkyBox* sky = dynamic_cast<SkyBox*>(CreateObj("Sky"));
	sky->Init(camera);
	sky->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	sky->SetRota(XMVectorZero());
	sky->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	sky->m_tags.AddTag("SkyDome");

	SeaMesh* seaMesh = dynamic_cast<SeaMesh*>(CreateObj("SeaMesh"));
	seaMesh->Init(camera);
	seaMesh->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	seaMesh->SetRota(XMVectorZero());
	seaMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	seaMesh->m_tags.AddTag("SeaMesh");

	// ショップシステム
	m_Shop = std::make_unique<Shop>();
	m_Shop->Init();

	//================================UI生成================================//

	UI* ui_con = dynamic_cast<UI*>(CreateObj("UI"));
	ui_con->Init(uiCamera, 700.0f, 100.0f, L"Assets/Texture/UI_Result_controller.png");
	ui_con->SetPos(XMVectorSet(-620.0f, -480.0f, 0.0f, 0.0f));
	ui_con->SetRota(XMVectorZero());
	ui_con->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_con->UpdateTransform();
	ui_con->UpdateCameraMatrix();
	ui_con->m_tags.AddTag("UI_con");

	UI* ui_con2 = dynamic_cast<UI*>(CreateObj("UI"));
	ui_con2->Init(uiCamera, 300.0f, 100.0f, L"Assets/Texture/UI_Result_controller2.png");
	ui_con2->SetPos(XMVectorSet(-100.0f, -480.0f, 0.0f, 0.0f));
	ui_con2->SetRota(XMVectorZero());
	ui_con2->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_con2->UpdateTransform();
	ui_con2->UpdateCameraMatrix();
	ui_con2->m_tags.AddTag("UI_con");

	// スコアUI
	UI_Score* score = dynamic_cast<UI_Score*>(CreateObj("UI_Score"));
	score->Init(uiCamera, 50.0f, 100.0f, L"Assets/Texture/UI_Number.png", L"Assets/Texture/UI_Score.png");
	score->m_tags.AddTag("UI_Score");
	
	UI_Shop* shop = dynamic_cast<UI_Shop*>(CreateObj("UI_Shop"));
	shop->Init(uiCamera, m_Shop.get());
	shop->m_tags.AddTag("UI_Shop");

	UI_Fade* ui_fade = dynamic_cast<UI_Fade*>(CreateObj("UI_Fade"));
	ui_fade->Init(uiCamera, 1920.0f, 1080.0f);
	ui_fade->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	ui_fade->SetRota(XMVectorZero());
	ui_fade->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_fade->SetFadeOut();
	ui_fade->UpdateTransform();
	ui_fade->UpdateCameraMatrix();
	ui_fade->m_tags.AddTag("UI_Fade");
}

void ResultScene::Update(float _deltaTime)
{
	//Update_Input();
	auto ui_fade = FindByTag<UI_Fade>("UI_Fade");
	bool isFading = (ui_fade && ui_fade->GetState() != FADE_STATE::FADE_STATE_NONE);
	if (!isFading)
	{
		if (input.GetKeyTrigger(VK_W))		m_Shop->Input_Up();
		if (input.GetKeyTrigger(VK_S))		m_Shop->Input_Down();
		if (input.GetKeyTrigger(VK_A))		m_Shop->Input_Left();
		if (input.GetKeyTrigger(VK_D))		m_Shop->Input_Right();

		if (input.GetKeyTrigger(VK_RETURN))
		{
			if (m_Shop->GetFocus() == SHOP_FOCUS::FOCUS_UPGRADE)
			{
				if (m_Shop->BuyUpgradeData())
				{
					// 強化成功SE再生予定
				}
			}
			else
			{
				// シーン遷移開始で画面を暗く
				if (ui_fade && ui_fade->GetState() == FADE_STATE::FADE_STATE_NONE)
				{
					ui_fade->SetFadeIn();
				}
			}
		}
	}
	
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	// 画面暗くなったの確認後、遷移
	if (ui_fade && ui_fade->IsFadeFinished() && ui_fade->GetAlpha() >= 1.0f)
	{
		if (m_Shop->GetMenuIndex() == 0)
		{
			SceneManager::ChangeScene(SCENE_ID_GAME, camera, uiCamera, hwnd);
		}
		else
		{
			SceneManager::ChangeScene(SCENE_ID_TITLE, camera, uiCamera, hwnd);
		}
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
		obj->UnInit();
	}
	objectInstance.clear();
}

void ResultScene::Update_Input()
{
	// 上移動
	if (input.GetKeyPress(VK_E)) camera->MoveUp(0.1f);
	// 下移動
	if (input.GetKeyPress(VK_Q)) camera->MoveDown(0.1f);

	// 右移動
	if (input.GetKeyPress(VK_D)) camera->MoveRight(0.1f);
	// 左移動
	if (input.GetKeyPress(VK_A)) camera->MoveLeft(0.1f);
	// 前移動
	if (input.GetKeyPress(VK_W)) camera->MoveForward(0.1f);
	// 後移動
	if (input.GetKeyPress(VK_S)) camera->MoveBack(0.1f);

	// 右方向回転
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_D)) camera->Rotate_Yaw(XMConvertToRadians(0.1f));
	// 左方向回転
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_A)) camera->Rotate_Yaw(XMConvertToRadians(-0.1f));
	// 上向きに回転
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_W)) camera->Rotate_Pitch(XMConvertToRadians(-0.1f));
	// 下向きに回転
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_S)) camera->Rotate_Pitch(XMConvertToRadians(0.1f));
	// マウス右入力で自由にカメラを回転
	Update_MouseRotate(0.001f);
}

void ResultScene::Update_MouseRotate(float _sensi)
{
	// マウスの現在位置取得
	POINT currentMousePos;
	GetCursorPos(&currentMousePos);
	ScreenToClient(hwnd, &currentMousePos);

	// 差分取得
	static POINT lastMousePos = currentMousePos;
	int dx = currentMousePos.x - lastMousePos.x;
	int dy = currentMousePos.y - lastMousePos.y;
	lastMousePos = currentMousePos;

	// マウス感度
	float sensi = _sensi;

	if (GetForegroundWindow() == hwnd)
	{
		// マウス右入力でカメラを回転
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			camera->Rotate_Yaw(dx * sensi);
			camera->Rotate_Pitch(dy * sensi);
		}
	}
}

void ResultScene::Draw_ImGui()
{
#if _DEBUG
	ImGui_Prop();
	ImGui_Shop();
#endif
}

void ResultScene::ImGui_Prop()
{
	ImGui::Begin("SceneName:ResultScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void ResultScene::ImGui_Shop()
{
	ImGui::Begin("Shop");
	ImGui::Text("Current Score: %d", SceneManager::GetGameStatus().score);

	auto& data = m_Shop->GetAllData();
	int select = m_Shop->GetUpgradeIndex();

	for (int i = 0; i < data.size(); ++i)
	{
		bool isSelected = (i == select);
		if (isSelected) {
			ImGui::Text(">> ");
			ImGui::SameLine();
		}
		ImGui::Text("%s Lv:%d Cost:%d", data[i].name.c_str(), 
			data[i].currentLevel, data[i].cost);
	}
	ImGui::End();
}