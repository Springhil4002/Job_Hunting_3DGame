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

	WaterMesh* waterMesh[MAX_OBJECT];
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		for (int j = 0; j < GRID_SIZE; ++j)
		{
			int index = i * GRID_SIZE + j;
			waterMesh[index] = static_cast<WaterMesh*>(CreateObj("WaterMesh"));
			waterMesh[index]->Init(camera);

			// 配置座標を計算
			float x = j * SPACE;
			float z = i * SPACE;

			waterMesh[index]->SetPos(XMVectorSet(x, 0.0f, z, 0.0f));
			waterMesh[index]->SetRota(XMVectorZero());
			waterMesh[index]->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
			waterMesh[index]->m_tags.AddTag("Water_MS");
		}
	}
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		objectInstance.insert(waterMesh[i]);
	}

	
	/*int test = 1;
	WaterMesh* waterMesh[1];
	for (int i = 0; i < test; ++i)
	{
		waterMesh[i] = static_cast<WaterMesh*>(CreateObj("WaterMesh"));
		waterMesh[i]->Init(camera);
		waterMesh[i]->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
		waterMesh[i]->SetRota(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
		waterMesh[i]->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	}
	waterMesh[0]->m_tags.AddTag("Water_MS");

	for (int i = 0; i < test; ++i)
	{
		objectInstance.insert(waterMesh[i]);
	}*/
}

void TitleScene::Update()
{
	if (input.GetKeyPress(VK_D))
	{
		camera->MoveRight(1.0f);
	}
	if (input.GetKeyPress(VK_A))
	{
		camera->MoveLeft(1.0f);
	}
	if (input.GetKeyPress(VK_W))
	{
		camera->MoveForward(1.0f);
	}
	if (input.GetKeyPress(VK_S))
	{
		camera->MoveBack(1.0f);
	}

	if (input.GetKeyPress(VK_E))
	{
		camera->MoveUp(1.0f);
	}
	if (input.GetKeyPress(VK_Q))
	{
		camera->MoveDown(1.0f);
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