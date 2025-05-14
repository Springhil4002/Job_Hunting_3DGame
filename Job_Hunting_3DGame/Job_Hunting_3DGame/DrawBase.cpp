#include "DrawBase.h"
#include <d3D12.h>
#include <stdio.h>
#include <Windows.h>
#include <DirectXTex.h>
#include <d3dx12.h>

//-------------------------------------------------------------------
// 自分が忘れないようすぐ見れるようにコメントを細かく書いてます
//-------------------------------------------------------------------

DrawBase* g_DrawBase;

bool DrawBase::Init(HWND _hwnd, UINT _windowWidth, UINT _windowHeight)
{
	m_FrameBufferWidth = _windowWidth;
	m_FrameBufferHeight = _windowHeight;
	m_hWnd = _hwnd;
	
	if (CreateDevice() == false)
	{
		printf("デバイスの生成に失敗\n");
		return false;
	}
	if (CreateCommandQueue() == false)
	{
		printf("コマンドキューの生成に失敗\n");
		return false;
	}
	
	if (CreateSwapChain() == false)
	{
		printf("スワップチェーンの生成に失敗\n");
		return false;
	}

	if (CreateCommandList() == false)
	{
		printf("コマンドリストの生成に失敗\n");
		return false;
	}

	if (CreateFence() == false)
	{
		printf("フェンスの生成に失敗\n");
		return false;
	}

	// ビューポートとシザー矩形を生成
	CreateViewPort();
	CreateScissorRect();

	if (CreateRenderTarget() == false)
	{
		printf("レンダーターゲットの生成に失敗\n");
		return false;
	}

	if (CreateDepthStencil() == false)
	{
		printf("深度ステンシルバッファの生成に失敗\n");
		return false;
	}

	printf("描画基盤の初期化成功\n");
	return true;
}

// デバイス:GPUのデバイスのインターフェイス
bool DrawBase::CreateDevice()
{
	auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_pDevice.ReleaseAndGetAddressOf()));

	// レイトレーシング対応しているかの確認
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 features5;
	hr = m_pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &features5, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));
	if (FAILED(hr) || features5.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
	{
		int a;
	}

	return SUCCEEDED(hr);
}

// コマンドキュー:デバイスに描画コマンドの投稿、描画コマンド実行の同期処理を行うもの
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

// スワップチェーン:ダブルバッファ・トリプルバッファを実現するためのもの
bool DrawBase::CreateSwapChain()
{
	// DXGIファクトリーの生成
	IDXGIFactory4* pFactory = nullptr;
	HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	if (FAILED(hr))
	{
		return false;
	}

	// スワップチェインの初期化
	DXGI_SWAP_CHAIN_DESC desc = {};
	// バッファの横幅
	desc.BufferDesc.Width = m_FrameBufferWidth;	
	// バッファの縦幅
	desc.BufferDesc.Height = m_FrameBufferHeight;
	// バッファのリフレッシュレートの分子
	desc.BufferDesc.RefreshRate.Numerator = 60;
	// バッファのリフレッシュレートの分母
	desc.BufferDesc.RefreshRate.Denominator = 1;
	// バッファのスキャンラインの順序(デフォルト状態)
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// バッファのスケーリング方式設定(デフォルト設定)
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// 描画バッファのピクセルフォーマット指定(赤・緑・青・アルファをそれぞれ8ビット)
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// マルチサンプリング(アンチエイリアス処理)の数の設定(1なんでしない)
	desc.SampleDesc.Count = 1;
	// サンプルの品質設定(0なのでデフォルト値のまま使用)
	desc.SampleDesc.Quality = 0;
	// レンダーターゲットの使用指定(描画結果を画面に出力するための用途)
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// スワップチェーンのバッファの数の設定
	desc.BufferCount = FRAME_BUFFER_COUNT;
	// 出力先のハンドル設定
	desc.OutputWindow = m_hWnd;
	// ウィンドウモードでの描画指定
	desc.Windowed = TRUE;
	// スワップチェーンの交換方法(DISCARD:古いバッファを破棄して、新しいバッファを使用する交換方式)
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// フルスクリーンモードとウィンドウモードの切り替えを許可するフラグ(フルスクリーンに切り替える際にサポートする設定)
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// スワップチェインの生成
	IDXGISwapChain* pSwapChain = nullptr;
	hr = pFactory->CreateSwapChain(m_pQueue.Get(), &desc, &pSwapChain);
	if (FAILED(hr))
	{
		pFactory->Release();
		return false;
	}

	// IDXGISwapChain3を取得
	hr = pSwapChain->QueryInterface(IID_PPV_ARGS(m_pSwapChain.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		pFactory->Release();
		pSwapChain->Release();
		return false;
	}

	// バックバッファ番号を取得
	m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	pFactory->Release();
	pSwapChain->Release();
	return true;
}

// コマンドリスト:描画命令を記録し、溜めておく貯蔵庫みたいなもの(理論上無限だがメモリの範囲内まで)
// コマンドアロケーター:コマンドリストの使用するメモリの管理、メモリ割り当てをする機能
bool DrawBase::CreateCommandList()
{
	// コマンドアロケーターの作成
	HRESULT hr;
	for (size_t i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		hr = m_pDevice->CreateCommandAllocator(
			// GPUが直接実行するタイプのコマンド(グラフィックや描画命令など)を扱うことを指定
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			// COMポインタのメモリ管理を行い、新しいコマンドアロケーターのメモリを取得
			IID_PPV_ARGS(m_pAllocator[i].ReleaseAndGetAddressOf()));
	}

	if (FAILED(hr))
	{
		return false;
	}

	// コマンドリストの生成
	hr = m_pDevice->CreateCommandList(
		// ノードマスク(マルチGPU環境で使用する)
		0,	
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		// 現在のバックバッファ用のコマンドアロケーターを関連づける
		m_pAllocator[m_CurrentBackBufferIndex].Get(),
		// 初期のパイプラインステートを指定しない
		nullptr,
		IID_PPV_ARGS(&m_pCommandList)
	);

	if (FAILED(hr))
	{
		return false;
	}

	// コマンドリストは作成直後は開いてるので、いったん閉じる。
	m_pCommandList->Close();

	return true;
}

// フェンス:CPUとGPUの同期を行うためのもの、描画完了の判断をフェンスの値がインクリメントされたかで判断する
bool DrawBase::CreateFence()
{
	// フェンス値の初期化
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; i++)
	{
		m_fenceValue[i] = 0;
	}

	// フェンスの作成
	auto hr = m_pDevice->CreateFence(
		0,						// 初期値として0に設定
		D3D12_FENCE_FLAG_NONE,	// デフォルトの動作設定 
		IID_PPV_ARGS(m_pFence.ReleaseAndGetAddressOf()
		));
	if (FAILED(hr))
	{
		return false;
	}

	//現在のフレームのフェンス値を更新 
	m_fenceValue[m_CurrentBackBufferIndex]++;

	// 同期を行うときのイベントハンドラを作成する。
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	return m_fenceEvent != nullptr;
}

// ビューポート:ウィンドウにレンダリング結果をどう表示するかの設定
void DrawBase::CreateViewPort()
{
	// ビューポートの左上が画面の左上に配置されるよう設定
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;

	// ビューポートが画面全体を覆うように設定
	m_Viewport.Width = static_cast<float>(m_FrameBufferWidth);
	m_Viewport.Height = static_cast<float>(m_FrameBufferHeight);
	
	// 深度バッファの範囲を設定
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
}

// シザー矩形:ビューポートに表示された画像のどこまでを画面に映し出すかの設定
// 例:マリオカートの複数人プレイ時の画面描画とか
void DrawBase::CreateScissorRect()
{
	// 描画範囲の左端
	m_Scissor.left = 0;
	// 画面描画の右端
	m_Scissor.right = m_FrameBufferWidth;
	// 画面描画の上端
	m_Scissor.top = 0;
	// 画面描画の下端
	m_Scissor.bottom = m_FrameBufferHeight;
}

// レンダーターゲット:キャンバスのようなもの、描画先。実態としてはバックバッファやテクスチャなどのリソース
bool DrawBase::CreateRenderTarget()
{
	// レンダーターゲットビュー(RTV)用のディスクリプタヒープを作成する
	// RTV:レンダーターゲットリソースにアクセスするための窓口、橋渡しのようなもの
	// ディスクリプタヒープ:レンダーターゲットリソースなどの複数のディスクリプタを効率的に管理する領域
	// ディスクリプタ:リソース(テクスチャ、レンダーターゲット、深度バッファなど)を参照するハンドル
	
	// ディスクリプタヒープの初期化
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	// ヒープに含まれるディスクリプタの数を指定(フレーム毎に必要なディスクリプタを確保)
	desc.NumDescriptors = FRAME_BUFFER_COUNT;
	// ヒープの種類を指定(RTV用のディスクリプタヒープ)
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	// ヒープのフラグを指定(特別な処理をしない標準的な設定)
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	// ディスクリプタヒープを作成
	auto hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pRtvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// ディスクリプタのサイズを取得
	m_RtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// RTVヒープの開始ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();

	// バックバッファごとのレンダーターゲットビュー作成
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		// バックバッファの取得
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(m_pRenderTargets[i].ReleaseAndGetAddressOf()));
		// レンダーターゲットビューの作成
		m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);
		// ハンドルのインクリメント(次のバックバッファに対応するRTVを作成するために更新)
		rtvHandle.ptr += m_RtvDescriptorSize;
	}

	return true;
}

// 深度ステンシルバッファ:カメラから見たZ値(奥行き、深度を表す値)を持っておくためのバッファ
bool DrawBase::CreateDepthStencil()
{
	// DSV用のディスクリプタヒープを作成する
	// DSV:デプスステンシルビュー
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	// ディスクリプタの数の設定、今回は1つのDSVを格納する
	heapDesc.NumDescriptors = 1;
	// DSV用のディスクリプタヒープを作成するように指定
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	// シェーダーから直接アクセスしないため「NONE」に設定
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	// DirectX12デバイスを使用してディスクリプタヒープの作成
	auto hr = m_pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pDsvHeap));
	if (FAILED(hr))
	{
		return false;
	}

	// ディスクリプタヒープ内では各ディスクリプタの間隔が固定されている、なのでそのサイズを取得
	m_DsvDescriptorSize = m_pDevice->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// 深度ステンシル用のクリア値を設定
	D3D12_CLEAR_VALUE dsvClearValue;
	dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	dsvClearValue.DepthStencil.Depth = 1.0f;
	dsvClearValue.DepthStencil.Stencil = 0;

	// 深度ステンシルバッファの作成
	// デフォルトヒーププロパティの設定
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc(
		// 深度ステンシルバッファをテクスチャ2Dに設定
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		// 縦横のフレームバッファサイズを設定
		m_FrameBufferWidth,
		m_FrameBufferHeight,
		1,
		1,
		// 32-bit深度バッファを定義
		DXGI_FORMAT_D32_FLOAT,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		// 深度ステンシルの使用を許可、シェーダーリソースとしての使用を禁止
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | 
		D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

	hr = m_pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClearValue,
		IID_PPV_ARGS(m_pDepthStencilBuffer.ReleaseAndGetAddressOf())
	);

	if (FAILED(hr))
	{
		return false;
	}

	// DSVディスクリプタを作成
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDsvHeap->
		// DSVのディスクリプタハンドルを取得
		GetCPUDescriptorHandleForHeapStart();
	// 深度ステンシルビューを作成
	m_pDevice->CreateDepthStencilView(
			m_pDepthStencilBuffer.Get(), nullptr, dsvHandle);

	return true;
}

// 描画開始処理の関数
void DrawBase::BeginRender()
{
	// 現在のレンダーターゲットを更新
	m_currentRenderTarget = m_pRenderTargets[m_CurrentBackBufferIndex].Get();

	// コマンドを初期化して描画命令を溜める準備をする
	m_pAllocator[m_CurrentBackBufferIndex]->Reset();
	m_pCommandList->Reset(m_pAllocator[m_CurrentBackBufferIndex].Get(), nullptr);

	// ビューポートとシザー矩形を設定
	m_pCommandList->RSSetViewports(1, &m_Viewport);
	m_pCommandList->RSSetScissorRects(1, &m_Scissor);

	// 現在のフレームのレンダーターゲットビューのディスクリプタヒープの開始アドレスを取得
	auto currentRtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	currentRtvHandle.ptr += m_CurrentBackBufferIndex * m_RtvDescriptorSize;

	// 深度ステンシルのディスクリプタヒープの開始アドレス取得
	auto currentDsvHandle = m_pDsvHeap->GetCPUDescriptorHandleForHeapStart();

	// レンダーターゲットが使用可能になるまで待つ
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_currentRenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_pCommandList->ResourceBarrier(1, &barrier);

	// レンダーターゲットを設定
	m_pCommandList->OMSetRenderTargets(1, &currentRtvHandle, FALSE, &currentDsvHandle);

	// レンダーターゲットをクリア
	const float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	m_pCommandList->ClearRenderTargetView(currentRtvHandle, clearColor, 0, nullptr);

	// 深度ステンシルビューをクリア
	m_pCommandList->ClearDepthStencilView(currentDsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

// 描画完了を待つ関数
void DrawBase::WaitRender()
{
	// 現在のバックバッファに対応するフェンスの値を取得
	const UINT64 fenceValue = m_fenceValue[m_CurrentBackBufferIndex];
	// 現時点のフェンスの値をfenceValueに設定することをGPUに命令する
	m_pQueue->Signal(m_pFence.Get(), fenceValue);
	// 次回以降のフレームに備えて、フェンスの値を増やす
	m_fenceValue[m_CurrentBackBufferIndex]++;

	// 次のフレームの描画準備がまだであれば待機する
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		// 完了時にイベントを設定
		auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent);
		if (FAILED(hr))
		{
			return;
		}

		// 待機処理
		if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE))
		{
			return;
		}
	}
}

// 描画終了処理の関数
void DrawBase::EndRender()
{
	// リソースの状態を描画用から表示用に遷移させる処理
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		m_currentRenderTarget, 
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		D3D12_RESOURCE_STATE_PRESENT
	);

	// ResourceBarrier:リソースの使用目的変更時に必要
	m_pCommandList->ResourceBarrier(1, &barrier);

	// コマンドの記録を終了
	m_pCommandList->Close();

	// コマンドを実行
	ID3D12CommandList* ppCmdLists[] = { m_pCommandList.Get() };
	m_pQueue->ExecuteCommandLists(1, ppCmdLists);

	// スワップチェーンを切り替え
	m_pSwapChain->Present(1, 0);

	// 描画完了を待つ
	WaitRender();

	// バックバッファ番号更新
	m_CurrentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

//-------------------------------------------------------------------
// ゲッター
//-------------------------------------------------------------------

/// @brief デバイスのゲッター関数
/// @return デバイスを返します
ID3D12Device6* DrawBase::Device()
{
	return m_pDevice.Get();
}
/// @brief コマンドリストのゲッター関数
/// @return コマンドリストを返します
ID3D12GraphicsCommandList* DrawBase::CommandList()
{
	return m_pCommandList.Get();
}

/// @brief 現在のフレーム番号のゲッター関数
/// @return 現在のフレーム番号を返します
UINT DrawBase::CurrentBackBufferIndex()
{
	return m_CurrentBackBufferIndex;
}