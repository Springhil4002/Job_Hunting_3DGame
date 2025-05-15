#include "IndexBuffer.h"
#include <d3dx12.h>
#include "DrawBase.h"

IndexBuffer::IndexBuffer(size_t _size, const uint32_t* _pInitData)
{
	// ヒーププロパティ
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 
	// リソースの設定
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(_size);	

	// リソースを生成
	auto hr = g_DrawBase->Device()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		printf("[OnInit] インデックスバッファリソースの生成に失敗");
		return;
	}

	// インデックスバッファビューの設定
	m_View = {};
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_View.Format = DXGI_FORMAT_R32_UINT;
	m_View.SizeInBytes = static_cast<UINT>(_size);

	// マッピングする
	if (_pInitData != nullptr)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			printf("[OnInit] インデックスバッファマッピングに失敗");
			return;
		}

		// インデックスデータをマッピング先に設定
		memcpy(ptr, _pInitData, _size);

		// マッピング解除
		m_pBuffer->Unmap(0, nullptr);
	}
	m_IsValid = true;
}

bool IndexBuffer::IsValid()
{
	return m_IsValid;
}

D3D12_INDEX_BUFFER_VIEW IndexBuffer::View() const
{
	return m_View;
}
