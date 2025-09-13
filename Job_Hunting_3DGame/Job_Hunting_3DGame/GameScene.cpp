#include "GameScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"
#include "Debug_New.h"

using namespace DirectX;

Object* GameScene::CreateObj(const std::string& _objectID)
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

void GameScene::Init(Camera* _camera, Camera2D* _uiCamera, HWND _hwnd)
{
	printf("シーン名：GameScene\n");

	hwnd = _hwnd;
	camera = _camera;
	uiCamera = _uiCamera;

	camera->SetPos(XMVectorSet(0.0f, 2.5f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	uiCamera->Init(screenWidth, screenHeight);

	// プロトタイプ登録
	prototypeManager->AddPrototype("Sky", std::make_unique<SkyDomeMesh>());
	prototypeManager->AddPrototype("Player", std::make_unique<Player>());
	prototypeManager->AddPrototype("WaterMesh", std::make_unique<WaterMesh>());
	prototypeManager->AddPrototype("Goal", std::make_unique<Goal>());
	prototypeManager->AddPrototype("UI", std::make_unique<UI>());
	prototypeManager->AddPrototype("UI_Fade", std::make_unique<UI_Fade>());

	XMVECTOR camPos = camera->GetPos();
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, camPos);

	SkyDomeMesh* sky = dynamic_cast<SkyDomeMesh*>(CreateObj("Sky"));
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
	goal->SetPos(XMVectorSet(0.0f, -1.0f, 50.0f, 0.0f));
	goal->SetRota(XMVectorZero());
	goal->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	goal->m_tags.AddTag("Goal");

	WaterMesh* waterMesh = dynamic_cast<WaterMesh*>(CreateObj("WaterMesh"));
	waterMesh->Init(camera);
	waterMesh->SetPos(XMVectorZero());
	waterMesh->SetRota(XMVectorZero());
	waterMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	waterMesh->m_tags.AddTag("WaterMesh");

	UI* ui_test = dynamic_cast<UI*>(CreateObj("UI"));
	ui_test->Init(uiCamera, 480.0f, 270.0f, L"Assets/Texture/hogehoge.png");
	ui_test->SetPos(XMVectorSet(720.0f, 405.0f, 0.0f, 0.0f));
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

	playerCtrl = std::make_unique<PlayerController>();
	playerCtrl->Init(player, waterMesh, camera, &BaseScene::input);

	game = std::make_unique<Game>();
	game->Init(player, goal);
}

void GameScene::Update(float _deltaTime)
{
	//Update_Input();
	game->Update(_deltaTime);

	playerCtrl->Update(_deltaTime);

	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	auto ui_fade = FindByTag<UI_Fade>("UI_Fade");
	if (ui_fade && ui_fade->GetState() == FADE_STATE::FADE_STATE_NONE)
	{
		if (input.GetKeyTrigger(VK_RETURN) || game->GetGoalFlag())
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
	ImGui_PlayerController();
	ImGui_Goal();
	ImGui_WaterMesh();
	ImGui_Timer();
}

void GameScene::ImGui_Prop()
{
	ImGui::Begin("SceneName:GameScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
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
			ImGui::SliderFloat("Camera_FollowSpeed", &followSpeed, 12.0f, 50.0f);
			playerCtrl->SetFollowSpeed(followSpeed);
		}
	}
	ImGui::End();
}

void GameScene::ImGui_Goal()
{
	ImGui::Begin("Goal");
	if (ImGui::CollapsingHeader("Goal", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Goal* goal = nullptr;
		for (auto& obj : objectInstance)
		{
			// ゴールを検索
			if (obj->m_tags.SearchTag("Goal"))
			{
				goal = dynamic_cast<Goal*>(obj.get());
				break;
			}
		}

		if (goal)
		{
			XMVECTOR position = goal->GetPos();
			XMFLOAT3 pos;
			XMStoreFloat3(&pos, position);

			// ゴールの座標の表示＋編集
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

		}
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
			// 水面メッシュを検索
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

void GameScene::ImGui_Timer()
{
	if (!game) return;
	ImGui::Begin("RaceTimer");

	int countDown = game->GetCountDownRemaining();
	if (countDown > 0)
	{
		// カウントダウン表示
		ImGui::SetWindowFontScale(5.0f);
		ImVec2 windowSize = ImGui::GetWindowSize();
		char buffer[8];
		snprintf(buffer, sizeof(buffer), "%d", countDown);
		ImVec2 textSize = ImGui::CalcTextSize(buffer);
		ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
		ImGui::Text("%s", buffer);
		ImGui::SetWindowFontScale(1.0f);
	}
	else
	{
		// レースタイマー表示
		auto currentElapsed = game->GetElapsedTime();

		int min = static_cast<int>(std::chrono::duration_cast
			<std::chrono::minutes>(currentElapsed).count());
		int sec = static_cast<int>(std::chrono::duration_cast
			<std::chrono::seconds>(currentElapsed).count() % 60);
		int cenSec = static_cast<int>((currentElapsed.count() % 1000) / 10);

		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", min, sec, cenSec);

		ImGui::SetWindowFontScale(3.0f);
		ImGui::Text("Time: %s", buffer);
		ImGui::SetWindowFontScale(1.0f);
	}

	ImGui::End();
}