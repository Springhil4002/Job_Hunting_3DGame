#include "IndexBuffer.h"
#include <d3dx12.h>
#include "DrawBase.h"

IndexBuffer::IndexBuffer(size_t _size, const uint32_t* _pInitData)
{
	// �q�[�v�v���p�e�B
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 
	// ���\�[�X�̐ݒ�
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(_size);	

	// ���\�[�X�𐶐�
	auto hr = g_DrawBase->Device()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		printf("[OnInit] �C���f�b�N�X�o�b�t�@���\�[�X�̐����Ɏ��s");
		return;
	}

	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
	m_View = {};
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_View.Format = DXGI_FORMAT_R32_UINT;
	m_View.SizeInBytes = static_cast<UINT>(_size);

	// �}�b�s���O����
	if (_pInitData != nullptr)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			printf("[OnInit] �C���f�b�N�X�o�b�t�@�}�b�s���O�Ɏ��s");
			return;
		}

		// �C���f�b�N�X�f�[�^���}�b�s���O��ɐݒ�
		memcpy(ptr, _pInitData, _size);

		// �}�b�s���O����
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
