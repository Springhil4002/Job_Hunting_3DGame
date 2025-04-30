#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "ComPtr.h"

#pragma comment(lib, "d3d12.lib")	// d3d12ライブラリをリンクする
#pragma comment(lib, "dxgi.lib")	// dxgiライブラリをリンクする

class DrawBase
{
public:
	/// @brief ダブルバッファリングするので2
	enum { FRAME_BUFFER_COUNT = 2 }; 

	/// @brief 描画基盤の初期化
	/// @param hwnd ウィンドウハンドル
	/// @param windowWidth ウィンドウの横幅
	/// @param windowHeight ウィンドウの縦幅
	/// @return 初期化の成否
	bool Init(HWND _hwnd, UINT _windowWidth, UINT _windowHeight); 

	/// @brief 描画の開始処理
	void BeginRender();
	/// @brief 描画の終了処理
	void EndRender(); 
	
	/// @brief 外部から参照できるようにゲッターとして公開する
	/// @return ID3D12Deviceのバージョン6のポインタ
	ID3D12Device6* Device();
	/// @brief 外部から参照できるようにゲッターとして公開する
	/// @return ID3D12GraphicsCommandListのポインタ
	ID3D12GraphicsCommandList* CommandList();
	/// @brief 外部から参照できるようにゲッターとして公開する
	/// @return UINT(unsigned int型)
	UINT CurrentBackBufferIndex();

private: 
	//==================================================================
	// DirectX12初期化に使う関数群
	//==================================================================

	/// @brief デバイス生成
	/// @return 生成の成否
	bool CreateDevice();	
	/// @brief コマンドキュー生成
	/// @return 生成の成否
	bool CreateCommandQueue();	
	/// @brief スワップチェイン生成
	/// @return 生成の成否
	bool CreateSwapChain();		
	/// @brief コマンドリストとコマンドアロケーター生成
	/// @return 生成の成否
	bool CreateCommandList();	
	/// @brief フェンス生成
	/// @return 生成の成否
	bool CreateFence();			
	/// @brief ビューポート生成
	void CreateViewPort();		
	/// @brief シザー矩形生成
	void CreateScissorRect();	

	//==================================================================
	// 描画に使うDirectX12のオブジェクト群
	//==================================================================
	
	// ウィンドウハンドル
	HWND m_hWnd;
	// フレームバッファの横幅
	UINT m_FrameBufferWidth = 0;
	// フレームバッファの縦幅
	UINT m_FrameBufferHeight = 0;
	// 現在のバックバッファの番号保持変数
	UINT m_CurrentBackBufferIndex = 0;

	// デバイス
	ComPtr<ID3D12Device6> m_pDevice = nullptr;		
	// コマンドキュー
	ComPtr<ID3D12CommandQueue> m_pQueue = nullptr;	
	// スワップチェイン
	ComPtr<IDXGISwapChain3> m_pSwapChain = nullptr; 
	// コマンドアロケーター
	ComPtr<ID3D12CommandAllocator> m_pAllocator[FRAME_BUFFER_COUNT] = { nullptr };
	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList = nullptr; 
	// フェンスで使うイベント
	HANDLE m_fenceEvent = nullptr;				
	// フェンス
	ComPtr<ID3D12Fence> m_pFence = nullptr;		
	// フェンスの値（enumで設定した数:2）
	UINT64 m_fenceValue[FRAME_BUFFER_COUNT];	
	// ビューポート
	D3D12_VIEWPORT m_Viewport; 
	// シザー矩形
	D3D12_RECT m_Scissor;

	//==================================================================
	// 描画に使うオブジェクトとその生成関数たち
	//==================================================================
	
	/// @brief レンダーターゲットを生成
	/// @return 生成の成否
	bool CreateRenderTarget(); 
	/// @brief 深度ステンシルバッファを生成
	/// @return 生成の成否
	bool CreateDepthStencil(); 

	// レンダーターゲットビューのディスクリプタサイズ
	UINT m_RtvDescriptorSize = 0; 
	// レンダーターゲットのディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap = nullptr; 
	// レンダーターゲット（ダブルバッファ用で2個）
	ComPtr<ID3D12Resource> m_pRenderTargets[FRAME_BUFFER_COUNT] = { nullptr }; 

	// 深度ステンシルのディスクリプターサイズ
	UINT m_DsvDescriptorSize = 0; 
	// 深度ステンシルのディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap = nullptr; 
	// 深度ステンシルバッファ
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer = nullptr; 

	//==================================================================
	// 描画ループで使用するもの
	//==================================================================
	
	// 現在のフレームのレンダーターゲットを一時的に保存しておく関数
	ID3D12Resource* m_currentRenderTarget = nullptr; 

	/// @brief 描画完了を待つ処理
	void WaitRender(); 
};

extern DrawBase* g_DrawBase; // どこからでも参照したいのでグローバルにする