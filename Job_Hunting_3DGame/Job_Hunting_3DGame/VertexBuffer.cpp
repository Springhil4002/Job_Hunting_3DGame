#include <d3dx12.h>
#include "VertexBuffer.h"
#include "DrawBase.h"

VertexBuffer::VertexBuffer(size_t _size, size_t _stride, const void* _pInitData)
{
	// �q�[�v�v���p�e�B
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 	
	// ���\�[�X�̐ݒ�
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(_size); 	

	// ���\�[�X�𐶐�(GPU�p�o�b�t�@���m��)
	auto hr = g_DrawBase->Device()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,	// �ǂݍ��ݐ�p�̏������
		nullptr,
		IID_PPV_ARGS(m_pBuffer.GetAddressOf()));
	// ���s���̃G���[�o��
	if (FAILED(hr))
	{
		printf("���_�o�b�t�@���\�[�X�̐����Ɏ��s");
		return;
	}

	// ���_�o�b�t�@�r���[�̐ݒ�
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_View.SizeInBytes = static_cast<UINT>(_size);
	m_View.StrideInBytes = static_cast<UINT>(_stride);

	// �}�b�s���O����
	if (_pInitData != nullptr)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			printf("���_�o�b�t�@�}�b�s���O�Ɏ��s");
			return;
		}

		// ���_�f�[�^���}�b�s���O��ɃR�s�[
		memcpy(ptr, _pInitData, _size);

		// �}�b�s���O����
		m_pBuffer->Unmap(0, nullptr);
	}

	// �쐬�o�����琬��
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
