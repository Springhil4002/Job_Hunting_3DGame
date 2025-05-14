#include <d3dx12.h>
#include "VertexBuffer.h"
#include "DrawBase.h"

VertexBuffer::VertexBuffer(size_t _size, size_t _stride, const void* _pInitData)
{
	// ヒーププロパティ
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 	
	// リソースの設定
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(_size); 	

	// リソースを生成(GPU用バッファを確保)
	auto hr = g_DrawBase->Device()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// 読み込み専用の初期状態
		nullptr,
		IID_PPV_ARGS(m_pBuffer.GetAddressOf()));
	// 失敗時のエラー出力
	if (FAILED(hr))
	{
		printf("頂点バッファリソースの生成に失敗");
		return;
	}

	// 頂点バッファビューの設定
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_View.SizeInBytes = static_cast<UINT>(_size);
	m_View.StrideInBytes = static_cast<UINT>(_stride);

	// マッピングする
	if (_pInitData != nullptr)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			printf("頂点バッファマッピングに失敗");
			return;
		}

		// 頂点データをマッピング先にコピー
		memcpy(ptr, _pInitData, _size);

		// マッピング解除
		m_pBuffer->Unmap(0, nullptr);
	}

	// 作成出来たら成功
	m_IsValid = true;
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::View() const
{
	return m_View;
}

bool VertexBuffer::IsValid()
{
	return m_IsValid;
}
