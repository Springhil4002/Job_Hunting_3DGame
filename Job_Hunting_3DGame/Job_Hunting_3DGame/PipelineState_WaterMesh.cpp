#include "PipelinState_WaterMesh.h"
#include "PipelineState.h"
#include "DrawBase.h"
#include <d3dx12.h>
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

// �p�C�v���C���X�e�[�g�̐ݒ�
PipelineState::PipelineState()
{
	// ���X�^���C�U�[�X�e�[�g:�f�t�H���g�ݒ�
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// �J�����O�Ȃ�:���ʕ`��
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// �`�惂�f���̖��ߕ�
	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// �u�����h�X�e�[�g:�f�t�H���g�ݒ�(�s�����`��)
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// �[�x�X�e���V���X�e�[�g:�f�t�H���g�ݒ�(�[�x�e�X�gON,��������OK)
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	// �T���v���}�X�N:�S�ẴT���v����L����(�ʏ�͂���)
	desc.SampleMask = UINT_MAX;
	// �`��v���~�e�B�u:�O�p�`�`��Ɏw��
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �`��Ώ�:1��
	desc.NumRenderTargets = 1;
	//�@�t�H�[�}�b�g:sRGB�ϊ��t����RGBA8�`��
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// �[�x�o�b�t�@�̃t�H�[�}�b�g32bitFloat
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	// �}���`�T���v�����O:�Ȃ�
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
}

bool PipelineState::IsValid()
{
	return m_IsValid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ID3D12RootSignature* rootSignature)
{
	desc.pRootSignature = rootSignature;
}

void PipelineState::SetVS(std::wstring filePath)
{
	// ���_�V�F�[�_�[�ǂݍ���
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("���_�V�F�[�_�[�̓ǂݍ��݂Ɏ��s\n");
		return;
	}

	desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState::SetPS(std::wstring filePath)
{
	// �s�N�Z���V�F�[�_�[�ǂݍ���
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		printf("�s�N�Z���V�F�[�_�[�̓ǂݍ��݂Ɏ��s\n");
		return;
	}

	desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState::Create()
{
	// �p�C�v���C���X�e�[�g�𐶐�
	auto hr = g_DrawBase->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s\n");
		return;
	}

	m_IsValid = true;
}

ID3D12PipelineState* PipelineState::Get()
{
	return m_pPipelineState.Get();
}