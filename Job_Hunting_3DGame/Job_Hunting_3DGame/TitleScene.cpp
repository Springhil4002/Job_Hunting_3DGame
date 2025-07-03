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
	// ãˆÚ“®
	if (input.GetKeyPress(VK_E)) camera->MoveUp(0.1f);
	// ‰ºˆÚ“®
	if (input.GetKeyPress(VK_Q)) camera->MoveDown(0.1f);

	// ‰EˆÚ“®
	if (input.GetKeyPress(VK_D)) camera->MoveRight(0.1f);
	// ¶ˆÚ“®
	if (input.GetKeyPress(VK_A)) camera->MoveLeft(0.1f);
	// ‘OˆÚ“®
	if (input.GetKeyPress(VK_W)) camera->MoveForward(0.1f);
	// ŒãˆÚ“®
	if (input.GetKeyPress(VK_S)) camera->MoveBack(0.1f);

	// ‰E•ûŒü‰ñ“]
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_D)) camera->Ratate_Yaw(XMConvertToRadians(0.1f));
	// ¶•ûŒü‰ñ“]
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_A)) camera->Ratate_Yaw(XMConvertToRadians(-0.1f));
	// ãŒü‚«‚É‰ñ“]
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_W)) camera->Ratate_Pitch(XMConvertToRadians(-0.1f));
	// ‰ºŒü‚«‚É‰ñ“]
	if (input.GetKeyPress(VK_SHIFT) && input.GetKeyPress(VK_S)) camera->Ratate_Pitch(XMConvertToRadians(0.1f));
}