#include "SkyBox.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> SkyBox::clone() const
{
	auto newObj = std::make_unique<SkyBox>();
	return newObj;
}

bool SkyBox::Init(Camera* _camera)
{
	m_camera = _camera;

	// メッシュ生成
	//CreateMesh(32, 64, 1000.0f);
	CreateCubeMesh();

	// 頂点バッファ生成
	auto vbSize = sizeof(SkyVertex) * vertices.size();
	auto vbStride = sizeof(SkyVertex);
	m_pVertexBuffer = std::make_unique<VertexBuffer>(vbSize, vbStride, vertices.data());
	if (!m_pVertexBuffer->IsValid())
	{
		printf("SkyBox:頂点バッファ生成失敗\n");
		return false;
	}

	// インデックスバッファ生成
	auto ibSize = sizeof(uint32_t) * indices.size();
	m_pIndexBuffer = std::make_unique<IndexBuffer>(ibSize, indices.data());
	if (!m_pIndexBuffer->IsValid())
	{
		printf("SkyBox:インデックスバッファ生成失敗\n");
		return false;
	}

	// コンスタントバッファ生成
	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("SkyBox:コンスタントバッファ生成失敗\n");
			return false;
		}

		// カメラの初期化
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();

		XMVECTOR camPosVec = m_camera->GetPos();
		XMFLOAT3 camPos;
		XMStoreFloat3(&camPos, camPosVec);
		ptr->cameraPos = camPos;
	}

	// ディスクリプタヒープ
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	// テクスチャの適用
	auto tex = TextureManager::Instance().GetCubeMap(L"Assets/Texture/SkyDome.dds");
	if(!tex)
	{
		printf("SkyBox:テクスチャの取得失敗\n");
		return false;
	}
	m_pCubeTexHandle = m_pDescriptorHeap->Register(tex.get());
	
	// ルートシグネチャ生成
	auto& rootManager = RootSignatureManager::GetInstance();
	m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_SKYBOX);
	if (!m_pRootSignature->IsValid())
	{
		printf("SkyBox:ルートシグネチャ生成失敗\n");
		return false;
	}

	// パイプラインステート生成
	auto& psoManager = PipelineState_Manager::GetInstance();
	m_pPipelineState = psoManager.GetPSO_SkyBox(PSO_Type::PSO_TYPE_SKYBOX);
	if (!m_pPipelineState->IsValid())
	{
		m_pPipelineState->SetInputLayout(SkyVertex::InputLayout);
		m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
#ifdef _DEBUG
		m_pPipelineState->SetVS(L"../x64/Debug/VS_SkyBox.cso");
		m_pPipelineState->SetPS(L"../x64/Debug/PS_SkyBox.cso");
#else
		m_pPipelineState->SetVS(L"../x64/Release/VS_SkyBox.cso");
		m_pPipelineState->SetPS(L"../x64/Release/PS_SkyBox.cso");
#endif 
		m_pPipelineState->Create();
	}

	if (!m_pPipelineState->IsValid())
	{
		printf("SkyBox:パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("SkyBox:初期化処理に成功\n\n");
	return true;
}

void SkyBox::Update()
{
	Update_Transform();
	Update_CameraMatrix();
}

void SkyBox::Draw()
{
	// 現在のフレーム番号取得
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// コマンドリスト
	auto cmdList = g_DrawBase->CommandList();
	// 頂点バッファビュー
	auto vbView = m_pVertexBuffer->View();
	// インデックスバッファビュー
	auto ibView = m_pIndexBuffer->View();
	// ディスクリプタヒープ
	auto Heap = m_pDescriptorHeap->GetHeap();

	// ルートシグネチャをセット
	cmdList->SetGraphicsRootSignature(m_pRootSignature->Get());
	// パイプラインステートをセット
	cmdList->SetPipelineState(m_pPipelineState->Get());
	// 定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, m_pConstantBuffer[currentIndex]->GetAddress());
	// ディスクリプタヒープをセット
	cmdList->SetDescriptorHeaps(1, &Heap);
	// テクスチャをセット
	cmdList->SetGraphicsRootDescriptorTable(1, m_pCubeTexHandle->handleGPU);
	
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(static_cast<UINT>(indices.size()), 1, 0, 0, 0);
}

void SkyBox::Uninit()
{
	m_camera = nullptr;
	m_pVertexBuffer.reset();
	m_pIndexBuffer.reset();
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pCubeTexHandle.reset();
}

void SkyBox::CreateCubeMesh()
{
	vertices.clear();
	indices.clear();
	
	// 立方体の頂点データ
	XMFLOAT3 cubeVertices[] =
	{
		{-1.0f,-1.0f,-1.0f},{1.0f,-1.0f,-1.0f},{1.0f,1.0f,-1.0f},{-1.0f,1.0f,-1.0f},
		{-1.0f,-1.0f, 1.0f},{1.0f,-1.0f, 1.0f},{1.0f,1.0f, 1.0f},{-1.0f,1.0f, 1.0f}
	};

	uint32_t cubeIndices[] =
	{
		0,1,2, 0,2,3,	// -Z
		4,6,5, 4,7,6,   // +Z
		4,5,1, 4,1,0,   // -Y
		3,2,6, 3,6,7,   // +Y
		1,5,6, 1,6,2,   // +X
		4,0,3, 4,3,7    // -X
	};

	for (auto& v : cubeVertices)
		vertices.push_back({ v,XMFLOAT2(0.0f, 0.0f) });

	indices.assign(cubeIndices, cubeIndices + _countof(cubeIndices));
}

void SkyBox::Update_Transform()
{
	// スカイボックスは回転・スケール不要、カメラ位置追従のみ
	m_worldMatrix = XMMatrixTranslationFromVector(m_camera->GetPos());
}

void SkyBox::Update_CameraMatrix()
{
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	ptr->world = m_worldMatrix;
	ptr->view = m_camera->GetViewMatrix();
	ptr->proj = m_camera->GetProjMatrix();

	// カメラ位置を毎フレーム更新
	XMVECTOR camPosVec = m_camera->GetPos();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, camPosVec);
	ptr->cameraPos = camPos;
}
