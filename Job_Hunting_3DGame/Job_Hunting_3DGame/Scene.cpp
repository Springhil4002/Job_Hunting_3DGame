#include "Scene.h"
#include "DrawBase.h"
#include "App.h"
#include <d3dx12.h>
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"

Scene* g_Scene;
VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[DrawBase::FRAME_BUFFER_COUNT];

bool Scene::Init()
{
	// ���_�f�[�^
	Vertex vertices[3] = {};
	vertices[0].position=XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[1].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// ���_�o�b�t�@�S�̂̃T�C�Y���v�Z
	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	// 1���_�̃T�C�Y���擾
	auto vertexStride = sizeof(Vertex);
	// ���_�o�b�t�@�̃C���X�^���X����
	vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	
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

	if (!vertexBuffer->IsValid())
	{
		printf("���_�o�b�t�@�̐����Ɏ��s\n");
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

}
