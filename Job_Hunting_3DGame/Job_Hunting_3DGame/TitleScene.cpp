#include "TitleScene.h"
#include "SceneManager.h"
#include "System/ImGui/imgui.h"

using namespace DirectX;

Object* TitleScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void TitleScene::Init(Camera* _camera,HWND _hwnd)
{
	printf("シーン名：TitleScene\n");

	camera = _camera;
	camera->SetPos(XMVectorSet(0.0f, 2.5f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	hwnd = _hwnd;
	
	// プロトタイプ登録
	prototypeManager->AddPrototype("Sky", new SkyDomeMesh);
	prototypeManager->AddPrototype("Player", new Player);
	prototypeManager->AddPrototype("WaterMesh", new WaterMesh);
	prototypeManager->AddPrototype("Goal", new Goal);

	XMVECTOR camPos = camera->GetPos();
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, camPos);
	pos.y -= 100.0f;

	SkyDomeMesh* sky = static_cast<SkyDomeMesh*>(CreateObj("Sky"));
	sky->Init(camera);
	sky->SetPos(XMLoadFloat3(&pos));
	sky->SetRota(XMVectorZero());
	sky->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	sky->m_tags.AddTag("SkyDome");
	objectInstance.insert(sky);

	Player* player = static_cast<Player*>(CreateObj("Player"));
	player->Init(camera);
	player->SetPos(XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f));
	player->SetRota(XMVectorZero());
	player->SetScale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.0f));
	player->m_tags.AddTag("Player");
	objectInstance.insert(player);

	Goal* goal = static_cast<Goal*>(CreateObj("Goal"));
	goal->Init(camera);
	goal->SetPos(XMVectorSet(0.0f, -1.0f, 50.0f, 0.0f));
	goal->SetRota(XMVectorZero());
	goal->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	goal->m_tags.AddTag("Goal");
	objectInstance.insert(goal);

	WaterMesh* waterMesh = static_cast<WaterMesh*>(CreateObj("WaterMesh"));
	waterMesh->Init(camera);
	waterMesh->SetPos(XMVectorZero());
	waterMesh->SetRota(XMVectorZero());
	waterMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	waterMesh->m_tags.AddTag("WaterMesh");
	objectInstance.insert(waterMesh);

	playerCtrl = new PlayerController();
	playerCtrl->Init(player, waterMesh, camera, &BaseScene::input);

	game = new Game();
	game->Init(player, goal);
}

void TitleScene::Update(float _deltaTime)
{
	//Update_Input();
	game->Update(_deltaTime);

	playerCtrl->Update(_deltaTime);

	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN) || game->GetGoalFlag())
	{
		SceneManager::ChangeScene(SCENE_ID_GAME, camera, hwnd);
	}
}

void TitleScene::Draw()
{
	for (auto& obj : objectInstance)
	{
		obj->Draw();
	}

	playerCtrl->Draw();
}

void TitleScene::Uninit()
{
	for (auto& obj : objectInstance)
	{
		obj->Uninit();
	}
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
	ImGui_Prop();
	ImGui_PlayerController();
	ImGui_Goal();
	ImGui_WaterMesh();
	ImGui_Timer();
}

void TitleScene::ImGui_Prop()
{
	ImGui::Begin("SceneName:TitleScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::Text("Press Enter to Game!");
	ImGui::End();
}

void TitleScene::ImGui_PlayerController()
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

void TitleScene::ImGui_Goal()
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
				goal = static_cast<Goal*>(obj);
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

void TitleScene::ImGui_WaterMesh()
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
				waterMesh = static_cast<WaterMesh*>(obj);
				break;
			}
		}

		if (waterMesh)
		{
			static int gridSize = waterMesh->GetGridSize();
			ImGui::DragInt("GridSize", &gridSize, 2.0f, 64, 1024);
			if (ImGui::Button("Apply Grid Size"))
			{
				waterMesh->Update_GridSize(gridSize);
			}
		}
	}
	ImGui::End();
}

void TitleScene::ImGui_Timer()
{
	ImGui::Begin("RaceTimer");

	// SPACEキー入力で開始・停止
	if (input.GetKeyTrigger(VK_SPACE))
	{
		if (!runningTimer)
		{
			// 計測開始
			startTime = std::chrono::steady_clock::now();
			runningTimer = true;
		}
		else
		{
			// 計測停止
			elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::steady_clock::now() - startTime);
			runningTimer = false;
		}
	}

	// ImGui用で開始・停止
	if (!runningTimer)
	{
		if (ImGui::Button("Timer Start"))
		{
			startTime = std::chrono::steady_clock::now();
			runningTimer = true;
		}
	}
	else
	{
		if (ImGui::Button("Timer Stop"))
		{
			elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::steady_clock::now() - startTime);
			runningTimer = false;
		}
	}

	// リセットボタン
	if (ImGui::Button("Timer Reset"))
	{
		elapsedTime = std::chrono::milliseconds(0);
		runningTimer = false;
	}

	// 現在の経過時間を計算
	auto currentElapsed = elapsedTime;
	if (runningTimer)
	{
		currentElapsed += std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::steady_clock::now() - startTime);
	}
	
	int min = static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(currentElapsed).count());
	int sec = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(currentElapsed).count() % 60);
	int cenSec = static_cast<int>((currentElapsed.count() % 1000) / 10);

	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", min, sec, cenSec);
	ImGui::SetWindowFontScale(3.0f);
	ImGui::Text("Time:%s", buffer);
	ImGui::SetWindowFontScale(1.0f);

	ImGui::End();
}