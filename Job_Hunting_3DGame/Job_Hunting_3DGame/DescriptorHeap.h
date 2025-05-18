#pragma once
#include <d3d12.h>
#include <vector>
#include "ComPtr.h"

class ConstantBuffer;
class Texture2D;

class DescriptorHandle
{
public:
	/// @brief CPU�n���h��:���\�[�X�r���[�Ȃǂ̑��삷��̂ɕK�v�ȃn���h��
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
	/// @brief GPU�n���h��:�`�悷��Ƃ��ɕK�v�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU;
};

class DescriptorHeap
{
private:
	// �f�B�X�N���v�^�̊Ԋu
	UINT m_IncrementSize = 0;
	// �f�B�X�N���v�^�����̐��ۃt���O
	bool m_IsValid = false;
	// �f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr;
	// �o�^���ꂽ�f�B�X�N���v�^�n���h����ێ�
	std::vector<DescriptorHandle*> m_pHandles;
public:
	/// @brief �R���X�g���N�^�Ő���
	DescriptorHeap(); 
	/// @brief �f�B�X�N���v�^�q�[�v���擾����֐�
	/// @return �f�B�X�N���v�^�q�[�v��Ԃ��܂�
	ID3D12DescriptorHeap* GetHeap(); 
	/// @brief �e�N�X�`��2D���f�B�X�N���v�^�q�[�v�ɓo�^�A�n���h�����擾����֐�
	/// @param texture �e�N�X�`��
	/// @return �f�B�X�N���v�^�n���h����Ԃ��܂�
	DescriptorHandle* Register(Texture2D* _texture); 
};