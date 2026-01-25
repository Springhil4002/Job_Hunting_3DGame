#include "GameScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"
#include "DirectionalLight.h"

using namespace DirectX;

Object* GameScene::CreateObj(const std::string& _objectID)
{
	auto obj = prototypeManager->Create(_objectID);
	if (obj)
	{
		// 所有権をシーン内管理配列に映す
		objectInstance.push_back(std::move(obj));
		return objectInstance.back().get();
	}
	return nullptr;
}

void GameScene::Init(Camera* _camera, Camera2D* _uiCamera, HWND _hwnd)
{
	printf("シーン名：GameScene\n");
	srand(static_cast<unsigned int>(time(NULL)));

	SceneManager::GetSound().Play(SOUND_LABEL_BGM_002);

	hwnd = _hwnd;
	camera = _camera;
	uiCamera = _uiCamera;

	// カメラ設定
	camera->SetPos(XMVectorSet(0.0f, 2.3f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	uiCamera->Init(screenWidth, screenHeight);

	// プロトタイプ登録
	prototypeManager->AddPrototype("Sky", std::make_unique<SkyBox>());
	prototypeManager->AddPrototype("Player", std::make_unique<Player>());
	prototypeManager->AddPrototype("Goal", std::make_unique<Goal>());
	prototypeManager->AddPrototype("UI", std::make_unique<UI>());
	prototypeManager->AddPrototype("UI_Fade", std::make_unique<UI_Fade>());
	prototypeManager->AddPrototype("UI_Timer", std::make_unique<UI_Timer>());
	prototypeManager->AddPrototype("UI_Speed", std::make_unique<UI_Speed>());
	prototypeManager->AddPrototype("UI_Score", std::make_unique<UI_Score>());
	prototypeManager->AddPrototype("SeaMesh", std::make_unique<SeaMesh>());

	// ライト設定
	DirectionalLight::Instance().SetLightDir({ 0.35f,-1.0f,0.15f });
	DirectionalLight::Instance().SetEnvStrength(0.65f);
	DirectionalLight::Instance().SetLightColor({ 1.0f,1.0f,1.0f,1.0f });

	// ゲームステータス
	GameStatus currentStatus = SceneManager::GetGameStatus();

	//===========================オブジェクト生成==============================//

	// スカイボックス
	SkyBox* sky = dynamic_cast<SkyBox*>(CreateObj("Sky"));
	sky->Init(camera);
	sky->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	sky->SetRota(XMVectorZero());
	sky->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	sky->m_tags.AddTag("SkyDome");

	// 水面メッシュ
	SeaMesh* seaMesh = dynamic_cast<SeaMesh*>(CreateObj("SeaMesh"));
	seaMesh->Init(camera);
	seaMesh->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	seaMesh->SetRota(XMVectorZero());
	seaMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	seaMesh->m_tags.AddTag("SeaMesh");

	// プレイヤー
	Player* player = dynamic_cast<Player*>(CreateObj("Player"));
	player->Init(camera);
	player->SetPos(XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f));
	player->SetRota(XMVectorZero());
	player->SetScale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.0f));
	player->m_tags.AddTag("Player");
	
	// プレイヤー操作系
	playerCtrl = std::make_unique<PlayerController>();
	playerCtrl->Init(player, seaMesh, camera, &BaseScene::input);
	playerCtrl->SetAcceleration(currentStatus.accelSpeed);
	playerCtrl->SetMaxSpeed(currentStatus.maxSpeed);
	
	// ゲームシステム
	game = std::make_unique<Game>();
	game->SetGameStatus(currentStatus);
	int CreateGoalNum = game->GetCreateGoalCount();
	// Goalオブジェクト群
	std::vector<Goal*> goals;
	
	// 配置パラメータ
	const float distance = 20.0f;
	const float rangeX = 200.0f;
	const float rangeZ = 200.0f;
	std::vector<XMFLOAT3> goalPositions;

	// プレイヤー位置に重なり配置を防ぐために最初に登録
	XMFLOAT3 playerPos;
	XMStoreFloat3(&playerPos, player->GetPos());
	goalPositions.push_back(playerPos);

	for (int i = 0; i < CreateGoalNum; ++i)
	{
		float posX, posZ;		// X,Z
		bool isFar = false;		// 一定距離離れているか
		int trials = 0;			// 試行回数

		while (!isFar && trials < 50)
		{
			posX = (static_cast<float>(rand()) / RAND_MAX) * (rangeX * 2.0f) - rangeX;
			posZ = (static_cast<float>(rand()) / RAND_MAX) * (rangeZ * 2.0f) - rangeX;
			
			isFar = true;
			for (const auto& p : goalPositions)
			{
				float dx = posX - p.x;
				float dz = posZ - p.z;
				float dist = dx * dx + dz * dz;

				// 一定距離未満ならリテイク
				if (dist < distance * distance)
				{
					isFar = false;
					break;
				}
			}
			trials++;
		}
		// 配置を確定
		goalPositions.push_back({ posX,-1.0f,posZ });

		Goal* goal = dynamic_cast<Goal*>(CreateObj("Goal"));
		goal->Init(camera);
		goal->SetPos(XMVectorSet(posX, -1.0f, posZ, 0.0f));
		float rotaY = XMConvertToRadians((rand() % 8) * 45.0f);
		goal->SetRota(XMVectorSet(0.0f, rotaY, 0.0f, 0.0f));
		goal->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
		goal->m_tags.AddTag("Goal");

		goals.push_back(goal);
	}
	game->Init(player, goals);

	//================================UI生成================================//

	// 操作方法UI
	UI* ui_con = dynamic_cast<UI*>(CreateObj("UI"));
	ui_con->Init(uiCamera, 600.0f, 100.0f, L"Assets/Texture/UI_Game_controller.png");
	ui_con->SetPos(XMVectorSet(-650.0f, -500.0f, 0.0f, 0.0f));
	ui_con->SetRota(XMVectorZero());
	ui_con->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_con->UpdateTransform();
	ui_con->UpdateCameraMatrix();
	ui_con->m_tags.AddTag("UI_con");

	// タイマーUI
	UI_Timer* timer = dynamic_cast<UI_Timer*>(CreateObj("UI_Timer"));
	timer->Init(uiCamera, 50.0f, 100.0f, L"Assets/Texture/UI_Number.png");
	timer->SetGame(game.get());
	timer->m_tags.AddTag("UI_Timer");
	
	// スピードメーターUI
	UI_Speed* speed = dynamic_cast<UI_Speed*>(CreateObj("UI_Speed"));
	speed->Init(uiCamera, 50.0f, 100.0f, L"Assets/Texture/UI_Number.png", L"Assets/Texture/UI_Km.png");
	speed->Set_PlayerController(playerCtrl.get());
	speed->m_tags.AddTag("UI_Speed");

	// スコアUI
	UI_Score* score = dynamic_cast<UI_Score*>(CreateObj("UI_Score"));
	score->Init(uiCamera, 50.0f, 100.0f, L"Assets/Texture/UI_Number.png",L"Assets/Texture/UI_Score.png");
	score->m_tags.AddTag("UI_Score");

	// フェード
	UI_Fade* ui_fade = dynamic_cast<UI_Fade*>(CreateObj("UI_Fade"));
	ui_fade->Init(uiCamera, 1920.0f, 1080.0f);
	ui_fade->SetTransform(
		XMVectorZero(),
		XMVectorZero(),
		XMVectorSet(1.0f, 1.0, 1.0f, 0.0f), 1.0f);
	ui_fade->SetFadeOut();
	ui_fade->m_tags.AddTag("UI_Fade");
}

void GameScene::Update(float _deltaTime)
{
	//Update_Input();
	game->Update(_deltaTime);
	// カウントダウン中はプレイヤー操作不可
	if (game->GetState() == RACE_STATE::RACE_STATE_COUNTDOWN)
		playerCtrl->SetPlayed(false);
	else
		playerCtrl->SetPlayed(true);

	playerCtrl->Update(_deltaTime);

	DirectionalLight::Instance().UpdateLightFollowCamera(camera);
	
	// 全オブジェクト更新
	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	auto ui_fade = FindByTag<UI_Fade>("UI_Fade");
	if (ui_fade && ui_fade->GetState() == FADE_STATE::FADE_STATE_NONE)
	{
		if (input.GetKeyTrigger(VK_SPACE) || 
			game->GetTimeUpFlag())
		{
			ui_fade->SetFadeIn();
			playerCtrl->SetPlayed(false);
		}
	}

	if (ui_fade && ui_fade->IsFadeFinished() && ui_fade->GetAlpha() >= 1.0f)
	{
		SceneManager::ChangeScene(SCENE_ID_RESULT, camera, uiCamera, hwnd);
	}
}

void GameScene::Draw()
{
	for (auto& obj : objectInstance)
	{
		obj->Draw();
	}

	playerCtrl->Draw();
}

void GameScene::Uninit()
{
	// BGM停止
	SceneManager::GetSound().Stop(SOUND_LABEL_BGM_002);

	playerCtrl->UnInit();
	playerCtrl.reset();
	game->UnInit();
	game.reset();

	for (auto& obj : objectInstance)
	{
		obj->UnInit();
	}
	objectInstance.clear();
}

void GameScene::Update_Input()
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

void GameScene::Update_MouseRotate(float _sensi)
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

void GameScene::Draw_ImGui()
{
#if _DEBUG
	ImGui_Prop();
	playerCtrl->Draw_ImGui();
	game->Draw_ImGui();
#endif
}

void GameScene::ImGui_Prop()
{
	ImGui::Begin("SceneName:GameScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::End();
}