#pragma once
#include "System/ComPtr.h"

struct ID3D12RootSignature;

class RootSignature
{
private:
	// ���[�g�V�O�l�`���̐����ɐ����������̃t���O
	bool m_IsValid = false; 
	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr; 
public:
	// �R���X�g���N�^�Ń��[�g�V�O�l�`���𐶐�
	RootSignature(); 
	/// @brief // ���[�g�V�O�l�`���̐����ɐ��ۂ��擾���鏈��
	/// @return ���[�g�V�O�l�`���̐������ۂ�Ԃ�
	bool IsValid(); 
	/// @brief ���[�g�V�O�l�`�����擾���鏈��
	/// @return // ���[�g�V�O�l�`����Ԃ�
	ID3D12RootSignature* Get(); 
};

