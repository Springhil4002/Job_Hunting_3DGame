#include "DrawBase.h"
#include <d3D12.h>
#include <stdio.h>
#include <Windows.h>

DrawBase* g_DrawBase;

bool DrawBase::Init(HWND _hwnd, UINT _windowWidth, UINT _windowHeight)
{
	m_FrameBufferWidth = _windowWidth;
	m_FrameBufferHeight = _windowHeight;
	m_hWnd = _hwnd;
	
	if (CreateDevice() == false)
	{
		printf("�f�o�C�X�̐����Ɏ��s\n");
		return false;
	}
	if (CreateCommandQueue() == false)
	{
		printf("�R�}���h�L���[�̐����Ɏ��s\n");
		return false;
	}
	
	if (CreateSwapChain() == false)
	{
		printf("�X���b�v�`�F�[���̐����Ɏ��s\n");
		return false;
	}

	if (CreateCommandList() == false)
	{
		printf("�R�}���h���X�g�̐����Ɏ��s\n");
		return false;
	}

	if (CreateFence() == false)
	{
		printf("�t�F���X�̐����Ɏ��s\n");
		return false;
	}

	// �r���[�|�[�g�ƃV�U�[��`�𐶐�
	CreateViewPort();
	CreateScissorRect();

	if (CreateRenderTarget() == false)
	{
		printf("�����_�[�^�[�Q�b�g�̐����Ɏ��s\n");
		return false;
	}

	printf("�`���Ղ̏���������\n");
	return true;
}

// �f�o�C�X:GPU�̃f�o�C�X�̃C���^�[�t�F�C�X
bool DrawBase::CreateDevice()
{
	auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf()));

	// ���C�g���[�V���O�Ή����Ă��邩�̊m�F
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 features5;
	hr = m_pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &features5, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));
	if (FAILED(hr) || features5.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		int a;
	}

	return SUCCEEDED(hr);
}

// �R�}���h�L���[:�f�o�C�X�ɕ`��R�}���h�̓��e�A�`��R�}���h���s�̓����������s������
bool DrawBase::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	auto hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.ReleaseAndGetAddressOf()));

	return SUCCEEDED(hr);
}

// �X���b�v�`�F�[��:�_�u���o�b�t�@�E�g���v���o�b�t�@���������邽�߂̂���
bool DrawBase::CreateSwapChain()
{
	// DXGI�t�@�N�g���[�̐���
	IDXGIFactory4* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	if (FAILED(hr))
	{
		return false;
	}

	// �X���b�v�`�F�C���̏�����
	DXGI_SWAP_CHAIN_DESC desc = {};
	// �o�b�t�@�̉���
	desc.BufferDesc.Width = m_FrameBufferWidth;	
	// �o�b�t�@�̏c��
	desc.BufferDesc.Height = m_FrameBufferHeight;
	// �o�b�t�@�̃��t���b�V�����[�g�̕��q
	desc.BufferDesc.RefreshRate.Numerator = 60;
	// �o�b�t�@�̃��t���b�V�����[�g�̕���
	desc.BufferDesc.RefreshRate.Denominator = 1;
	// �o�b�t�@�̃X�L�������C���̏���(�f�t�H���g���)
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// �o�b�t�@�̃X�P�[�����O�����ݒ�(�f�t�H���g�ݒ�)
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// �`��o�b�t�@�̃s�N�Z���t�H�[�}�b�g�w��(�ԁE�΁E�E�A���t�@�����ꂼ��8�r�b�g)
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �}���`�T���v�����O(�A���`�G�C���A�X����)�̐��̐ݒ�(1�Ȃ�ł��Ȃ�)
	desc.SampleDesc.Count = 1;
	// �T���v���̕i���ݒ�(0�Ȃ̂Ńf�t�H���g�l�̂܂܎g�p)
	desc.SampleDesc.Quality = 0;
	// �����_�[�^�[�Q�b�g�̎g�p�w��(�`�挋�ʂ���ʂɏo�͂��邽�߂̗p�r)
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// �X���b�v�`�F�[���̃o�b�t�@�̐��̐ݒ�
	desc.BufferCount = FRAME_BUFFER_COUNT;
	// �o�͐�̃n���h���ݒ�
	desc.OutputWindow = m_hWnd;
	// �E�B���h�E���[�h�ł̕`��w��
	desc.Windowed = TRUE;
	// �X���b�v�`�F�[���̌������@(DISCARD:�Â��o�b�t�@��j�����āA�V�����o�b�t�@���g�p�����������)
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// �t���X�N���[�����[�h�ƃE�B���h�E���[�h�̐؂�ւ���������t���O(�t���X�N���[���ɐ؂�ւ���ۂɃT�|�[�g����ݒ�)
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// �X���b�v�`�F�C���̐���
	IDXGISwapChain* pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);
	if (FAILED(hr))
	{
		pFactory->Release();
		return false;
	}

	// IDXGISwapChain3���擾
	hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		pFactory->Release();
		pSwapChain->Release();
		return false;
	}

	// �o�b�N�o�b�t�@�ԍ����擾
	m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	pFactory->Release();
	pSwapChain->Release();
	return true;
}

// �R�}���h���X�g:�`�施�߂��L�^���A���߂Ă��������ɂ݂����Ȃ���(���_�㖳�������������͈͓̔��܂�)
// �R�}���h�A���P�[�^�[:�R�}���h���X�g�̎g�p���郁�����̊Ǘ��A���������蓖�Ă�����@�\
bool DrawBase::CreateCommandList()
{
	// �R�}���h�A���P�[�^�[�̍쐬
	HRESULT hr;
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		hr = m_pDevice->CreateCommandAllocator(
			// GPU�����ڎ��s����^�C�v�̃R�}���h(�O���t�B�b�N��`�施�߂Ȃ�)���������Ƃ��w��
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			// COM�|�C���^�̃������Ǘ����s���A�V�����R�}���h�A���P�[�^�[�̃��������擾
			IID_PPV_ARGS(m_pAllocator[i].ReleaseAndGetAddressOf()));
	}

	if (FAILED(hr))
	{
		return false;
	}

	// �R�}���h���X�g�̐���
	hr = m_pDevice->CreateCommandList(
		// �m�[�h�}�X�N(�}���`GPU���Ŏg�p����)
		0,	
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		// ���݂̃o�b�N�o�b�t�@�p�̃R�}���h�A���P�[�^�[���֘A�Â���
		m_pAllocator[m_CurrentBackBufferIndex].Get(),
		// �����̃p�C�v���C���X�e�[�g���w�肵�Ȃ�
		nullptr,
		IID_PPV_ARGS(&m_pCommandList)
	);

	if (FAILED(hr))
	{
		return false;
	}

	// �R�}���h���X�g�͍쐬����͊J���Ă�̂ŁA�����������B
	m_pCommandList->Close();

	return true;
}

// �t�F���X:CPU��GPU�̓������s�����߂̂��́A�`�抮���̔��f���t�F���X�̒l���C���N�������g���ꂽ���Ŕ��f����
bool DrawBase::CreateFence()
{
	// �t�F���X�l�̏�����
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; i++)
	{
		m_fenceValue[i] = 0;
	}

	// �t�F���X�̍쐬
	auto hr = m_pDevice->CreateFence(
		0,						// �����l�Ƃ���0�ɐݒ�
		D3D12_FENCE_FLAG_NONE,	// �f�t�H���g�̓���ݒ� 
		IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()
		));
	if (FAILED(hr))
	{
		return false;
	}

	//���݂̃t���[���̃t�F���X�l���X�V 
	m_fenceValue[m_CurrentBackBufferIndex]++;

	// �������s���Ƃ��̃C�x���g�n���h�����쐬����B
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	return m_fenceEvent != nullptr;
}

// �r���[�|�[�g:�E�B���h�E�Ƀ����_�����O���ʂ��ǂ��\�����邩�̐ݒ�
void DrawBase::CreateViewPort()
{
	// �r���[�|�[�g�̍��オ��ʂ̍���ɔz�u�����悤�ݒ�
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;

	// �r���[�|�[�g����ʑS�̂𕢂��悤�ɐݒ�
	m_Viewport.Width = static_cast<float>(m_FrameBufferWidth);
	m_Viewport.Height = static_cast<float>(m_FrameBufferHeight);
	
	// �[�x�o�b�t�@�͈̔͂�ݒ�
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
}

// �V�U�[��`:�r���[�|�[�g�ɕ\�����ꂽ�摜�̂ǂ��܂ł���ʂɉf���o�����̐ݒ�
// ��:�}���I�J�[�g�̕����l�v���C���̉�ʕ`��Ƃ�
void DrawBase::CreateScissorRect()
{
	// �`��͈͂̍��[
	m_Scissor.left = 0;
	// ��ʕ`��̉E�[
	m_Scissor.right = m_FrameBufferWidth;
	// ��ʕ`��̏�[
	m_Scissor.top = 0;
	// ��ʕ`��̉��[
	m_Scissor.bottom = m_FrameBufferHeight;
}

// �����_�[�^�[�Q�b�g:�L�����o�X�̂悤�Ȃ��́A�`���B���ԂƂ��Ă̓o�b�N�o�b�t�@��e�N�X�`���Ȃǂ̃��\�[�X
bool DrawBase::CreateRenderTarget()
{
	// �����_�[�^�[�Q�b�g�r���[(RTV)�p�̃f�B�X�N���v�^�q�[�v���쐬����
	// RTV:�����_�[�^�[�Q�b�g���\�[�X�ɃA�N�Z�X���邽�߂̑����A���n���̂悤�Ȃ���
	// �f�B�X�N���v�^�q�[�v:�����_�[�^�[�Q�b�g���\�[�X�Ȃǂ̕����̃f�B�X�N���v�^�������I�ɊǗ�����̈�
	// �f�B�X�N���v�^:���\�[�X(�e�N�X�`���A�����_�[�^�[�Q�b�g�A�[�x�o�b�t�@�Ȃ�)���Q�Ƃ���n���h��
	
	// �f�B�X�N���v�^�q�[�v�̏�����
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	// �q�[�v�Ɋ܂܂��f�B�X�N���v�^�̐����w��(�t���[�����ɕK�v�ȃf�B�X�N���v�^���m��)
	desc.NumDescriptors = FRAME_BUFFER_COUNT;
	// �q�[�v�̎�ނ��w��(RTV�p�̃f�B�X�N���v�^�q�[�v)
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	// �q�[�v�̃t���O���w��(���ʂȏ��������Ȃ��W���I�Ȑݒ�)
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	// �f�B�X�N���v�^�q�[�v���쐬
	auto hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pRtvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// �f�B�X�N���v�^�̃T�C�Y���擾
	m_RtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// RTV�q�[�v�̊J�n�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();

	// �o�b�N�o�b�t�@���Ƃ̃����_�[�^�[�Q�b�g�r���[�쐬
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		// �o�b�N�o�b�t�@�̎擾
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pRenderTargets[i].ReleaseAndGetAddressOf()));
		// �����_�[�^�[�Q�b�g�r���[�̍쐬
		m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);
		// �n���h���̃C���N�������g(���̃o�b�N�o�b�t�@�ɑΉ�����RTV���쐬���邽�߂ɍX�V)
		rtvHandle.ptr += m_RtvDescriptorSize;
	}

	return true;
}