#pragma once
#include "ComPtr.h"
#include <string>
#include <d3dx12.h>

class DescriptorHandle;
class DescriptorHeap;

class Texture2D
{
private:
	// �ǂݍ��݂̐��ۃt���O
	bool m_IsValid;
	// �e�N�X�`������ێ����郊�\�[�X
	ComPtr<ID3D12Resource> m_pResource;
	/// @brief �R���X�g���N�^
	/// @param _path �t�@�C���p�X
	Texture2D(std::string _path);
	/// @brief �R���X�g���N�^
	/// @param _path �t�@�C���p�X
	Texture2D(std::wstring _path);
	/// @brief �R���X�g���N�^
	/// @param _buffer ���\�[�X
	Texture2D(ID3D12Resource* _buffer);
	
	/// @brief �t�@�C���p�X��ǂݍ���Ńe�N�X�`�����\�[�X���쐬
	/// @param _path �t�@�C���p�X
	/// @return �쐬�̐��ۂ�Ԃ��܂�
	bool Load(std::string& _path);
	/// @brief �t�@�C���p�X��ǂݍ���Ńe�N�X�`�����\�[�X���쐬
	/// @param _path �t�@�C���p�X
	/// @return �쐬�̐��ۂ�Ԃ��܂�
	bool Load(std::wstring& _path);
	/// @brief �F�Ȃ��̃e�N�X�`���Ȃǂ̃��\�[�X�����֐�
	/// @param _width ����
	/// @param _height ����
	/// @return �e�N�X�`�����\�[�X��Ԃ��܂�
	static ID3D12Resource* GetDefaultResource(size_t _width, size_t _height);

	// �R�s�[�֎~
	Texture2D(const Texture2D&) = delete;
	void operator = (const Texture2D&) = delete;
public:
	/// @brief �t�@�C���p�X��n���ăe�N�X�`�����擾����֐�
	/// @param _path �t�@�C���p�X
	/// @return �e�N�X�`����Ԃ��܂�
	static Texture2D* Get(std::string _path); 
	/// @brief �t�@�C���p�X��n���ăe�N�X�`�����擾����֐�
	/// @param _path �t�@�C���p�X
	/// @return �e�N�X�`����Ԃ��܂�
	static Texture2D* Get(std::wstring _path);
	/// @brief ���F�e�N�X�`���𐶐�����֐�
	/// @return �e�N�X�`����Ԃ��܂�
	static Texture2D* GetWhite(); 
	/// @brief �ǂݍ��݂̐��ۂ��擾����֐�
	/// @return �ǂݍ��݂̐��ۂ�Ԃ��܂�
	bool IsValid(); 

	/// @brief �e�N�X�`�����\�[�X���擾����֐�
	/// @return �e�N�X�`�����\�[�X��Ԃ��܂�
	ID3D12Resource* Resource(); 
	/// @brief �V�F�[�_�[���\�[�X�r���[�̐ݒ���擾����֐�
	/// @return �V�F�[�_�[���\�[�X�r���[��Ԃ��܂�
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();
};
