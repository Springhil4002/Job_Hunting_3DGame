#pragma once
#include <d3d12.h>
#include "ComPtr.h"

class VertexBuffer
{
private:
	bool m_IsValid = false;						// バッファの生成に成功したかのフラグ
	ComPtr<ID3D12Resource> m_pBuffer = nullptr; // バッファ
	D3D12_VERTEX_BUFFER_VIEW m_View = {};		// 頂点バッファビュー

	// コピー禁止
	VertexBuffer(const VertexBuffer&) = delete;
	void operator = (const VertexBuffer&) = delete;
public:
	/// @brief コンストラクタでバッファを生成
	/// @param _size		:バッファサイズ
	/// @param _stride		:1頂点あたりのデータサイズ
	/// @param _pInitData	:初期データ
	VertexBuffer(size_t _size, size_t _stride, const void* _pInitData);

	/// @brief	頂点バッファビューを取得する関数
	/// @return	頂点バッファビューを返します 
	D3D12_VERTEX_BUFFER_VIEW View() const;

	/// @brief	バッファの生成に成功したかを取得
	/// @return バッファ生成の成否を返します
	bool IsValid();								
};