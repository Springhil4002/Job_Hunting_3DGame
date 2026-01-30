#include "TitleScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"
#include "DirectionalLight.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

using namespace DirectX;

Object* TitleScene::CreateObj(const std::string& _objectID)
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

void TitleScene::Init(Camera* _camera, Camera2D* _uiCamera, HWND _hwnd)
{
	DEBUG_LOG(L"シーン名:TitleScene");

	SceneManager::GetSound().Play(SOUND_LABEL_BGM_001);

	hwnd = _hwnd;
	camera = _camera;
	uiCamera = _uiCamera;

	camera->SetPos(XMVectorSet(0.0f, 2.5f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	uiCamera->Init(screenWidth, screenHeight);
	
	// プロトタイプ登録
	prototypeManager->AddPrototype("UI", std::make_unique<UI>());
	prototypeManager->AddPrototype("UI_Fade", std::make_unique<UI_Fade>());
	prototypeManager->AddPrototype("UI_Flash", std::make_unique<UI_Flash>());
	prototypeManager->AddPrototype("Sky", std::make_unique<SkyBox>());
	prototypeManager->AddPrototype("Player", std::make_unique<Player>());
	prototypeManager->AddPrototype("SeaMesh", std::make_unique<SeaMesh>());

	// ライト設定
	DirectionalLight::Instance().SetLightDir({ 0.35f,-1.0f,0.15f });
	DirectionalLight::Instance().SetEnvStrength(0.65f);
	DirectionalLight::Instance().SetLightColor({ 1.0f,1.0f,1.0f,1.0f });

	SkyBox* sky = dynamic_cast<SkyBox*>(CreateObj("Sky"));
	sky->Init(camera);
	sky->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	sky->SetRota(XMVectorZero());
	sky->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	sky->m_tags.AddTag("SkyDome");

	Player* player = dynamic_cast<Player*>(CreateObj("Player"));
	player->Init(camera);
	player->SetPos(XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f));
	player->SetRota(XMVectorZero());
	player->SetScale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.0f));
	player->m_tags.AddTag("Player");

	SeaMesh* seaMesh = dynamic_cast<SeaMesh*>(CreateObj("SeaMesh"));
	seaMesh->Init(camera);
	seaMesh->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	seaMesh->SetRota(XMVectorZero());
	seaMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	seaMesh->m_tags.AddTag("SeaMesh");

	autoPlayerCtrl = std::make_unique<AutoPlayerController>();
	autoPlayerCtrl->Init(player, seaMesh, camera);

	UI* ui_logo = dynamic_cast<UI*>(CreateObj("UI"));
	ui_logo->Init(uiCamera, 720.0f, 540.0f, L"Assets/Texture/UI_Title_Logo.png");
	ui_logo->SetPos(XMVectorSet(-620.0f, 400.0f, 0.0f, 0.0f));
	ui_logo->SetRota(XMVectorZero());
	ui_logo->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_logo->UpdateTransform();
	ui_logo->UpdateCameraMatrix();
	ui_logo->m_tags.AddTag("UI_Logo");

	UI_Flash* ui_start = dynamic_cast<UI_Flash*>(CreateObj("UI_Flash"));
	ui_start->Init(uiCamera, 720.0f, 150.0f, L"Assets/Texture/UI_Title_Start.png");
	ui_start->SetPos(XMVectorSet(-550.0f, -250.0f, 0.0f, 0.0f));
	ui_start->SetRota(XMVectorZero());
	ui_start->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_start->UpdateTransform();
	ui_start->UpdateCameraMatrix(); 
	ui_start->m_tags.AddTag("UI_Start");
	
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

void TitleScene::Update(float _deltaTime)
{
	//Update_Input();
	
	// 自動操縦更新
	autoPlayerCtrl->Update(_deltaTime);
	// ライトをカメラ追従更新
	DirectionalLight::Instance().UpdateLightFollowCamera(camera);

	// シーン内の全オブジェクト更新
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	// シーン遷移開始で画面を暗く
	auto ui_fade = FindByTag<UI_Fade>("UI_Fade");
	auto ui_start = FindByTag<UI_Flash>("UI_Start");
	if (ui_fade && ui_fade->GetState() == FADE_STATE::FADE_STATE_NONE)
	{
		if (input.GetKeyTrigger(VK_SPACE))
		{
			ui_fade->SetFadeIn();
			if (ui_start)
			{
				// 遷移時、点滅なし
				ui_start->SetAlpha(1.0f);
				ui_start->SetFlashNone();
			}
		}
	}

	// 画面暗くなったの確認後、遷移
	if (ui_fade && ui_fade->IsFadeFinished() && ui_fade->GetAlpha() >= 1.0f)
	{
		SceneManager::ChangeScene(SCENE_ID_GAME, camera, uiCamera, hwnd);
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
	// BGM停止
	SceneManager::GetSound().Stop(SOUND_LABEL_BGM_001);

	for (auto& obj : objectInstance)
	{
		obj->UnInit();
	}
	objectInstance.clear();
}

void TitleScene::Update_Input()
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

void TitleScene::Update_MouseRotate(float _sensi)
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

void TitleScene::Draw_ImGui()
{
#if _DEBUG
	ImGui_Prop();
#endif
}

void TitleScene::ImGui_Prop()
{
	ImGui::Begin("SceneName:TitleScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();
}