#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "DrawBase.h"
#include <d3dx12.h>

const UINT HANDLE_MAX = 512;

DescriptorHeap::DescriptorHeap()
{
	// �n���h�����X�g���N���A
	m_pHandles.clear();
	// �n���h�����X�g���ő��������Ă���
	m_pHandles.reserve(HANDLE_MAX);

	// �f�B�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NodeMask = 1;
	// �e�N�X�`���E�萔�o�b�t�@�p�̃f�B�X�N���v�^�q�[�v
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// �ő�o�^��
	desc.NumDescriptors = HANDLE_MAX;
	// �V�F�[�_�[����A�N�Z�X�ł���悤�ɂ���
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	// �f�o�C�X�擾
	auto device = g_DrawBase->Device();

	// �f�B�X�N���v�^�q�[�v�𐶐�
	auto hr = device->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf()));
	// �쐬���s��
	if (FAILED(hr))
	{
		m_IsValid = false;
		return;
	}
	// �f�B�X�N���v�^�̊Ԋu���擾(�q�[�v���Ŏ��̃f�B�X�N���v�^�Ɉړ�����̂ɕK�v)
	m_IncrementSize = device->GetDescriptorHandleIncrementSize(desc.Type); 
	m_IsValid = true;
}

ID3D12DescriptorHeap* DescriptorHeap::GetHeap()
{
	return m_pHeap.Get();
}

DescriptorHandle* DescriptorHeap::Register(Texture2D* _texture)
{
	if (!_texture) {
		OutputDebugStringA("Error: null �e�N�X�`���� Register �ɓn���܂����B\n");
		return nullptr;
	}

	// �f�B�X�N���v�^�q�[�v�ɓo�^�ł��邩�`�F�b�N
	auto count = m_pHandles.size();
	if (HANDLE_MAX <= count)
	{
		return nullptr;
	}

	// �f�B�X�N���v�^�q�[�v�̍ŏ��̃A�h���X
	auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart(); 
	// �ŏ��̃A�h���X����count���i�߂��ꏊ������̓o�^��
	handleCPU.ptr += m_IncrementSize * count; 

	// �f�B�X�N���v�^�q�[�v�̍ŏ��̃A�h���X
	auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); 
	// �ŏ��̃A�h���X����count���i�߂��ꏊ������̓o�^��
	handleGPU.ptr += m_IncrementSize * count; 

	// �f�B�X�N���v�^�n���h���𐶐�
	DescriptorHandle* pHandle = new DescriptorHandle();
	// CPU�EGPU�̓o�^�����n���h���������o�ϐ��ɑ��
	pHandle->handleCPU = handleCPU;
	pHandle->handleGPU = handleGPU;

	// �f�o�C�X���擾
	auto device = g_DrawBase->Device();
	// ���\�[�X���擾
	auto resource = _texture->Resource();
	// �V�F�[�_�[���\�[�X�r���[���擾
	auto desc = _texture->ViewDesc();

	// �e�N�X�`���p��SRV���쐬�A�f�B�X�N���v�^�q�[�v�̎w��ʒu�ɓo�^
	device->CreateShaderResourceView(resource, &desc, pHandle->handleCPU); 
	// �n���h�����X�g�ɓo�^
	m_pHandles.push_back(pHandle);
	// �n���h����Ԃ�
	return pHandle;	
}