#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "ComPtr.h"

#pragma comment(lib, "d3d12.lib")	// d3d12���C�u�����������N����
#pragma comment(lib, "dxgi.lib")	// dxgi���C�u�����������N����

class DrawBase
{
public:
	/// @brief �_�u���o�b�t�@�����O����̂�2
	enum { FRAME_BUFFER_COUNT = 2 }; 

	/// @brief �`���Ղ̏�����
	/// @param hwnd �E�B���h�E�n���h��
	/// @param windowWidth �E�B���h�E�̉���
	/// @param windowHeight �E�B���h�E�̏c��
	/// @return �������̐���
	bool Init(HWND _hwnd, UINT _windowWidth, UINT _windowHeight); 

	/// @brief �`��̊J�n����
	void BeginRender();
	/// @brief �`��̏I������
	void EndRender(); 
	
	/// @brief �O������Q�Ƃł���悤�ɃQ�b�^�[�Ƃ��Č��J����
	/// @return ID3D12Device�̃o�[�W����6�̃|�C���^
	ID3D12Device6* Device();
	/// @brief �O������Q�Ƃł���悤�ɃQ�b�^�[�Ƃ��Č��J����
	/// @return ID3D12GraphicsCommandList�̃|�C���^
	ID3D12GraphicsCommandList* CommandList();
	/// @brief �O������Q�Ƃł���悤�ɃQ�b�^�[�Ƃ��Č��J����
	/// @return UINT(unsigned int�^)
	UINT CurrentBackBufferIndex();

private: 
	//==================================================================
	// DirectX12�������Ɏg���֐��Q
	//==================================================================

	/// @brief �f�o�C�X����
	/// @return �����̐���
	bool CreateDevice();	
	/// @brief �R�}���h�L���[����
	/// @return �����̐���
	bool CreateCommandQueue();	
	/// @brief �X���b�v�`�F�C������
	/// @return �����̐���
	bool CreateSwapChain();		
	/// @brief �R�}���h���X�g�ƃR�}���h�A���P�[�^�[����
	/// @return �����̐���
	bool CreateCommandList();	
	/// @brief �t�F���X����
	/// @return �����̐���
	bool CreateFence();			
	/// @brief �r���[�|�[�g����
	void CreateViewPort();		
	/// @brief �V�U�[��`����
	void CreateScissorRect();	

	//==================================================================
	// �`��Ɏg��DirectX12�̃I�u�W�F�N�g�Q
	//==================================================================
	
	// �E�B���h�E�n���h��
	HWND m_hWnd;
	// �t���[���o�b�t�@�̉���
	UINT m_FrameBufferWidth = 0;
	// �t���[���o�b�t�@�̏c��
	UINT m_FrameBufferHeight = 0;
	// ���݂̃o�b�N�o�b�t�@�̔ԍ��ێ��ϐ�
	UINT m_CurrentBackBufferIndex = 0;

	// �f�o�C�X
	ComPtr<ID3D12Device6> m_pDevice = nullptr;		
	// �R�}���h�L���[
	ComPtr<ID3D12CommandQueue> m_pQueue = nullptr;	
	// �X���b�v�`�F�C��
	ComPtr<IDXGISwapChain3> m_pSwapChain = nullptr; 
	// �R�}���h�A���P�[�^�[
	ComPtr<ID3D12CommandAllocator> m_pAllocator[FRAME_BUFFER_COUNT] = { nullptr };
	// �R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList = nullptr; 
	// �t�F���X�Ŏg���C�x���g
	HANDLE m_fenceEvent = nullptr;				
	// �t�F���X
	ComPtr<ID3D12Fence> m_pFence = nullptr;		
	// �t�F���X�̒l�ienum�Őݒ肵����:2�j
	UINT64 m_fenceValue[FRAME_BUFFER_COUNT];	
	// �r���[�|�[�g
	D3D12_VIEWPORT m_Viewport; 
	// �V�U�[��`
	D3D12_RECT m_Scissor;

	//==================================================================
	// �`��Ɏg���I�u�W�F�N�g�Ƃ��̐����֐�����
	//==================================================================
	
	/// @brief �����_�[�^�[�Q�b�g�𐶐�
	/// @return �����̐���
	bool CreateRenderTarget(); 
	/// @brief �[�x�X�e���V���o�b�t�@�𐶐�
	/// @return �����̐���
	bool CreateDepthStencil(); 

	// �����_�[�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�T�C�Y
	UINT m_RtvDescriptorSize = 0; 
	// �����_�[�^�[�Q�b�g�̃f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap = nullptr; 
	// �����_�[�^�[�Q�b�g�i�_�u���o�b�t�@�p��2�j
	ComPtr<ID3D12Resource> m_pRenderTargets[FRAME_BUFFER_COUNT] = { nullptr }; 

	// �[�x�X�e���V���̃f�B�X�N���v�^�[�T�C�Y
	UINT m_DsvDescriptorSize = 0; 
	// �[�x�X�e���V���̃f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap = nullptr; 
	// �[�x�X�e���V���o�b�t�@
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer = nullptr; 

	//==================================================================
	// �`�惋�[�v�Ŏg�p�������
	//==================================================================
	
	// ���݂̃t���[���̃����_�[�^�[�Q�b�g���ꎞ�I�ɕۑ����Ă����֐�
	ID3D12Resource* m_currentRenderTarget = nullptr; 

	/// @brief �`�抮����҂���
	void WaitRender(); 
};

extern DrawBase* g_DrawBase; // �ǂ�����ł��Q�Ƃ������̂ŃO���[�o���ɂ���