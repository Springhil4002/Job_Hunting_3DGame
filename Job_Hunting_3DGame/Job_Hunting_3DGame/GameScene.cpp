#include "GameScene.h"
#include "SceneManager.h"

using namespace DirectX;

Object* GameScene::CreateObj(const std::string& _objectID)
{
	return prototypeManager->Create(_objectID);
}

void GameScene::Init(Camera* _camera,HWND _hwnd)
{
	camera = _camera;
	hwnd = _hwnd;
	printf("ƒV[ƒ“–¼FGameScene\n");

	prototypeManager->AddPrototype("3DModel", new Model3D);

	Model3D* model = static_cast<Model3D*>(CreateObj("3DModel"));
	model->Init(camera);
	model->SetPos(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	model->SetRota(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	model->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	model->m_tags.AddTag("Alicia");
	objectInstance.insert(model);
}

void GameScene::Update()
{
	auto allModel = GetInstance()->GetObjects<Model3D>();
	for (auto* model : allModel)
	{
		if (model->m_tags.SearchTag("Alicia"))
		{
			if (input.GetKeyPress(VK_D))
			{
				XMVECTOR pos = model->GetPos();
				pos = XMVectorSetX(pos, XMVectorGetX(pos) + 1.0f);
				model->SetPos(pos);
			}
			if (input.GetKeyPress(VK_A))
			{
				XMVECTOR pos = model->GetPos();
				pos = XMVectorSetX(pos, XMVectorGetX(pos) - 1.0f);
				model->SetPos(pos);
			}
			if (input.GetKeyPress(VK_W))
			{
				XMVECTOR pos = model->GetPos();
				pos = XMVectorSetZ(pos, XMVectorGetZ(pos) + 1.0f);
				model->SetPos(pos);
			}
			if (input.GetKeyPress(VK_S))
			{
				XMVECTOR pos = model->GetPos();
				pos = XMVectorSetZ(pos, XMVectorGetZ(pos) - 1.0f);
				model->SetPos(pos);
			}
		}
	}

	for (auto& obj : objectInstance)
	{
		obj->Update();
	}

	if (input.GetKeyTrigger(VK_RETURN))
	{
		SceneManager::ChangeScene(SCENE_ID_RESULT, camera, hwnd);
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