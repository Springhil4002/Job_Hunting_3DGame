#include "SeaMesh.h"
#include "Debug_New.h"

using namespace DirectX;

bool SeaMesh::Init(Camera* _camera)
{
	m_Camera = _camera;

	Mesh mesh = CreateMesh();

	auto vertexSize = sizeof(Vertex) * std::size(mesh.Vertices);
	auto vertexStride = sizeof(Vertex);
	m_pVertexBuffer = std::make_unique<VertexBuffer>(vertexSize, vertexStride, mesh.Vertices.data());
	if (!m_pVertexBuffer->IsValid())
	{
		printf("SeaMesh:頂点バッファ生成失敗\n");
		return false;
	}

	auto indexSize = sizeof(uint32_t) * std::size(mesh.Indices);
	m_pIndexBuffer = std::make_unique<IndexBuffer>(indexSize, mesh.Indices.data());
	if (!m_pIndexBuffer->IsValid())
	{
		printf("SeaMesh:インデックスバッファ生成失敗\n");
		return false;
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("SeaMesh:コンスタントバッファ生成失敗\n");
			return false;
		}

		// カメラの初期化
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = XMMatrixIdentity();
		ptr->view = m_Camera->GetViewMatrix();
		ptr->proj = m_Camera->GetProjMatrix();

		XMVECTOR camPosVec = m_Camera->GetPos();
		XMFLOAT3 camPos;
		XMStoreFloat3(&camPos, camPosVec);
		ptr->cameraPos = XMFLOAT3(camPos.x, camPos.y, camPos.z);
	}

	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	auto cubeTex = TextureManager::Instance().GetCubeMap(L"Assets/Texture/SkyDome.dds");
	if (!cubeTex)
	{
		printf("SeaMesh:キューブマップ読み込み失敗\n");
		return false;
	}
	m_pSkyCubeTexHandle = m_pDescriptorHeap->Register(cubeTex.get());

	auto normalTex = TextureManager::Instance().GetTexture(L"Assets/Texture/SeaMesh_Normal.png");
	if (!normalTex)
	{
		printf("SeaMesh:ノーマルマップ読み込み失敗\n");
		return false;
	}
	m_pNormalTexHandle = m_pDescriptorHeap->Register(normalTex.get());

	auto normalTex2 = TextureManager::Instance().GetTexture(L"Assets/Texture/SeaMesh_Normal_2.png");
	if (!normalTex2)
	{
		printf("SeaMesh:ノーマルマップ2読み込み失敗\n");
		return false;
	}
	m_pNormalTexHandle2 = m_pDescriptorHeap->Register(normalTex2.get());

	auto& rootManager = RootSignatureManager::GetInstance();
	m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_SEAMESH);
	if (!m_pRootSignature->IsValid())
	{
		printf("SeaMesh:ルートシグネチャ生成失敗\n");
		return false;
	}

	auto& psoManager = PipelineState_Manager::GetInstance();
	m_pPipelineState = psoManager.GetPSO_General(PSO_Type::PSO_TYPE_SEAMESH);
	if (!m_pPipelineState->IsValid())
	{
		// 頂点レイアウトの設定
		m_pPipelineState->SetInputLayout(Vertex::InputLayout);
		// ルートシグネチャの設定
		m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
#ifdef _DEBUG	// DEBUG
		m_pPipelineState->SetVS(L"../x64/Debug/VS_SeaMesh.cso");
		m_pPipelineState->SetPS(L"../x64/Debug/PS_SeaMesh.cso");
#else			// Release
		m_pPipelineState->SetVS(L"../x64/Release/VS_SeaMesh.cso");
		m_pPipelineState->SetPS(L"../x64/Release/PS_SeaMesh.cso");
#endif 
		// パイプラインステート作成
		m_pPipelineState->Create();
	}
	if (!m_pPipelineState->IsValid())
	{
		printf("SeaMesh:パイプラインステート生成失敗\n");
		return false;
	}

	printf("SeaMesh:初期化処理に成功\n\n");
	return true;
}

void SeaMesh::Update()
{
	m_Time += 0.016f;
	Update_Transform();
	Update_CameraMatrix();
}

void SeaMesh::Draw()
{
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto cmdList = g_DrawBase->CommandList();
	auto vbView = m_pVertexBuffer->View();
	auto ibView = m_pIndexBuffer->View();
	auto Heap = m_pDescriptorHeap->GetHeap();

	cmdList->SetGraphicsRootSignature(m_pRootSignature->Get());
	cmdList->SetPipelineState(m_pPipelineState->Get());
	cmdList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[currentIndex]->GetAddress());
	
	cmdList->SetDescriptorHeaps(1, &Heap);
	cmdList->SetGraphicsRootDescriptorTable(1, m_pSkyCubeTexHandle->handleGPU);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
void SeaMesh::Uninit()
{
	m_Camera = nullptr;
	m_pVertexBuffer.reset();
	m_pIndexBuffer.reset();
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pSkyCubeTexHandle.reset();
	m_pRootSignature.reset();
}

Mesh SeaMesh::CreateMesh()
{
	Mesh mesh;

	mesh.Vertices.resize(4);
	
	mesh.Vertices[0] = { 
		XMFLOAT3(-10.0f, 0.0f, 10.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f,0.0f,0.0f,1.0f) 
	};

	mesh.Vertices[1] = { 
		XMFLOAT3(10.0f, 0.0f, 10.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(0.0f,1.0f,0.0f,1.0f) 
	};

	mesh.Vertices[2] = { 
		XMFLOAT3(-10.0f, 0.0f,-10.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(0.0f,0.0f,1.0f,1.0f) 
	};

	mesh.Vertices[3] = { 
		XMFLOAT3(10.0f, 0.0f,-10.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(0.0f,0.0f,0.0f,1.0f) 
	};
	
	mesh.Indices.resize(6);
	mesh.Indices[0] = 0;
	mesh.Indices[1] = 1;
	mesh.Indices[2] = 2;

	mesh.Indices[3] = 1;
	mesh.Indices[4] = 3;
	mesh.Indices[5] = 2;

	return mesh;
}

void SeaMesh::Update_Transform()
{
	// カメラの更新処理
	auto pos = GetPos();
	auto rota = GetRota();
	auto scale = GetScale();

	m_worldMatrix =
		DirectX::XMMatrixScalingFromVector(scale) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(rota) *
		DirectX::XMMatrixTranslationFromVector(pos);
}

void SeaMesh::Update_CameraMatrix()
{
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	ptr->world = m_worldMatrix;
	ptr->view = m_Camera->GetViewMatrix();
	ptr->proj = m_Camera->GetProjMatrix();
	ptr->time = m_Time;

	// カメラ位置を毎フレーム更新
	XMVECTOR camPosVec = m_Camera->GetPos();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, camPosVec);
	ptr->cameraPos = camPos;
}

std::unique_ptr<Object> SeaMesh::clone() const
{
	auto newObj = std::make_unique<SeaMesh>();
	return newObj;
}