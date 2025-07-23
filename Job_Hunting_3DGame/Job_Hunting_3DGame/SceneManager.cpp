#include "SceneManager.h"

bool SceneManager::change = false;
BaseScene* SceneManager::currentScene = nullptr;
SceneFactory SceneManager::sceneFactory;
std::set<Object*> SceneManager::createObjects;
std::set<Object*> SceneManager::deleteObjects;

void SceneManager::ChangeScene(SCENE_ID _scene_ID,Camera* _camera,HWND _hwnd)
{
	ClearConsole();
	currentScene = sceneFactory.CreateScene(_scene_ID, _camera, _hwnd);
	change = true;
}

void SceneManager::ClearConsole()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = { 0,0 };

	if (hConsole == INVALID_HANDLE_VALUE) return;

	// 現在のコンソールのバッファ情報を取得
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	// 空白でコンソールバッファを上書き
	if ((!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count))) return;

	// 属性情報のリセット
	if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count)) return;

	// カーソル位置を左上に戻す
	SetConsoleCursorPosition(hConsole, homeCoords);
}

void SceneManager::Init()
{
	if (currentScene)
	{
		currentScene->Init();
	}
}

void SceneManager::Update(float _deltaTime)
{
	if (change)
	{
		change = false;
	}

	// オブジェクトの生成・初期化処理
	Create();

	// 入力処理
	currentScene->Input();
	// 更新処理
	currentScene->Update(_deltaTime);

	// オブジェクトの削除・終了処理
	Delete();

	if (change == true)
	{
		currentScene->Uninit();
	}
}

void SceneManager::Draw()
{
	//現在のシーンの描画
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

void SceneManager::Draw_ImGui()
{
	if (currentScene)
	{
		currentScene->Draw_ImGui();
	}
}