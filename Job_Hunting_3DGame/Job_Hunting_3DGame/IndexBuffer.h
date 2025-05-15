#pragma once
#include <cstdint>
#include <d3d12.h>
#include "ComPtr.h"

class IndexBuffer
{
private:
	bool m_IsValid = false;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> m_pBuffer; 
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW m_View; 

	// �R�s�[�֎~
	IndexBuffer(const IndexBuffer&) = delete;
	void operator = (const IndexBuffer&) = delete;
public:
	/// @brief �R���X�g���N�^
	/// @param size �f�[�^�T�C�Y
	/// @param pInitData �����f�[�^
	IndexBuffer(size_t _size, const uint32_t* _pInitData = nullptr);
	/// @brief �����̐��ۂ�Ԃ�����
	/// @return �����̐��ی��ʂ�Ԃ��܂�
	bool IsValid();
	/// @brief �C���f�b�N�X�o�b�t�@�r���[��Ԃ�����
	/// @return �C���f�b�N�X�o�b�t�@��Ԃ��܂�
	D3D12_INDEX_BUFFER_VIEW View() const;
};

