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
	camera = _camera;
	hwnd = _hwnd;

	printf("シーン名：TitleScene\n");

	prototypeManager->AddPrototype("Sky", new SkyDomeMesh);
	prototypeManager->AddPrototype("Player", new Player);
	prototypeManager->AddPrototype("WaterMesh", new WaterMesh);
	
	XMVECTOR camPos = camera->GetPos();
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, camPos);
	pos.y -= 100.0f;

	SkyDomeMesh* sky = static_cast<SkyDomeMesh*>(CreateObj("Sky"));
	sky->Init(camera);
	sky->SetPos(XMLoadFloat3(&pos));
	sky->SetRota(XMVectorZero());
	sky->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	objectInstance.insert(sky);

	Player* player = static_cast<Player*>(CreateObj("Player"));
	player->Init(camera);
	player->SetPos(XMVectorSet(0.0f,1.5f,0.0f,0.0f));
	player->SetRota(XMVectorZero());
	player->SetScale(XMVectorSet(0.01f, 0.01f, 0.01f, 0.0f));
	objectInstance.insert(player);

	WaterMesh* waterMesh = static_cast<WaterMesh*>(CreateObj("WaterMesh"));
	waterMesh->Init(camera, gridX, gridZ, gridMeshSize);
	waterMesh->SetPos(XMVectorZero());
	waterMesh->SetRota(XMVectorZero());
	waterMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	waterMesh->m_tags.AddTag("WaterMesh");
	objectInstance.insert(waterMesh);

	playerCtrl = new PlayerController();
	playerCtrl->Init(player, waterMesh, camera, &BaseScene::input);
}

void TitleScene::Update(float _deltaTime)
{
	//Update_Input();
	
	playerCtrl->Update(_deltaTime);

	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
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
	ImGui::Begin("SceneName:TitleScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::Text("Press Enter to Game!");
	
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
			if (ImGui::TreeNodeEx("Position", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("X: %.3f", pos.x);	ImGui::SameLine();
				ImGui::Text("Y: %.3f", pos.y);	ImGui::SameLine();
				ImGui::Text("Z: %.3f", pos.z);
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("ForwardVec", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("X: %.3f", forward.x);	ImGui::SameLine();
				ImGui::Text("Y: %.3f", forward.y);	ImGui::SameLine();
				ImGui::Text("Z: %.3f", forward.z);
				ImGui::TreePop();
			}

			// プレイヤーの速度関係
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