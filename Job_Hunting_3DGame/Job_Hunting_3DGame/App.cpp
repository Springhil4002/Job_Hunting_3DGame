#include "App.h"
#include "DrawBase.h"
#include "SceneManager.h"

HINSTANCE g_hInst;
HWND g_hWnd = NULL;

LRESULT CALLBACK WndProc(HWND _hWnd, UINT _msg, WPARAM _wparam, LPARAM _lparam)
{
	switch(_msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);	// OS�ɑ΂��ďI���錾��`����
		break;
	default:
		break;
	}
	return DefWindowProc(_hWnd, _msg, _wparam, _lparam);
}

/// @brief �֐�����:�E�B���h�E�쐬�A�������ƕ`��
/// @param _appName �w�肳�ꂽ�E�B���h�E��
void InitWindow(const TCHAR* _appName)
{
	// �C���X�^���X�n���h���̎擾
	g_hInst = GetModuleHandle(nullptr);
	if (g_hInst == nullptr)
	{
		return;
	}

	// �E�B���h�E�N���X�̐ݒ�
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

	// �ݒ肵���E�B���h�E�N���X���V�X�e���ɓo�^
	RegisterClassEx(&wc);

	// �E�B���h�E�T�C�Y�̐ݒ�
	RECT rect = {};
	rect.right = static_cast<LONG>(WINDOW_WIDTH);
	rect.bottom = static_cast<LONG>(WINDOW_HEIGHT);

	// �E�B���h�E�T�C�Y�𒲐�
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rect, style, FALSE);

	// �E�B���h�E�̐���
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

	// �E�B���h�E��\��
	ShowWindow(g_hWnd, SW_SHOWNORMAL);

	// �E�B���h�E�Ƀt�H�[�J�X����
	SetFocus(g_hWnd);
}

/// @brief �A�v���P�[�V�����̃��C�����[�v�֐�
void MainLoop(const TCHAR* _appName)
{
	// ���b�Z�[�W�ێ��p�ϐ�
	MSG msg = {};
	
	// �E�B���h�E����
	InitWindow(_appName);

	Camera* camera = new Camera();
	
	// �`���Ղ̏��������s��
	g_DrawBase = new DrawBase();
	if (!g_DrawBase->Init(g_hWnd, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		return;
	}

	// �V�[���Ǘ��N���X�̐���
	auto sm = new SceneManager(camera);
	
	// ���b�Z�[�W���󂯎��܂Ń��[�v
	while (WM_QUIT != msg.message)
	{
		// ���b�Z�[�W�L���[���m�F�A����ꍇ�͎擾
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE == TRUE))
		{
			TranslateMessage(&msg);	// �L�[�{�[�h���̓��b�Z�[�W��ϊ�����
			DispatchMessage(&msg);	// ���b�Z�[�W���E�B���h�E�v���V�[�W���ɓ]������
		}
		else
		{
			// ���݂̃V�[���̍X�V����
			sm->Update();
			
			// �`��J�n����
			g_DrawBase->BeginRender();
			
			// ���݂̃V�[���̕`�揈��
			sm->Draw();
			
			// �`��I������
			g_DrawBase->EndRender();
		}
	}
}

void Application::Run(const TCHAR* _appName)
{
	// �Q�[�����[�v���s
	MainLoop(_appName);
}