#include "WaterMesh.h"

using namespace DirectX;
// ���ʃe�N�X�`���n���h��
DescriptorHandle* WaterMesh::s_pSharedTexHandle = nullptr;

Object* WaterMesh::clone() const
{
	return new WaterMesh(*this);
}

Mesh WaterMesh::CreateQuad(int _gridX,int _gridY,int _gridSize)
{
	float uvStep = 1.0f / _gridSize;
	float u0 = uvStep * _gridX;
	float v0 = uvStep * _gridY;
	float u1 = u0 + uvStep;
	float v1 = v0 + uvStep;

	Mesh mesh;
	mesh.Vertices.resize(4);
	XMFLOAT3 normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 tangent	= XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT4 color		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	
	mesh.Vertices[0] = { XMFLOAT3(-1.0f, 0.0f, 1.0f), normal, XMFLOAT2(u0, v0), tangent, color };
	mesh.Vertices[1] = { XMFLOAT3(1.0f, 0.0f, 1.0f), normal, XMFLOAT2(u1, v0), tangent, color };
	mesh.Vertices[2] = { XMFLOAT3(1.0f, 0.0f, -1.0f), normal, XMFLOAT2(u1, v1), tangent, color };
	mesh.Vertices[3] = { XMFLOAT3(-1.0f, 0.0f, -1.0f), normal, XMFLOAT2(u0, v1), tangent, color };

	mesh.Indices = {
		0,1,2,
		0,2,3
	};
	
	return mesh;
}

bool WaterMesh::Init(Camera* _camera,int _gridX,int _gridY,int _gridSize)
{
	m_camera = _camera;
	
	auto mesh = CreateQuad(_gridX, _gridY, _gridSize);
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

		XMVECTOR camPosVec = m_camera->GetPos();
		XMFLOAT3 camPos;
		XMStoreFloat3(&camPos, camPosVec);
		ptr->cameraPos = XMFLOAT3(camPos.x, camPos.y, camPos.z);
	}

	m_pWaveBuffer = new ConstantBuffer(sizeof(GerstnerParams));
	if (!m_pWaveBuffer->IsValid())
	{
		printf("���ʃ��b�V��:�g�p�R���X�^���g�o�b�t�@�������s\n");
		return false;
	}

	// 4�̔g�̃p�����[�^�ݒ�
	m_waveParams.amplitude[0]	= { 0.3f, 0,0,0 };
	m_waveParams.direction[0]	= { 1.0f,  0.2f, 0,0 };
	m_waveParams.waveLength[0]	= { 6.0f, 0,0,0 };
	m_waveParams.speed[0]		= { 1.0f, 0,0,0 };

	m_waveParams.amplitude[1]	= { 0.2f, 0,0,0 };
	m_waveParams.direction[1]	= { -0.7f,  1.0f, 0,0 };
	m_waveParams.waveLength[1]	= { 5.0f, 0,0,0 };
	m_waveParams.speed[1]		= { 0.8f, 0,0,0 };

	m_waveParams.amplitude[2]	= { 0.15f, 0,0,0 };
	m_waveParams.direction[2]	= { 0.5f, -1.0f, 0,0 };
	m_waveParams.waveLength[2]	= { 4.5f, 0,0,0 };
	m_waveParams.speed[2]		= { 1.2f, 0,0,0 };

	m_waveParams.amplitude[3]	= { 0.1f, 0,0,0 };
	m_waveParams.direction[3]	= { -1.0f, -0.3f, 0,0 };
	m_waveParams.waveLength[3]	= { 3.0f, 0,0,0 };
	m_waveParams.speed[3]		= { 0.6f, 0,0,0 };

	// �o�b�t�@�ɃR�s�[
	std::memcpy(m_pWaveBuffer->GetPtr(), &m_waveParams, sizeof(GerstnerParams));

	m_pLightBuffer = new ConstantBuffer(sizeof(LightPalams));
	if (!m_pLightBuffer->IsValid())
	{
		printf("���ʃ��b�V��:���C�g�p�R���X�^���g�o�b�t�@�������s\n");
		return false;
	}
	// ���C�g�̏����l��ݒ�
	LightPalams lightParams;
	// ���C�g�̕����ݒ�
	lightParams.lightDir = XMFLOAT3(0.0f, -1.0f, 0.0f);	
	// ���C�g�̃J���[�ݒ�
	lightParams.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	
	
	// �o�b�t�@�ɃR�s�[
	std::memcpy(m_pLightBuffer->GetPtr(), &lightParams, sizeof(LightPalams));

	// �f�B�X�N���v�^�q�[�v
	m_pDescriptorHeap = new DescriptorHeap();
	
	if (s_pSharedTexHandle == nullptr)
	{
		auto tex = TextureManager::Instance().LoadTexture(L"Assets/Texture/WaterMesh.png");
		s_pSharedTexHandle = m_pDescriptorHeap->Register(tex.get());
	}
	m_pTexHandle = s_pSharedTexHandle;

	m_pRootSignature = new RootSignature_WaterMesh();
	if (!m_pRootSignature->IsValid())
	{
		printf("���ʃ��b�V��:���[�g�V�O�l�`���������s\n");
		return false;
	}
	
	m_pPipelineState = new PipelineState_WaterMesh();
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

	return true;
}

void WaterMesh::Update()
{
	// ���ԍX�V
	g_time += 0.016f;	
	m_waveTime += 0.016f;

	Update_WaterWave(m_waveTime);

	Update_Transform();
	Update_CameraMatrix();
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
	// �f�B�X�N���v�^�q�[�v
	auto Heap = m_pDescriptorHeap->GetHeap();

	// ���[�g�V�O�l�`�����Z�b�g
	cmdList->SetGraphicsRootSignature(m_pRootSignature->Get());
	// �p�C�v���C���X�e�[�g���Z�b�g
	cmdList->SetPipelineState(m_pPipelineState->Get());
	// �萔�o�b�t�@���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[currentIndex]->GetAddress());
	cmdList->SetGraphicsRootConstantBufferView(1, m_pWaveBuffer->GetAddress());
	cmdList->SetGraphicsRootConstantBufferView(2, m_pLightBuffer->GetAddress());

	// �f�B�X�N���v�^�q�[�v���Z�b�g
	cmdList->SetDescriptorHeaps(1, &Heap);
	// �e�N�X�`�����Z�b�g
	cmdList->SetGraphicsRootDescriptorTable(3, m_pTexHandle->handleGPU);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void WaterMesh::Uninit()
{

}

void WaterMesh::Update_Transform()
{
	// �J�����̍X�V����
	auto pos = GetPos();
	auto rota = GetRota();
	auto scale = GetScale();

	m_worldMatrix =
		DirectX::XMMatrixScalingFromVector(scale) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(rota) *
		DirectX::XMMatrixTranslationFromVector(pos);
}

void WaterMesh::Update_CameraMatrix()
{
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	ptr->world = m_worldMatrix;
	ptr->view = m_camera->GetViewMatrix();
	ptr->proj = m_camera->GetProjMatrix();
	ptr->time = g_time;

	// �J�����ʒu�𖈃t���[���X�V
	XMVECTOR camPosVec = m_camera->GetPos();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, camPosVec);
	ptr->cameraPos = camPos;
}

void WaterMesh::Update_WaterWave(float _waveTime)
{
	if (m_waveTime >= 3.0f)
	{
		for (int i = 0; i < 4; ++i)
		{
			float amp = GetRandomAmplitude();
			m_waveParams.amplitude[i] = { amp,0,0,0 };
		}
	}

	// �o�b�t�@�ɕύX���e�𔽉f
	std::memcpy(m_pWaveBuffer->GetPtr(), &m_waveParams, sizeof(GerstnerParams));
	// ���Ԃ̃��Z�b�g
	m_waveTime = 0.0f;
}

float WaterMesh::GetRandomAmplitude(float _min, float _max)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(_min, _max);
	return dist(mt);
}