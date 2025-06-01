#pragma once
#include "System/ComPtr.h"
#include <d3dx12.h>
#include <string>

class PipelineState_WaterMesh
{
private:
	// �����ɐ��ۂ̃t���O
	bool m_IsValid = false;
	// �p�C�v���C���X�e�[�g�̐ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	// �p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
	// ���_�V�F�[�_�[
	ComPtr<ID3DBlob> m_pVsBlob;
	// �s�N�Z���V�F�[�_�[
	ComPtr<ID3DBlob> m_pPSBlob;
public:
	/// @brief �R���X�g���N�^�ł�����x�̐ݒ������
	PipelineState_WaterMesh();
	/// @brief �����̐��ۂ�Ԃ�����
	/// @return�@�����̐��ۂ�Ԃ��܂� 
	bool IsValid();
	/// @brief ���̓��C�A�E�g��ݒ肷�鏈��
	/// @param _layout ���̓��C�A�E�g
	void SetInputLayout(D3D12_INPUT_LAYOUT_DESC _layout);
	/// @brief ���[�g�V�O�l�`����ݒ肷�鏈��
	/// @param _rootSignature ���[�g�V�O�l�`��
	void SetRootSignature(ID3D12RootSignature* _rootSignature);
	/// @brief ���_�V�F�[�_�[��ݒ肷�鏈��
	/// @param _filePath �t�@�C���p�X��
	void SetVS(std::wstring _filePath);
	/// @brief // �s�N�Z���V�F�[�_�[��ݒ肷�鏈��
	/// @param _filePath �t�@�C���p�X��
	void SetPS(std::wstring _filePath);
	/// @brief �p�C�v���C���X�e�[�g�𐶐����鏈��
	void Create();
	/// @brief �p�C�v���C���X�e�[�g�̃|�C���^�̃Q�b�^�[�֐�
	/// @return �p�C�v���C���X�e�[�g�̃|�C���^
	ID3D12PipelineState* Get();
};

