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
	printf("ƒV[ƒ“–¼FTitleScene\n");
	prototypeManager->AddPrototype("3DModel", new Model3D);

	Model3D* model[MAX_OBJECT];
	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		model[i] = static_cast<Model3D*>(CreateObj("3DModel"));
		model[i]->Init(camera);
		model[i]->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
		model[i]->SetRota(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
		model[i]->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	}
	model[0]->m_tags.AddTag("Alicia");

	for (int i = 0; i < MAX_OBJECT; ++i)
	{
		objectInstance.insert(model[i]);
	}
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