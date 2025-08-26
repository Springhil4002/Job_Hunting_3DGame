#include "SkyDomeMesh.h"

using namespace DirectX;

Object* SkyDomeMesh::clone() const
{
	return new SkyDomeMesh(*this);
}

bool SkyDomeMesh::Init(Camera* _camera)
{
	m_camera = _camera;

	// メッシュ生成
	CreateMesh(32, 64, 1000.0f);

	// 頂点バッファ生成
	auto vbSize = sizeof(SkyVertex) * vertices.size();
	auto vbStride = sizeof(SkyVertex);
	m_pVertexBuffer = new VertexBuffer(vbSize, vbStride, vertices.data());
	if (!m_pVertexBuffer->IsValid())
	{
		printf("スカイドーム:頂点バッファ生成失敗\n");
		return false;
	}

	// インデックスバッファ生成
	auto ibSize = sizeof(uint32_t) * indices.size();
	m_pIndexBuffer = new IndexBuffer(ibSize, indices.data());
	if (!m_pIndexBuffer->IsValid())
	{
		printf("スカイドーム:インデックスバッファ生成失敗\n");
		return false;
	}

	// コンスタントバッファ生成
	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("スカイドーム:コンスタントバッファ生成失敗\n");
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
		ptr->cameraPos = XMFLOAT3(camPos.x, camPos.y, camPos.z);
	}

	// ディスクリプタヒープ
	m_pDescriptorHeap = new DescriptorHeap();

	// テクスチャの適用
	if (m_pTexHandle == nullptr)
	{
		auto tex = TextureManager::Instance().LoadTexture(L"Assets/Texture/SkyDome.dds");
		m_pTexHandle = m_pDescriptorHeap->Register(tex.get());
	}
	
	// ルートシグネチャ生成
	m_pRootSignature = new RootSignature_SkyDomeMesh();
	if (!m_pRootSignature->IsValid())
	{
		printf("スカイドーム:ルートシグネチャ生成失敗\n");
		return false;
	}

	// パイプラインステート生成
	m_pPipelineState = new PipelineState_SkyDomeMesh();
	m_pPipelineState->SetInputLayout(SkyVertex::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
#ifdef _DEBUG
	m_pPipelineState->SetVS(L"../x64/Debug/VS_SkyDomeMesh.cso");
	m_pPipelineState->SetPS(L"../x64/Debug/PS_SkyDomeMesh.cso");
#else
	m_pPipelineState->SetVS(L"../x64/Release/VS_SkyDomeMesh.cso");
	m_pPipelineState->SetPS(L"../x64/Release/PS_SkyDomeMesh.cso");
#endif 
	m_pPipelineState->Create();
	if (!m_pPipelineState->IsValid())
	{
		printf("スカイドーム:パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("SkyDomeMesh:初期化処理に成功\n");
	return true;
}

void SkyDomeMesh::Update()
{
	Update_Transform();
	Update_CameraMatrix();
}

void SkyDomeMesh::Draw()
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
	cmdList->SetGraphicsRootDescriptorTable(1,m_pTexHandle->handleGPU);
	
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(static_cast<UINT>(indices.size()), 1, 0, 0, 0);
}

void SkyDomeMesh::Uninit()
{
}

void SkyDomeMesh::CreateMesh(int _slices, int _stacks, float _radius)
{
	vertices.clear();
	indices.clear();

	for (int stack = 0; stack <= _stacks; ++stack)
	{
		float phi = XM_PI * float(stack) / float(_stacks);
		for (int slice = 0; slice <= _slices; ++slice)
		{
			float theta = XM_2PI * float(slice) / float(_slices);
			
			float x = _radius * sinf(phi) * cosf(theta);
			float y = _radius * cosf(phi);
			float z = _radius * sinf(phi) * sinf(theta);

			float u = float(slice) / float(_slices);
			float v = float(stack) / float(_stacks);

			vertices.push_back({ XMFLOAT3(x,y,z),XMFLOAT2(u,v) });
		}
	}

	for (int stack = 0; stack < _stacks; ++stack) 
	{
		for (int slice = 0; slice < _slices; ++slice)
		{
			int cur = stack * (_slices + 1) + slice;
			int next = (stack + 1) * (_slices + 1) + slice;

			indices.push_back(cur);
			indices.push_back(next);
			indices.push_back(cur + 1);

			indices.push_back(cur + 1);
			indices.push_back(next);
			indices.push_back(next + 1);
		}
	}
	//printf("スカイドーム:頂点数:%zu\n", vertices.size());
	//printf("スカイドーム:インデックス数:%zu\n", indices.size());
}

void SkyDomeMesh::Update_Transform()
{
	// カメラの更新処理
	auto pos = GetPos();
	auto rota = GetRota();
	auto scale = GetScale();

	auto mat =
		DirectX::XMMatrixScalingFromVector(scale) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(rota) *
		DirectX::XMMatrixTranslationFromVector(pos);

	//m_worldMatrix = ;
}

void SkyDomeMesh::Update_CameraMatrix()
{
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	//ptr->world = ;
	ptr->world = XMMatrixTranslationFromVector(m_camera->GetPos());
	ptr->view = m_camera->GetViewMatrix();
	ptr->proj = m_camera->GetProjMatrix();

	// カメラ位置を毎フレーム更新
	XMVECTOR camPosVec = m_camera->GetPos();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, camPosVec);
	ptr->cameraPos = camPos;
}
