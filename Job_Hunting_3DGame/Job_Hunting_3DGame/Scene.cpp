#include "Scene.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "IndexBuffer.h"

Scene* g_Scene;
VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[DrawBase::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;
IndexBuffer* indexBuffer;

bool Scene::Init()
{
	// ���_�f�[�^
	Vertex vertices[4] = {};
	vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[3].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	// ���_�o�b�t�@�S�̂̃T�C�Y���v�Z
	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	// 1���_�̃T�C�Y���擾
	auto vertexStride = sizeof(Vertex);
	// ���_�o�b�t�@�̃C���X�^���X����
	vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	
	if (!vertexBuffer->IsValid())
	{
		printf("���_�o�b�t�@�̐����Ɏ��s\n");
		return false;
	}

	// ����ɏ�����Ă鏇�Ԃŕ`�悷��
	uint32_t indices[] = { 0,1,2,0,2,3 };

	// �C���f�b�N�X�o�b�t�@�̐���
	auto size = sizeof(uint32_t) * std::size(indices);
	indexBuffer = new IndexBuffer(size, indices);
	if (!indexBuffer->IsValid())
	{
		printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
		return false;
	}

	// ���_�̈ʒu
	auto eyePos= DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); 
	// �����_�̍��W
	auto targetPos = DirectX::XMVectorZero(); 
	// �������\���x�N�g��
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
	// ����p
	auto fov = DirectX::XMConvertToRadians(37.5); 
	// �A�X�y�N�g��
	auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); 
	// �萔�o�b�t�@�̐����ƕϊ��s��̓o�^
	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; i++)
	{
		// �萔�o�b�t�@�̐���
		constantBuffer[i] = new ConstantBuffer(sizeof(Matrix));
		if (!constantBuffer[i]->IsValid())
		{
			printf("�ϊ��s��p�萔�o�b�t�@�̐����Ɏ��s\n");
			return false;
		}

		// �ϊ��s��̓o�^
		auto ptr = constantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = DirectX::XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->proj = DirectX::XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
	}

	rootSignature = new RootSignature();
	if (!rootSignature->IsValid())
	{
		printf("���[�g�V�O�l�`���̐����Ɏ��s\n");
		return false;
	}

	// �p�C�v���C���X�e�[�g�̃C���X�^���X����
	pipelineState = new PipelineState();
	// ���_���C�A�E�g�̐ݒ�
	pipelineState->SetInputLayout(Vertex::InputLayout);
	// ���[�g�V�O�l�`���̐ݒ�
	pipelineState->SetRootSignature(rootSignature->Get());

#ifdef _DEBUG	// DEBUG
	// VS��ݒ�
	pipelineState->SetVS(L"../x64/Debug/VS_Simple.cso");
	// PS��ݒ�
	pipelineState->SetPS(L"../x64/Debug/PS_Simple.cso");
#else			// Release
	// VS��ݒ�
	pipelineState->SetVS(L"../x64/Release/VS_Simple.cso");
	// PS��ݒ�
	pipelineState->SetPS(L"../x64/Release/PS_Simple.cso");
#endif 

	// �p�C�v���C���X�e�[�g�쐬
	pipelineState->Create();

	if (!pipelineState->IsValid())
	{
		printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s\n");
		return false;
	}

	printf("�V�[���̏����������ɐ���\n");
	return true;
}

void Scene::Update()
{
	//// ���t���[����]������
	//rotateY += 0.02f;
	//// ���݂̃t���[���ԍ����擾
	//auto cuurentIndex = g_DrawBase->CurrentBackBufferIndex();
	//// ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾
	//auto cuurentTransform = constantBuffer[cuurentIndex]->GetPtr<Matrix>();

	//cuurentTransform->world = DirectX::XMMatrixRotationY(rotateY);
}

void Scene::Draw()
{
	// ���݂̃t���[���ԍ����擾
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// �R�}���h���X�g
	auto commandList = g_DrawBase->CommandList();
	// ���_�o�b�t�@�r���[
	auto vbView = vertexBuffer->View();
	// �C���f�b�N�X�o�b�t�@�r���[
	auto ibView = indexBuffer->View();

	// ���[�g�V�O�l�`�����Z�b�g
	commandList->SetGraphicsRootSignature(rootSignature->Get());
	// �p�C�v���C���X�e�[�g���Z�b�g
	commandList->SetPipelineState(pipelineState->Get());
	// �萔�o�b�t�@���Z�b�g
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress());
	
	// �O�p�`��`�悷��ݒ�
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	// ���_�o�b�t�@���X���b�g0�Ԃ��g���Ĉ�����ݒ肷��
	commandList->IASetVertexBuffers(0, 1, &vbView);
	// �C���f�b�N�X�o�b�t�@���Z�b�g����
	commandList->IASetIndexBuffer(&ibView);
	
	// �O�p�`�|���S���񖇂Ŏl�p�`��`��
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
