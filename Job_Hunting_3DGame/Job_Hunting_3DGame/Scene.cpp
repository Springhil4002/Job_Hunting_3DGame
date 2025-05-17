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
#include "AssimpLoader.h"

Scene* g_Scene;
VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[DrawBase::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;
IndexBuffer* indexBuffer;
const wchar_t* modelFile = L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX";
std::vector<Mesh> meshes;
std::vector<VertexBuffer*> vertexBuffers;	// ���b�V���̐����̒��_�o�b�t�@
std::vector<IndexBuffer*> indexBuffers;		// ���b�V���̐����̃C���f�b�N�X�o�b�t�@

bool Scene::Init()
{
	// �C���|�[�g�ɕK�v�ȃp�����[�^�\����
	ImportSettings importSetting = 
	{
		modelFile,
		meshes,
		false,
		true,	// �A���V�A�̓e�N�X�`��V���W�����]���Ă邽��true
	};

	AssimpLoader loader;
	if (!loader.Load(importSetting))
	{
		return false;
	}
	
	// ���b�V���̐��������_�o�b�t�@��p�ӂ���
	vertexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto size = sizeof(Vertex) * meshes[i].Vertices.size();
		auto stride = sizeof(Vertex);
		auto vertices = meshes[i].Vertices.data();
		auto pVB = new VertexBuffer(size, stride, vertices);
		if (!pVB->IsValid())
		{
			printf("���_�o�b�t�@�̐����Ɏ��s\n");
			return false;
		}
		vertexBuffers.push_back(pVB);
	}

	// ���b�V���̐������C���f�b�N�X�o�b�t�@��p�ӂ���
	indexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto size = sizeof(uint32_t) * meshes[i].Indices.size();
		auto indices = meshes[i].Indices.data();
		auto pIB = new IndexBuffer(size, indices);
		
		if (!pIB->IsValid())
		{
			printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
			return false;
		}
		indexBuffers.push_back(pIB);
	}

	// ���_�̈ʒu
	auto eyePos = DirectX::XMVectorSet(0.0f, 120.0f, 75.0f, 0.0f); 
	// �����_�̍��W
	auto targetPos = DirectX::XMVectorSet(0.0f, 120.0f, 0.0f, 0.0f);
	// �������\���x�N�g��
	auto upward = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); 
	// ����p
	auto fov = DirectX::XMConvertToRadians(60.0); 
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
	
}

void Scene::Draw()
{
	// ���݂̃t���[���ԍ����擾
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// �R�}���h���X�g
	auto commandList = g_DrawBase->CommandList();

	//�@���b�V���̐������C���f�b�N�X���̕`����s��
	for (size_t i = 0; i < meshes.size(); i++)
	{
		// ���b�V���ɑΉ����钸�_�o�b�t�@
		auto vbView = vertexBuffers[i]->View();	
		// ���b�V���ɑΉ����钸�_�̏��ԃo�b�t�@
		auto ibView = indexBuffers[i]->View();

		commandList->SetGraphicsRootSignature(rootSignature->Get());
		commandList->SetPipelineState(pipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(
			0, constantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);
		
		// �C���f�b�N�X�̐����`��
		commandList->DrawIndexedInstanced(meshes[i].Indices.size(), 1, 0, 0, 0);
	}
}
