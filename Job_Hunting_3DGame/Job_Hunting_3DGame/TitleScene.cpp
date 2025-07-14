#include "TitleScene.h"
#include "SceneManager.h"

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

	WaterMesh* waterMesh = static_cast<WaterMesh*>(CreateObj("WaterMesh"));
	waterMesh->Init(camera, gridX, gridZ, gridMeshSize);
	waterMesh->SetPos(XMVectorZero());
	waterMesh->SetRota(XMVectorZero());
	waterMesh->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	waterMesh->m_tags.AddTag("WaterMesh");
	objectInstance.insert(waterMesh);
}

void TitleScene::Update()
{
	Update_Input();
	
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