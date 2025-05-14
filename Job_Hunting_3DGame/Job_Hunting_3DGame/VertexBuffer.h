#pragma once
#include <d3d12.h>
#include "ComPtr.h"

class VertexBuffer
{
private:
	bool m_IsValid = false;						// �o�b�t�@�̐����ɐ����������̃t���O
	ComPtr<ID3D12Resource> m_pBuffer = nullptr; // �o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW m_View = {};		// ���_�o�b�t�@�r���[

	// �R�s�[�֎~
	VertexBuffer(const VertexBuffer&) = delete;
	void operator = (const VertexBuffer&) = delete;
public:
	/// @brief �R���X�g���N�^�Ńo�b�t�@�𐶐�
	/// @param _size		:�o�b�t�@�T�C�Y
	/// @param _stride		:1���_������̃f�[�^�T�C�Y
	/// @param _pInitData	:�����f�[�^
	VertexBuffer(size_t _size, size_t _stride, const void* _pInitData);

	/// @brief	���_�o�b�t�@�r���[���擾����֐�
	/// @return	���_�o�b�t�@�r���[��Ԃ��܂� 
	D3D12_VERTEX_BUFFER_VIEW View() const;

	/// @brief	�o�b�t�@�̐����ɐ������������擾
	/// @return �o�b�t�@�����̐��ۂ�Ԃ��܂�
	bool IsValid();								
};