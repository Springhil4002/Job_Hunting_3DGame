#include "App.h"
#include "Timer.h"
#include "DrawBase.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include "System/ImGui/imgui_impl_win32.h"
#include "Debug_New.h"

HINSTANCE g_hInst;
HWND g_hWnd = NULL;
ImGuiManager g_ImGuiManager;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK WndProc(HWND _hWnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(_hWnd, _msg, _wparam, _lparam))
	{
		return true;
	}
	switch(_msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);	// OSに対して終了宣言を伝える
		break;
	default:
		break;
	}
	return DefWindowProc(_hWnd, _msg, _wparam, _lparam);
}

/// @brief 関数説明:ウィンドウ作成、初期化と描画
/// @param _appName 指定されたウィンドウ名
void InitWindow(const TCHAR* _appName)
{
	// インスタンスハンドルの取得
	g_hInst = GetModuleHandle(nullptr);
	if (g_hInst == nullptr)
	{
		return;
	}

	// ウィンドウクラスの設定
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hIcon = LoadIcon(g_hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(g_hInst, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = _appName;
	wc.hIconSm = LoadIcon(g_hInst, IDI_APPLICATION);

	// 設定したウィンドウクラスをシステムに登録
	RegisterClassEx(&wc);

	// ウィンドウサイズの設定
	RECT rect = {};
	rect.right = static_cast<LONG>(WINDOW_WIDTH);
	rect.bottom = static_cast<LONG>(WINDOW_HEIGHT);

	// ウィンドウサイズを調整
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	// ウィンドウの生成
	g_hWnd = CreateWindowEx(
		0,
		_appName,
		_appName,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		g_hInst,
		nullptr
	);

	// ウィンドウを表示
	ShowWindow(g_hWnd, SW_SHOWNORMAL);

	// ウィンドウにフォーカスする
	SetFocus(g_hWnd);
}

/// @brief アプリケーションのメインループ関数
void MainLoop(const TCHAR* _appName)
{
	// メッセージ保持用変数
	MSG msg = {};
	
	// ウィンドウ生成
	InitWindow(_appName);

	Camera* camera = new Camera();
	
	// 描画基盤の初期化を行う
	g_DrawBase = new DrawBase();
	if (!g_DrawBase->Init(g_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		return;
	}

	// ImGuiの初期化
	g_ImGuiManager.Init(g_hWnd, g_DrawBase);
	
	// シーン管理クラスの生成
	auto sm = new SceneManager(camera,g_hWnd);
	
	// 経過時間計測処理の初期化
	Timer* timer = new Timer();
	timer->Init();

	// メッセージを受け取るまでループ
	while (WM_QUIT != msg.message)
	{
		// メッセージキューを確認、ある場合は取得
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	// キーボード入力メッセージを変換する
			DispatchMessage(&msg);	// メッセージをウィンドウプロシージャに転送する
		}
		else
		{
			// 毎フレーム経過時間を更新
			timer->Update();
			float deltaTime = timer->GetDeltaTime();

			// 現在のシーンの更新処理
			sm->Update(deltaTime);
			
			g_DrawBase->BeginRender();	// 描画開始処理
			g_ImGuiManager.Begin();		// ImGuiの開始処理
			sm->Draw_ImGui();			// 現在のシーンのImGui描画処理
			sm->Draw();					// 現在のシーンの描画処理
			g_ImGuiManager.End();		// ImGuiの終了処理
			g_DrawBase->EndRender();	// 描画終了処理
		}
	}
	sm->Uninit();
	delete sm;
	delete timer;
	delete camera;
	delete g_DrawBase;
	g_DrawBase = nullptr;
}

void Application::Run(const TCHAR* _appName)
{
	// ゲームループ実行
	MainLoop(_appName);
	// ImGuiのシャットダウン
	g_ImGuiManager.ShutDown();
}