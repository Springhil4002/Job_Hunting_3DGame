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
	camera = _camera;
	uiCamera = _uiCamera;
	hwnd = _hwnd;

	camera->SetPos(XMVectorSet(0.0f, 2.5f, 0.0f, 1.0f));
	camera->SetTarget(XMVectorSet(0.0f, -15.0f, 50.0f, 0.0f));
	camera->SetUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	uiCamera->Init(screenWidth, screenHeight);

	printf("シーン名：GameScene\n");
}

void GameScene::Update(float _deltaTime)
{
	//Update_Input();

	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
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
}

void GameScene::Uninit()
{
	for (auto& obj : objectInstance)
	{
		obj->Uninit();
	}
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
	ImGui::Begin("SceneName:GameScene");
	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
	ImGui::Text("Press Enter to Result!");
	ImGui::End();
}