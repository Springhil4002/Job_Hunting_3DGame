#include "RootSignature.h"
#include "DrawBase.h"
#include <d3dx12.h>

RootSignature::RootSignature()
{	
	// �A�v���P�[�V�����̓��̓A�Z���u�����g�p����
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
	// �h���C���V�F�[�_�[�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	// �n���V�F�[�_�[�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS; 
	// �W�I���g���V�F�[�_�[�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS; 

	// ���[�g�p�����[�^��ݒ�
	// �萔�o�b�t�@�ƃe�N�X�`���̓��
	CD3DX12_ROOT_PARAMETER rootParam[2] = {};
	// b0�̒萔�o�b�t�@��ݒ�A�S�ẴV�F�[�_�[���猩����悤�ɂ���
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); 

	// �f�B�X�N���v�^�e�[�u��
	CD3DX12_DESCRIPTOR_RANGE tableRange[1] = {};
	// ����������:�V�F�[�_�[���\�[�X�r���[��ݒ�A��n���At0�Ƀo�C���h
	tableRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	// t1�ɃV�F�[�_�[���\�[�X��ݒ�A�S�ẴV�F�[�_�[���猩����悤�ɂ���
	rootParam[1].InitAsDescriptorTable(std::size(tableRange), tableRange, D3D12_SHADER_VISIBILITY_ALL);

	// �X�^�e�B�b�N�T���v���[�̐ݒ�
	auto sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

	// ���[�g�V�O�j�`���̐ݒ�i�ݒ肵�������[�g�p�����[�^�[�ƃX�^�e�B�b�N�T���v���[������j
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	// ���[�g�p�����[�^�[�̌��������
	desc.NumParameters = std::size(rootParam);
	// �T���v���[�̌��������
	desc.NumStaticSamplers = 1; 
	// ���[�g�p�����[�^�[�̃|�C���^�������
	desc.pParameters = rootParam; 
	// �T���v���[�̃|�C���^������
	desc.pStaticSamplers = &sampler;
	// �t���O��ݒ�
	desc.Flags = flag; 

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3DBlob> pErrorBlob;

	// �V���A���C�Y
	auto hr = D3D12SerializeRootSignature(
		&desc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		pBlob.GetAddressOf(),
		pErrorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("���[�g�V�O�l�`���V���A���C�Y�Ɏ��s");
		return;
	}

	// ���[�g�V�O�l�`������
	hr = g_DrawBase->Device()->CreateRootSignature(
		0, // GPU����������ꍇ�̃m�[�h�}�X�N�i�����1���������z��Ȃ̂�0�j
		pBlob->GetBufferPointer(), // �V���A���C�Y�����f�[�^�̃|�C���^
		pBlob->GetBufferSize(), // �V���A���C�Y�����f�[�^�̃T�C�Y
		IID_PPV_ARGS(m_pRootSignature.GetAddressOf())); // ���[�g�V�O�j�`���i�[��̃|�C���^
	if (FAILED(hr))
	{
		printf("���[�g�V�O�l�`���̐����Ɏ��s");
		return;
	}

	m_IsValid = true;
}

bool RootSignature::IsValid()
{
	return m_IsValid;
}

ID3D12RootSignature* RootSignature::Get()
{
	return m_pRootSignature.Get();
}