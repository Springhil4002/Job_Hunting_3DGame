#pragma once
#include <cstdint>
#include <d3d12.h>
#include "ComPtr.h"

class IndexBuffer
{
private:
	bool m_IsValid = false;
	// インデックスバッファ
	ComPtr<ID3D12Resource> m_pBuffer; 
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW m_View; 

	// コピー禁止
	IndexBuffer(const IndexBuffer&) = delete;
	void operator = (const IndexBuffer&) = delete;
public:
	/// @brief コンストラクタ
	/// @param size データサイズ
	/// @param pInitData 初期データ
	IndexBuffer(size_t _size, const uint32_t* _pInitData = nullptr);
	/// @brief 生成の成否を返す処理
	/// @return 生成の成否結果を返します
	bool IsValid();
	/// @brief インデックスバッファビューを返す処理
	/// @return インデックスバッファを返します
	D3D12_INDEX_BUFFER_VIEW View() const;
};

