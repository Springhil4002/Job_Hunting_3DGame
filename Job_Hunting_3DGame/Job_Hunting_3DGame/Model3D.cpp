#include "Model3D.h"

Model3D::Model3D()
{

}

std::wstring Model3D::ReplaceExtension(const std::wstring& _origin, const char* _ext)
{
	if (_origin.empty()) return L"";
	std::filesystem::path path = _origin.c_str();
	return path.replace_extension(_ext).wstring();
}

Object* Model3D::clone() const
{
	return new Model3D(*this);
}

bool Model3D::Init(Camera* _camera)
{
	m_camera = _camera;
	// �C���|�[�g�ɕK�v�ȃp�����[�^�ݒ�
	ImportSettings importSetting = {
		m_pModelFile,
		m_meshes,
		false,
		true	// �A���V�A�̓e�N�X�`��V���W�����]���Ă邽��true	
	};
	
	// ���f�����[�_�[
	AssimpLoader loader;

	// ���f�������[�h
	if (!loader.Load(importSetting))
	{
		return false;
	}

	// ���b�V���̐��������_�o�b�t�@��p�ӂ���
	m_pVertexBuffers.reserve(m_meshes.size());
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		auto size = sizeof(Vertex) * m_meshes[i].Vertices.size();
		auto stride = sizeof(Vertex);
		auto vertices = m_meshes[i].Vertices.data();
		auto pVB = new VertexBuffer(size, stride, vertices);
		if (!pVB->IsValid())
		{
			printf("���_�o�b�t�@�̐����Ɏ��s\n");
			return false;
		}
		m_pVertexBuffers.push_back(pVB);
	}

	// ���b�V���̐������C���f�b�N�X�o�b�t�@��p�ӂ���
	m_pIndexBuffers.reserve(m_meshes.size());
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		auto size = sizeof(uint32_t) * m_meshes[i].Indices.size();
		auto indices = m_meshes[i].Indices.data();
		auto pIB = new IndexBuffer(size, indices);

		if (!pIB->IsValid())
		{
			printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
			return false;
		}
		m_pIndexBuffers.push_back(pIB);
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid()) 
		{
			return false;
		}
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();
	}

	// �f�B�X�N���v�^�q�[�v�̐���
	m_pDescriptorHeap = new DescriptorHeap();

	// �}�e���A���̓ǂݍ���
	m_pMaterialHandles.clear();
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		if (m_meshes[i].DiffuseMap.empty()) continue;

		auto texPath = ReplaceExtension(m_meshes[i].DiffuseMap, "tga");
		auto mainTex = Texture2D::Get(texPath);
		if (!mainTex)
		{
			OutputDebugStringW((L"�e�N�X�`�����ǂݍ��߂܂���ł���: " + texPath + L"\n").c_str());
			continue;
		}
		if (!m_pDescriptorHeap)
		{
			OutputDebugStringA("descriptorHeap ������������Ă��܂���B\n");
			continue;
		}
		auto handle = m_pDescriptorHeap->Register(mainTex);
		m_pMaterialHandles.push_back(handle);
	}

	m_pRootSignature = new RootSignature();
	if (!m_pRootSignature->IsValid())
	{
		printf("���[�g�V�O�l�`���̐����Ɏ��s\n");
		return false;
	}

	// �p�C�v���C���X�e�[�g�̃C���X�^���X����
	m_pPipelineState = new PipelineState();
	// ���_���C�A�E�g�̐ݒ�
	m_pPipelineState->SetInputLayout(Vertex::InputLayout);
	// ���[�g�V�O�l�`���̐ݒ�
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());

#ifdef _DEBUG	// DEBUG
	// VS��ݒ�
	m_pPipelineState->SetVS(L"../x64/Debug/VS_Simple.cso");
	// PS��ݒ�
	m_pPipelineState->SetPS(L"../x64/Debug/PS_Simple.cso");
#else			// Release
	// VS��ݒ�
	m_pPipelineState->SetVS(L"../x64/Release/VS_Simple.cso");
	// PS��ݒ�
	m_pPipelineState->SetPS(L"../x64/Release/PS_Simple.cso");
#endif 

	// �p�C�v���C���X�e�[�g�쐬
	m_pPipelineState->Create();

	if (!m_pPipelineState->IsValid())
	{
		printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s\n");
		return false;
	}

	printf("���f��:Alicia�̏����������ɐ���\n");
	return true;
}

void Model3D::Update()
{
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
}

void Model3D::Draw()
{
	// ���݂̃t���[���ԍ����擾
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// �R�}���h���X�g
	auto commandList = g_DrawBase->CommandList();
	// �f�B�X�N���v�^�q�[�v
	auto materialHeap = m_pDescriptorHeap->GetHeap();

	//�@���b�V���̐������C���f�b�N�X���̕`����s��
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		// ���b�V���ɑΉ����钸�_�o�b�t�@
		auto vbView = m_pVertexBuffers[i]->View();
		// ���b�V���ɑΉ����钸�_�̏��ԃo�b�t�@
		auto ibView = m_pIndexBuffers[i]->View();

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());
		commandList->SetPipelineState(m_pPipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(
			0, m_pConstantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);

		// �g�p����f�B�X�N���v�^�q�[�v���Z�b�g
		commandList->SetDescriptorHeaps(1, &materialHeap);
		// ���b�V���ɑΉ�����f�B�X�N���v�^�e�[�u�����Z�b�g
		commandList->SetGraphicsRootDescriptorTable(1, m_pMaterialHandles[i]->handleGPU);

		// �C���f�b�N�X�̐����`��
		commandList->DrawIndexedInstanced(m_meshes[i].Indices.size(), 1, 0, 0, 0);
	}
}

void Model3D::Uninit()
{

}
