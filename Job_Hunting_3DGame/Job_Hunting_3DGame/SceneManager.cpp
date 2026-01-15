#include "SceneManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Debug_New.h"

std::unique_ptr<BaseScene> SceneManager::currentScene = nullptr;
SceneFactory SceneManager::sceneFactory;
SoundManager SceneManager::soundManager;

void SceneManager::ChangeScene(
	SCENE_ID _scene_ID, Camera* _camera, Camera2D* _uiCamera, HWND _hwnd)
{
	ClearConsole();
	if (currentScene)
	{
		currentScene->Uninit();
		currentScene.reset();
	}

	// 次に使うリソース一覧を取得
	auto resList = GetSceneResourceList(_scene_ID);

	// テクスチャをまとめてロード
	for (auto& tex : resList.textures)
	{
		bool success = false;
		switch (tex.type)
		{
		case TEX_TYPE::TEX_TYPE_TEXTURE2D:
		{
			success = (TextureManager::Instance().LoadTexture(tex.path) != nullptr);
			break;
		}
		case TEX_TYPE::TEX_TYPE_CUBEMAP:
		{
			success = (TextureManager::Instance().LoadCubeMap(tex.path) != nullptr);
			break;
		}
		default:
			printf("SceneManager:未知のTEX_TYPE検出 %d\n", tex.type);
			break;
		}
		if (!success)
			printf("SceneManager:リソース読み込み失敗 %ls\n", tex.path.c_str());
	}

	// モデルをまとめてロード
	for (auto& path : resList.modelPaths)
	{
		auto model = ModelManager::GetInstance().LoadModel(path);
		if (!model)
		{
			printf("SceneManager:モデル読み込み失敗 %ls\n", path.c_str());
		}
	}
	printf("SceneManager:リソース読み込み完了\n\n");

	// 新しいシーンを作成
	currentScene.reset(sceneFactory.CreateScene(
		_scene_ID, _camera, _uiCamera,_hwnd));
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