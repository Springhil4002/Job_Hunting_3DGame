#include "TitleScene.h"
#include "SceneManager.h"

using namespace DirectX;

Object* TitleScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void TitleScene::Init(Camera* _camera)
{
	camera = _camera;
	printf("シーン名：TitleScene\n");

	prototypeManager->AddPrototype("Sky", new SkyDomeMesh);
	prototypeManager->AddPrototype("WaterMesh", new WaterMesh);
	
	XMVECTOR camPos = camera->GetPos();
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, camPos);
	pos.y -= 30.0f;

	SkyDomeMesh* sky;
	sky = static_cast<SkyDomeMesh*>(CreateObj("Sky"));
	sky->Init(camera);
	sky->SetPos(XMLoadFloat3(&pos));
	sky->SetRota(XMVectorZero());
	sky->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	objectInstance.insert(sky);

	WaterMesh* waterMesh[MAX_OBJECT];
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		for (int j = 0; j < GRID_SIZE; ++j)
		{
			int index = i * GRID_SIZE + j;
			waterMesh[index] = static_cast<WaterMesh*>(CreateObj("WaterMesh"));
			waterMesh[index]->Init(camera, j, i, GRID_SIZE);

			// 原点を中心とした配置座標を計算
			float x = j * SPACE - OFFSET;
			float z = i * SPACE - OFFSET;

			waterMesh[index]->SetPos(XMVectorSet(x, 0.0f, z, 0.0f));
			waterMesh[index]->SetRota(XMVectorZero());
			waterMesh[index]->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
			waterMesh[index]->m_tags.AddTag("Water_MS");
			objectInstance.insert(waterMesh[index]);
		}
	}
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
		SceneManager::ChangeScene(SCENE_ID_GAME, camera);
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
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_D)) camera->Ratate_Yaw(XMConvertToRadians(0.1f));
	// 左方向回転
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_A)) camera->Ratate_Yaw(XMConvertToRadians(-0.1f));
	// 上向きに回転
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_W)) camera->Ratate_Pitch(XMConvertToRadians(-0.1f));
	// 下向きに回転
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_S)) camera->Ratate_Pitch(XMConvertToRadians(0.1f));
}