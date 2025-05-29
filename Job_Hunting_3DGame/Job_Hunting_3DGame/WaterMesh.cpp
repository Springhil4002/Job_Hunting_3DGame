#include "WaterMesh.h"

using namespace DirectX;

Object* WaterMesh::clone() const
{
	return new WaterMesh(*this);
}

Mesh WaterMesh::CreateQuad()
{
	XMFLOAT3 normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 tangent	= XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT4 color		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	
	Mesh mesh;
	mesh.Vertices.resize(4);

	mesh.Vertices[0] = { XMFLOAT3(-1.0f, 0.0f, 1.0f), normal, XMFLOAT2(0.0f, 0.0f), tangent, color };
	mesh.Vertices[1] = { XMFLOAT3(1.0f, 0.0f, 1.0f), normal, XMFLOAT2(1.0f, 0.0f), tangent, color };
	mesh.Vertices[2] = { XMFLOAT3(1.0f, 0.0f, -1.0f), normal, XMFLOAT2(1.0f, 1.0f), tangent, color };
	mesh.Vertices[3] = { XMFLOAT3(-1.0f, 0.0f, -1.0f), normal, XMFLOAT2(0.0f, 1.0f), tangent, color };

	mesh.Indices = {
		0,1,2,
		0,2,3
	};
	
	return mesh;
}

bool WaterMesh::Init(Camera* _camera)
{
	m_camera = _camera;
	
	auto mesh = CreateQuad();
	auto vertexSize = sizeof(Vertex) * std::size(mesh.Vertices);
	auto vertexStride = sizeof(Vertex);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, mesh.Vertices.data());
	if (!m_pVertexBuffer->IsValid())
	{
		printf("���ʃ��b�V��:���_�o�b�t�@�������s\n");
		return false;
	}

	auto indexSize = sizeof(uint32_t) * std::size(mesh.Indices);
	m_pIndexBuffer = new IndexBuffer(indexSize, mesh.Indices.data());
	if (!m_pIndexBuffer->IsValid())
	{
		printf("���ʃ��b�V��:�C���f�b�N�X�o�b�t�@�������s\n");
		return false;
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("���ʃ��b�V��:�R���X�^���g�o�b�t�@�������s\n");
			return false;
		}

		// �J�����̏�����
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();
	}

	m_pWaveBuffer = new ConstantBuffer(sizeof(GerstnerParams));
	if (!m_pWaveBuffer->IsValid())
	{
		printf("���ʃ��b�V��:�g�p�R���X�^���g�o�b�t�@�������s\n");
		return false;
	}

	// 4�̔g�̃p�����[�^�ݒ�
	m_waveParams.amplitude[0] = { 0.3f, 0,0,0 };
	m_waveParams.direction[0] = { 1.0f,  0.2f, 0,0 };
	m_waveParams.waveLength[0] = { 6.0f, 0,0,0 };
	m_waveParams.speed[0] = { 1.0f, 0,0,0 };

	m_waveParams.amplitude[1] = { 0.2f, 0,0,0 };
	m_waveParams.direction[1] = { -0.7f,  1.0f, 0,0 };
	m_waveParams.waveLength[1] = { 5.0f, 0,0,0 };
	m_waveParams.speed[1] = { 0.8f, 0,0,0 };

	m_waveParams.amplitude[2] = { 0.15f, 0,0,0 };
	m_waveParams.direction[2] = { 0.5f, -1.0f, 0,0 };
	m_waveParams.waveLength[2] = { 4.5f, 0,0,0 };
	m_waveParams.speed[2] = { 1.2f, 0,0,0 };

	m_waveParams.amplitude[3] = { 0.1f, 0,0,0 };
	m_waveParams.direction[3] = { -1.0f, -0.3f, 0,0 };
	m_waveParams.waveLength[3] = { 3.0f, 0,0,0 };
	m_waveParams.speed[3] = { 0.6f, 0,0,0 };

	// �o�b�t�@�ɃR�s�[
	std::memcpy(m_pWaveBuffer->GetPtr(), &m_waveParams, sizeof(GerstnerParams));

	m_pRootSignature = new RootSignature_WaterMesh();
	if (!m_pRootSignature->IsValid())
	{
		printf("���ʃ��b�V��:���[�g�V�O�l�`���������s\n");
		return false;
	}
	
	m_pPipelineState = new PipelineState();
	m_pPipelineState->SetInputLayout(Vertex::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
#ifdef _DEBUG
	m_pPipelineState->SetVS(L"../x64/Debug/VS_WaterMesh.cso");
	m_pPipelineState->SetPS(L"../x64/Debug/PS_WaterMesh.cso");
#else
	m_pPipelineState->SetVS(L"../x64/Release/VS_WaterMesh.cso");
	m_pPipelineState->SetPS(L"../x64/Release/PS_WaterMesh.cso");
#endif 
	m_pPipelineState->Create();
	if (!m_pPipelineState->IsValid())
	{
		printf("���ʃ��b�V��:�p�C�v���C���X�e�[�g�̐����Ɏ��s\n");
		return false;
	}

	printf("\n");
	return true;
}

void WaterMesh::Update()
{
	// ���t���[��(60FPS)
	g_time += 0.016f;	

	// �J�����̍X�V����
	auto pos = GetPos();
	auto rota = GetRota();
	auto scale = GetScale();

	auto world =
		DirectX::XMMatrixScalingFromVector(scale) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(rota) *
		DirectX::XMMatrixTranslationFromVector(pos);

	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	ptr->world = world;
	ptr->view = m_camera->GetViewMatrix();
	ptr->proj = m_camera->GetProjMatrix();
	ptr->time = g_time;
}

void WaterMesh::Draw()
{
	// ���݂̃t���[���ԍ��擾
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// �R�}���h���X�g
	auto cmdList = g_DrawBase->CommandList();
	// ���_�o�b�t�@�r���[
	auto vbView = m_pVertexBuffer->View();
	// �C���f�b�N�X�o�b�t�@�r���[
	auto ibView = m_pIndexBuffer->View(); 

	// ���[�g�V�O�l�`�����Z�b�g
	cmdList->SetGraphicsRootSignature(m_pRootSignature->Get());
	// �p�C�v���C���X�e�[�g���Z�b�g
	cmdList->SetPipelineState(m_pPipelineState->Get());
	// �萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[currentIndex]->GetAddress());
	cmdList->SetGraphicsRootConstantBufferView(1, m_pWaveBuffer->GetAddress());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void WaterMesh::Uninit()
{

}