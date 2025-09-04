#pragma once
#include <d3d12.h>
#include <vector>
#include <memory>
#include "System/ComPtr.h"

class ConstantBuffer;
class Texture2D;
class TextureCube;

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
	std::vector<std::shared_ptr<DescriptorHandle>> m_pHandles;
public:
	/// @brief コンストラクタで生成
	DescriptorHeap(); 
	/// @brief デストラクタ
	~DescriptorHeap();
	/// @brief ディスクリプタヒープを取得する関数
	/// @return ディスクリプタヒープを返します
	ID3D12DescriptorHeap* GetHeap(); 
	/// @brief テクスチャ2Dをディスクリプタヒープに登録、ハンドルを取得する関数
	/// @param texture テクスチャ2D
	/// @return ディスクリプタハンドルを返します
	std::shared_ptr<DescriptorHandle> Register(Texture2D* _texture);
	/// @brief テクスチャキューブをディスクリプタヒープに登録、ハンドルを取得する関数
	/// @param _texture テクスチャキューブ
	/// @return ディスクリプタハンドルを返します
	std::shared_ptr<DescriptorHandle> Register(TextureCube* _texture);
	/// @brief ディスクリプタ生成の成否を取得する関数
	/// @return ディスクリプタ生成の成否
	bool IsValid() const { return m_IsValid; }
};