#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "DrawBase.h"
#include <d3dx12.h>

const UINT HANDLE_MAX = 512;

DescriptorHeap::DescriptorHeap()
{
	// ハンドルリストをクリア
	m_pHandles.clear();
	// ハンドルリストを最大個数分取っておく
	m_pHandles.reserve(HANDLE_MAX);

	// ディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NodeMask = 1;
	// テクスチャ・定数バッファ用のディスクリプタヒープ
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// 最大登録数
	desc.NumDescriptors = HANDLE_MAX;
	// シェーダーからアクセスできるようにする
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	// デバイス取得
	auto device = g_DrawBase->Device();

	// ディスクリプタヒープを生成
	auto hr = device->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf()));
	// 作成失敗時
	if (FAILED(hr))
	{
		m_IsValid = false;
		return;
	}
	// ディスクリプタの間隔を取得(ヒープ内で次のディスクリプタに移動するのに必要)
	m_IncrementSize = device->GetDescriptorHandleIncrementSize(desc.Type); 
	m_IsValid = true;
}

ID3D12DescriptorHeap* DescriptorHeap::GetHeap()
{
	return m_pHeap.Get();
}

DescriptorHandle* DescriptorHeap::Register(Texture2D* _texture)
{
	if (!_texture) {
		OutputDebugStringA("Error: null テクスチャを Register に渡しました。\n");
		return nullptr;
	}

	// ディスクリプタヒープに登録できるかチェック
	auto count = m_pHandles.size();
	if (HANDLE_MAX <= count)
	{
		return nullptr;
	}

	// ディスクリプタヒープの最初のアドレス
	auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart(); 
	// 最初のアドレスからcount分進めた場所が今回の登録先
	handleCPU.ptr += m_IncrementSize * count; 

	// ディスクリプタヒープの最初のアドレス
	auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); 
	// 最初のアドレスからcount分進めた場所が今回の登録先
	handleGPU.ptr += m_IncrementSize * count; 

	// ディスクリプタハンドルを生成
	DescriptorHandle* pHandle = new DescriptorHandle();
	// CPU・GPUの登録したハンドルをメンバ変数に代入
	pHandle->handleCPU = handleCPU;
	pHandle->handleGPU = handleGPU;

	// デバイスを取得
	auto device = g_DrawBase->Device();
	// リソースを取得
	auto resource = _texture->Resource();
	// シェーダーリソースビューを取得
	auto desc = _texture->ViewDesc();

	// テクスチャ用のSRVを作成、ディスクリプタヒープの指定位置に登録
	device->CreateShaderResourceView(resource, &desc, pHandle->handleCPU); 
	// ハンドルリストに登録
	m_pHandles.push_back(pHandle);
	// ハンドルを返す
	return pHandle;	
}