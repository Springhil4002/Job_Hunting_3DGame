#include "RootSignature_WaterMesh.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include "Debug_New.h"

RootSignature_WaterMesh::RootSignature_WaterMesh()
{
	// �A�v���P�[�V�����̓��̓A�Z���u�����g�p����
	auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// �h���C���V�F�[�_�[�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
	// �n���V�F�[�_�[�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
	// �W�I���g���V�F�[�_�[�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
	flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// �f�B�X�N���v�^�����W
	CD3DX12_DESCRIPTOR_RANGE texRange[1] = {};
	// t3��1��SRV
	texRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

	// ���[�g�p�����[�^�ݒ�
	CD3DX12_ROOT_PARAMETER rootParam[5] = {};
	// b0:��Matrix�萔�o�b�t�@��ݒ�A�S�ẴV�F�[�_�[���猩����悤�ɂ���
	rootParam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); 
	// b1:Gerstner�g�萔�o�b�t�@��ݒ�A�S�ẴV�F�[�_�[���猩����悤�ɂ���
	rootParam[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL); 
	// b2:���C�g�p�萔�o�b�t�@��ݒ�A�S�ẴV�F�[�_�[���猩����悤�ɂ���
	rootParam[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	// b3:WaterParams�A���ׂẴV�F�[�_�[���猩����悤�ɂ���
	rootParam[3].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	// t3�FCubeMap��ݒ�A�s�N�Z���V�F�[�_�[���猩����悤�ɂ���
	rootParam[4].InitAsDescriptorTable(1, &texRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	
	// �X�^�e�B�b�N�T���v���[�̐ݒ�
	auto sampler = CD3DX12_STATIC_SAMPLER_DESC(
		0,									// ���W�X�^�[
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,	// �t�B���^�[
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// �A�h���X���[�h U
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// �A�h���X���[�h V
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,	// �A�h���X���[�h W
		0.0f,								// MipLODBias
		1,									// MaxAnisotropy
		D3D12_COMPARISON_FUNC_ALWAYS,		// ��r�֐��i�g���ĂȂ��Ȃ� ALWAYS ��OK�j
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
		0.0f,								// MinLOD
		D3D12_FLOAT32_MAX,					// MaxLOD
		D3D12_SHADER_VISIBILITY_PIXEL		// PS���猩����悤�ɖ���
	);

	// ���[�g�V�O�j�`���̐ݒ�i�ݒ肵�������[�g�p�����[�^�[�ƃX�^�e�B�b�N�T���v���[������j
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	// ���[�g�p�����[�^�[�̌��������
	desc.NumParameters = _countof(rootParam);
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
		printf("WaterMesh:���[�g�V�O�l�`���V���A���C�Y�Ɏ��s");
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
		printf("WaterMesh:���[�g�V�O�l�`���̐����Ɏ��s");
		return;
	}

	m_IsValid = true;
}

bool RootSignature_WaterMesh::IsValid() const
{
	return m_IsValid;
}

ID3D12RootSignature* RootSignature_WaterMesh::Get()
{
	return m_pRootSignature.Get();
}