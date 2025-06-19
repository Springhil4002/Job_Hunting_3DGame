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
	
	prototypeManager->AddPrototype("WaterMesh", new WaterMesh);
	//prototypeManager->AddPrototype("Player", new Player);

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
	if (input.GetKeyPress(VK_D))
	{
		// 右方向に移動
		camera->MoveRight(1.0f);
	}
	if (input.GetKeyPress(VK_A))
	{
		// 左方向に移動
		camera->MoveLeft(1.0f);
	}
	if (input.GetKeyPress(VK_W))
	{
		// 前方向に移動
		camera->MoveForward(1.0f);
	}
	if (input.GetKeyPress(VK_S))
	{
		// 後ろ方向に移動
		camera->MoveBack(1.0f);
	}

	if (input.GetKeyPress(VK_E))
	{
		// 上方向に移動
		camera->MoveUp(1.0f);
	}
	if (input.GetKeyPress(VK_Q))
	{
		// 下方向に移動
		camera->MoveDown(1.0f);
	}
	
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_A))
	{
		// 左に10度回転
		camera->Ratate_Yaw(XMConvertToRadians(1.0f));	
	}

	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_D))
	{
		// 右に10度回転
		camera->Ratate_Yaw(XMConvertToRadians(-1.0f));
	}
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_W))
	{
		// 上に10度回転
		camera->Ratate_Pitch(XMConvertToRadians(1.0f));
	}
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_S))
	{
		// 下に10度回転
		camera->Ratate_Pitch(XMConvertToRadians(-1.0f));
	}
	
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