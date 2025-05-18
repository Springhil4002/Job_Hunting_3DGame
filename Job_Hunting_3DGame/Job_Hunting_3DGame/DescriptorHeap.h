#pragma once
#include <d3d12.h>
#include <vector>
#include "ComPtr.h"

class ConstantBuffer;
class Texture2D;

class DescriptorHandle
{
public:
	/// @brief CPUハンドル:リソースビューなどの操作するのに必要なハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
	/// @brief GPUハンドル:描画するときに必要ハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU;
};

class DescriptorHeap
{
private:
	// ディスクリプタの間隔
	UINT m_IncrementSize = 0;
	// ディスクリプタ生成の成否フラグ
	bool m_IsValid = false;
	// ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr;
	// 登録されたディスクリプタハンドルを保持
	std::vector<DescriptorHandle*> m_pHandles;
public:
	/// @brief コンストラクタで生成
	DescriptorHeap(); 
	/// @brief ディスクリプタヒープを取得する関数
	/// @return ディスクリプタヒープを返します
	ID3D12DescriptorHeap* GetHeap(); 
	/// @brief テクスチャ2Dをディスクリプタヒープに登録、ハンドルを取得する関数
	/// @param texture テクスチャ
	/// @return ディスクリプタハンドルを返します
	DescriptorHandle* Register(Texture2D* _texture); 
};