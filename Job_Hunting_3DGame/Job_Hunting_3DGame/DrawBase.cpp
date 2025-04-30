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