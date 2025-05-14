#include "ConstantBuffer.h"
#include "DrawBase.h"

ConstantBuffer::ConstantBuffer(size_t _size)
{
    // �A���C�����g����
    size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    // align�ɐ؂�グ��.
    UINT64 sizeAligned = (_size + (align - 1)) & ~(align - 1); 
    // �q�[�v�v���p�e�B
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 
    // ���\�[�X�̐ݒ�
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeAligned); 

    // ���\�[�X�𐶐�
    auto hr = g_DrawBase->Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,  // CPU���ǂݍ��߂��ԂɃ��\�[�X��������
        nullptr,    
        IID_PPV_ARGS(m_pBuffer.GetAddressOf()));    // ���\�[�X��ێ�����|�C���^
    if (FAILED(hr))
    {
        printf("�萔�o�b�t�@���\�[�X�̐����Ɏ��s\n");
        return;
    }
    // �o�b�t�@�̃}�b�s���O
    hr = m_pBuffer->Map(0, nullptr, &m_pMappedPtr);
    if (FAILED(hr))
    {
        printf("�萔�o�b�t�@�̃}�b�s���O�Ɏ��s\n");
        return;
    }
    // �萔�o�b�t�@�r���[�̐ݒ�
    m_Desc = {};
    m_Desc.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
    m_Desc.SizeInBytes = UINT(sizeAligned);

    // �쐬�ł����琬��
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

