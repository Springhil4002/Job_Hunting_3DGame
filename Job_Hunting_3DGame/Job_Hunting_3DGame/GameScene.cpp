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

	hwnd = _hwnd;
	camera = _camera;
	uiCamera = _uiCamera;

	// カメラ設定
	camera->SetPos(XMVectorSet(0.0f, 2.3f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	uiCamera->Init(screenWidth, screenHeight);

	XMVECTOR camPos = camera->GetPos();
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, camPos);

	// プロトタイプ登録
	prototypeManager->AddPrototype("Sky", std::make_unique<SkyBox>());
	prototypeManager->AddPrototype("Player", std::make_unique<Player>());
	prototypeManager->AddPrototype("WaterMesh", std::make_unique<WaterMesh>());
	prototypeManager->AddPrototype("Goal", std::make_unique<Goal>());
	prototypeManager->AddPrototype("UI", std::make_unique<UI>());
	prototypeManager->AddPrototype("UI_Fade", std::make_unique<UI_Fade>());
	prototypeManager->AddPrototype("UI_Timer", std::make_unique<UI_Timer>());
	
	// ライト設定
	DirectionalLight::Instance().SetLightDir({ 0.35f,-1.0f,0.15f });
	DirectionalLight::Instance().SetEnvStrength(0.65f);
	DirectionalLight::Instance().SetLightColor({ 1.0f,1.0f,1.0f,1.0f });

	// オブジェクト生成
	SkyBox* sky = dynamic_cast<SkyBox*>(CreateObj("Sky"));
	sky->Init(camera);
	sky->SetPos(XMLoadFloat3(&pos));
	sky->SetRota(XMVectorZero());
	sky->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	sky->m_tags.AddTag("SkyDome");

	Player* player = dynamic_cast<Player*>(CreateObj("Player"));
	player->Init(camera);
	player->SetPos(XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f));
	player->SetRota(XMVectorZero());
	player->SetScale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.0f));
	player->m_tags.AddTag("Player");
	
	Goal* goal = dynamic_cast<Goal*>(CreateObj("Goal"));
	goal->Init(camera);
	goal->SetPos(XMVectorSet(50.0f, -1.0f, 100.0f, 0.0f));
	goal->SetRota(XMVectorZero());
	goal->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	goal->m_tags.AddTag("Goal");

	Goal* goal2 = dynamic_cast<Goal*>(CreateObj("Goal"));
	goal2->Init(camera);
	goal2->SetPos(XMVectorSet(0.0f, -1.0f, 50.0f, 0.0f));
	goal2->SetRota(XMVectorZero());
	goal2->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	goal2->m_tags.AddTag("Goal");

	Goal* goal3 = dynamic_cast<Goal*>(CreateObj("Goal"));
	goal3->Init(camera);
	goal3->SetPos(XMVectorSet(-50.0f, -1.0f, 100.0f, 0.0f));
	goal3->SetRota(XMVectorSet(0.0f, XMConvertToRadians(90.0f), 0.0f, 0.0f));
	goal3->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	goal3->m_tags.AddTag("Goal");

	Goal* goal4 = dynamic_cast<Goal*>(CreateObj("Goal"));
	goal4->Init(camera);
	goal4->SetPos(XMVectorSet(-50.0f, -1.0f, -100.0f, 0.0f));
	goal4->SetRota(XMVectorSet(0.0f, XMConvertToRadians(45.0f), 0.0f, 0.0f));
	goal4->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	goal4->m_tags.AddTag("Goal");

	Goal* goal5 = dynamic_cast<Goal*>(CreateObj("Goal"));
	goal5->Init(camera);
	goal5->SetPos(XMVectorSet(50.0f, -1.0f, -100.0f, 0.0f));
	goal5->SetRota(XMVectorSet(0.0f, XMConvertToRadians(70.0f), 0.0f, 0.0f));
	goal5->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	goal5->m_tags.AddTag("Goal");

	WaterMesh* waterMesh = dynamic_cast<WaterMesh*>(CreateObj("WaterMesh"));
	waterMesh->Init(camera);
	waterMesh->SetPos(XMVectorZero());
	waterMesh->SetRota(XMVectorZero());
	waterMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	waterMesh->m_tags.AddTag("WaterMesh");

	playerCtrl = std::make_unique<PlayerController>();
	playerCtrl->Init(player, waterMesh, camera, &BaseScene::input);

	std::vector<Goal*> goals;
	for (auto& obj : objectInstance)
	{
		if (obj->m_tags.SearchTag("Goal"))
		{
			goals.push_back(dynamic_cast<Goal*>(obj.get()));
		}
	}
	game = std::make_unique<Game>();
	game->Init(player, goals);

	// UI系生成
	UI* ui_if = dynamic_cast<UI*>(CreateObj("UI"));
	ui_if->Init(uiCamera, 500.0f, 200.0f, L"Assets/Texture/Game_if.png");
	ui_if->SetTransform(
		XMVectorSet(-740.0f, 450.0f, 0.0f, 0.0f),
		XMVectorZero(),
		XMVectorSet(1.0f, 1.0, 1.0f, 0.0f), 1.0f);
	ui_if->m_tags.AddTag("UI_If");

	UI* ui_con = dynamic_cast<UI*>(CreateObj("UI"));
	ui_con->Init(uiCamera, 600.0f, 100.0f, L"Assets/Texture/Game_controller.png");
	ui_con->SetPos(XMVectorSet(-650.0f, -500.0f, 0.0f, 0.0f));
	ui_con->SetRota(XMVectorZero());
	ui_con->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	ui_con->UpdateTransform();
	ui_con->UpdateCameraMatrix();
	ui_con->m_tags.AddTag("UI_con");

	UI_Timer* timer = dynamic_cast<UI_Timer*>(CreateObj("UI_Timer"));
	timer->Init(uiCamera, 50.0f, 100.0f, L"Assets/Texture/Number.png");
	timer->SetGame(game.get());
	timer->m_tags.AddTag("UI_Timer");

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
	if (game->GetState() == RACE_STATE::RACE_STATE_COUNTDOWN)
		playerCtrl->SetPlayed(false);
	else
		playerCtrl->SetPlayed(true);

	playerCtrl->Update(_deltaTime);

	DirectionalLight::Instance().UpdateLightFollowCamera(camera);

	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	auto timer = FindByTag<UI_Timer>("UI_Timer");
	if (timer)
		timer->SetTime(game->GetElapsedTime());
	
	auto ui_fade = FindByTag<UI_Fade>("UI_Fade");
	if (ui_fade && ui_fade->GetState() == FADE_STATE::FADE_STATE_NONE)
	{
		if (input.GetKeyTrigger(VK_SPACE) || game->GetGoalFlag())
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
	playerCtrl->Uninit();
	playerCtrl.reset();
	game->Uninit();
	game.reset();

	for (auto& obj : objectInstance)
	{
		obj->Uninit();
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
	ImGui_Prop();
	
	//ImGui_PlayerController();
	//ImGui_Goal();
	//ImGui_WaterMesh();
	//ImGui_Camera();
}

void GameScene::ImGui_Prop()
{
	ImGui::Begin("SceneName:GameScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

	const auto& light = DirectionalLight::Instance().GetLightData();
	ImGui::Text("LightDir: X=%.2f Y=%.2f Z=%.2f",
		light.lightDir.x, light.lightDir.y, light.lightDir.z);

	ImGui::Text("Press Enter to Result!");
	ImGui::End();
}

void GameScene::ImGui_PlayerController()
{
	ImGui::Begin("PlayerController");
	// プレイヤーの状態
	if (playerCtrl)
	{
		// プレイヤーの座標
		XMVECTOR position = playerCtrl->GetPosition();
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, position);

		// プレイヤーの前方向ベクトル
		XMVECTOR forwardVec = playerCtrl->GetForwardVec();
		XMFLOAT3 forward;
		XMStoreFloat3(&forward, forwardVec);

		// プレイヤーの現在速度
		float currentSpeed = XMVectorGetX(XMVector3Length(playerCtrl->GetVelocity()));
		float maxSpeed = playerCtrl->GetMaxSpeed();

		// カメラの追尾速度
		float followSpeed = playerCtrl->GetFollowSpeed();

		if (ImGui::CollapsingHeader("PlayerInfo", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// プレイヤーの座標の表示＋編集
			if (ImGui::TreeNodeEx("Position", ImGuiTreeNodeFlags_DefaultOpen))
			{
				float posArray[3] = { pos.x,pos.y,pos.z };
				if (ImGui::DragFloat3("Player Position", posArray, 0.1f))
				{
					playerCtrl->SetPosition(XMVectorSet(
						posArray[0], posArray[1], posArray[2], 0.0f));
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("ForwardVec", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("X: %.3f", forward.x);	ImGui::SameLine();
				ImGui::Text("Y: %.3f", forward.y);	ImGui::SameLine();
				ImGui::Text("Z: %.3f", forward.z);
				ImGui::TreePop();
			}

			// プレイヤーの速度(調節可能)
			ImGui::Text("Player_CurrentSpeed: %.3f/%.3f", currentSpeed, maxSpeed);
			ImGui::SliderFloat("Player_MaxSpeed", &maxSpeed, 1.0f, 200.0f);
			playerCtrl->SetMaxSpeed(maxSpeed);

			// カメラの追尾速度(調節可能)
			ImGui::SliderFloat("Camera_FollowSpeed", &followSpeed, 10.0f, 200.0f);
			playerCtrl->SetFollowSpeed(followSpeed);
		}
	}
	ImGui::End();
}

void GameScene::ImGui_Goal()
{
	ImGui::Begin("Goal_Manager");

	// ゴールオブジェクト一覧を作る
	std::vector<Goal*> goals;
	for (auto& obj : objectInstance)
	{
		if (obj->m_tags.SearchTag("Goal"))
		{
			goals.push_back(dynamic_cast<Goal*>(obj.get()));
		}
	}
	// 選択中ゴールオブジェクトのインデックス
	static int selectIndex = 0;
	
	// ゴールオブジェクトのリスト表示
	if (!goals.empty())
	{
		ImGui::Text("Select Goal:");
		for (int i = 0; i < goals.size(); ++i)
		{
			std::string label = "Goal" + std::to_string(i);
			if (ImGui::Selectable(label.c_str(), selectIndex == i))
				selectIndex = i;
		}
		// 選択されたゴールオブジェクトの座表表示＋編集
		Goal* goal = goals[selectIndex];
		XMVECTOR posVec = goal->GetPos();
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, posVec);

		if (ImGui::TreeNodeEx("Position", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float posArray[3] = { pos.x,pos.y,pos.z };
			if (ImGui::DragFloat3("Goal Position", posArray, 0.1f))
			{
				goal->SetPos(XMVectorSet(
					posArray[0], posArray[1], posArray[2], 0.0f));
			}
			ImGui::TreePop();
		}

		goal->Draw_ImGui();
	}
	else
	{
		ImGui::Text("No Goals Create!");
	}
	ImGui::End();
}

void GameScene::ImGui_WaterMesh()
{
	ImGui::Begin("WaterMesh");
	if (ImGui::CollapsingHeader("WaterMesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		WaterMesh* waterMesh = nullptr;
		for (auto& obj : objectInstance)
		{
			if (obj->m_tags.SearchTag("WaterMesh"))
			{
				waterMesh = dynamic_cast<WaterMesh*>(obj.get());
				break;
			}
		}

		if (waterMesh)
		{
			static float gridSize = waterMesh->GetGridSize();
			ImGui::DragFloat("GridSize", &gridSize, 2.0f, 64.0f, 1024.0f);
			if (ImGui::Button("Apply Grid Size"))
			{
				waterMesh->Update_GridSize(gridSize);
			}
		}
	}
	ImGui::End();
}

void GameScene::ImGui_Camera()
{
	ImGui::Begin("Camera");
	if (camera)
	{
		// 座表表示
		XMVECTOR camPosVec = camera->GetPos();
		XMFLOAT3 camPos;
		XMStoreFloat3(&camPos, camPosVec);
		ImGui::Text("Position:");
		ImGui::Text("X:%.3f Y:%.3f Z:%.3f", camPos.x, camPos.y, camPos.z);

		// 注視点表示
		XMVECTOR camTargetVec = camera->GetTarget();
		XMFLOAT3 camTarget;
		XMStoreFloat3(&camTarget, camTargetVec);
		ImGui::Text("Target:");
		ImGui::Text("X:%.3f Y:%.3f Z:%.3f", camTarget.x, camTarget.y, camTarget.z);
	}
	ImGui::End();
}