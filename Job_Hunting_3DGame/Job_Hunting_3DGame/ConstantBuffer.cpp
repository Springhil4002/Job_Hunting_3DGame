#include "ConstantBuffer.h"
#include "DrawBase.h"

ConstantBuffer::ConstantBuffer(size_t _size)
{
    // アライメント調整
    size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    // alignに切り上げる.
    UINT64 sizeAligned = (_size + (align - 1)) & ~(align - 1); 
    // ヒーププロパティ
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 
    // リソースの設定
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeAligned); 

    // リソースを生成
    auto hr = g_DrawBase->Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,  // CPUが読み込める状態にリソースを初期化
        nullptr,    
        IID_PPV_ARGS(m_pBuffer.GetAddressOf()));    // リソースを保持するポインタ
    if (FAILED(hr))
    {
        printf("定数バッファリソースの生成に失敗\n");
        return;
    }
    // バッファのマッピング
    hr = m_pBuffer->Map(0, nullptr, &m_pMappedPtr);
    if (FAILED(hr))
    {
        printf("定数バッファのマッピングに失敗\n");
        return;
    }
    // 定数バッファビューの設定
    m_Desc = {};
    m_Desc.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
    m_Desc.SizeInBytes = UINT(sizeAligned);

    // 作成できたら成功
    m_IsValid = true;
}

bool ConstantBuffer::IsValid()
{
    return m_IsValid;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const
{
    return m_Desc.BufferLocation;
}

D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer::ViewDesc()
{
    return m_Desc;
}

void* ConstantBuffer::GetPtr() const
{
    return m_pMappedPtr;
}

