#include "SceneManager.h"
#include "Debug_New.h"

std::unique_ptr<BaseScene> SceneManager::currentScene = nullptr;
SceneFactory SceneManager::sceneFactory;

void SceneManager::ChangeScene(SCENE_ID _scene_ID, Camera* _camera, HWND _hwnd)
{
	ClearConsole();
	if (currentScene)
	{
		currentScene->Uninit();
		currentScene.reset();
	}
	// 新しいシーンを作成
	currentScene.reset(sceneFactory.CreateScene(_scene_ID, _camera, _hwnd));
}

void SceneManager::ClearConsole()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD count;
	DWORD cellCount;
	COORD homeCoords = { 0,0 };

	if (hConsole == INVALID_HANDLE_VALUE) return;
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
	cellCount = csbi.dwSize.X * csbi.dwSize.Y;

	// 空白でコンソールバッファを上書き
	FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords, &count);
	// 属性情報のリセット
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
	// カーソル位置を左上に戻す
	SetConsoleCursorPosition(hConsole, homeCoords);
}

void SceneManager::Init()
{
	if (currentScene) 
		// 現在のシーンの初期化
		currentScene->Init();
}

void SceneManager::Update(float _deltaTime)
{
	if (currentScene)
	{
		// 現在のシーンの入力処理
		currentScene->Input();
		// 現在のシーンの更新処理
		currentScene->Update(_deltaTime);
	}
}

void SceneManager::Draw()
{
	if(currentScene) 
	// 現在のシーンの描画処理
	   currentScene->Draw();
}

void SceneManager::Uninit()
{
	// 現在のシーンの自動解放
	currentScene.reset();
}

void SceneManager::Draw_ImGui()
{
	if (currentScene)
	// 現在のシーンのGUI描画
		currentScene->Draw_ImGui();
}