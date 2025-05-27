#include "SceneManager.h"

bool SceneManager::change = false;
BaseScene* SceneManager::currentScene = nullptr;
SceneFactory SceneManager::sceneFactory;
std::set<Object*> SceneManager::createObjects;
std::set<Object*> SceneManager::deleteObjects;

void SceneManager::ChangeScene(SCENE_ID _scene_ID,Camera* _camera)
{
	currentScene = sceneFactory.CreateScene(_scene_ID, _camera);
	change = true;
}

void SceneManager::Init()
{
	if (currentScene)
	{
		currentScene->Init();
	}
}

void SceneManager::Update()
{
	if (change)
	{
		change = false;
	}

	// �I�u�W�F�N�g�̐����E����������
	Create();

	// ���͏���
	currentScene->Input();
	// �X�V����
	currentScene->Update();

	// �I�u�W�F�N�g�̍폜�E�I������
	Delete();

	if (change == true)
	{
		currentScene->Uninit();
	}
}

void SceneManager::Draw()
{
	//���݂̃V�[���̕`��
	currentScene->Draw();
}

void SceneManager::Uninit()
{
	if (currentScene != nullptr)
	{
		delete currentScene;
	}
}

void SceneManager::Create()
{
	if (!createObjects.empty())
	{
		auto buf = createObjects;
		for (auto& obj : buf)
		{
			obj->Init();
			currentScene->GetObjects()->insert(obj);
			createObjects.erase(obj);
		}
	}
}

void SceneManager::Delete()
{
	if (!deleteObjects.empty())
	{
		auto buf = deleteObjects;
		for (auto& obj : buf)
		{
			obj->Uninit();
			currentScene->GetObjects()->erase(obj);
			deleteObjects.erase(obj);
			delete obj;
		}
	}
}
