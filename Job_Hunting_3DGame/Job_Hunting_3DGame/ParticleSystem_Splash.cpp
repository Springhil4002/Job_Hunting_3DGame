#include "ParticleSystem_Splash.h"
#include "Debug_New.h"

using namespace DirectX;

ParticleSystem_Splash::ParticleSystem_Splash(Camera* _camera)
{
	Init(_camera);
}

ParticleSystem_Splash::~ParticleSystem_Splash()
{
	Uninit();
}

bool ParticleSystem_Splash::Init(Camera* _camera)
{
	if (!Init_Prop(_camera)) return false;
	if (!Init_Param()) return false;
	
	return true;
}

void ParticleSystem_Splash::Update(float _deltaTime)
{
	int activeCount = 0;
	for (int i = 0; i < m_Particles.size(); ++i)
	{
		// パーティクル取得
		Particle& p = m_Particles[i];
		// 非アクティブなら次
		if (!p.IsActive()) continue;
		// 経過時間更新
		p.elapsedTime += _deltaTime;
		// 寿命きてるなら停止して次
		if (p.elapsedTime > p.lifeTime)
		{
			p.particleState = Stop;
			continue;
		}

		XMVECTOR vel = p.velocity;
		vel = XMVectorSetY(vel, XMVectorGetY(vel) + p.gravity * _deltaTime);
		p.velocity = vel;

		p.position += p.velocity * _deltaTime;

		// 色補間
		float time = p.elapsedTime / p.lifeTime;
		p.color = p.LerpColor(p.startColor, p.endColor, time);

		// GPUへ送るデータ作成
		InstanceData& instData = m_InstanceData[activeCount];
		XMStoreFloat3(&instData.worldPos, p.position);
		instData.scale = 0.03f;
		instData.color = p.color;

		++activeCount;
	}

	// GPU転送
	if (activeCount > 0)
	{
		void* mapped = nullptr;
		HRESULT hr = m_InstanceBuffer->Map(0, nullptr, &mapped);
		if (SUCCEEDED(hr))
		{
			memcpy(mapped, m_InstanceData.data(), sizeof(InstanceData) * activeCount);
			m_InstanceBuffer->Unmap(0, nullptr);
		}
	}
	m_ActiveInstanceCount = activeCount;

	auto index = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[index]->GetPtr<Matrix>();
	ptr->view = m_Camera->GetViewMatrix();
	ptr->proj = m_Camera->GetProjMatrix();
}

void ParticleSystem_Splash::Draw()
{
	if (m_ActiveInstanceCount <= 0) return;
	// 現在のフレーム番号取得
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// コマンドリスト
	auto cmdList = g_DrawBase->CommandList();
	// インデックスバッファ
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
	// ディスクリプタテーブル
	cmdList->SetGraphicsRootDescriptorTable(1, m_pTexHandle->handleGPU);
	// 三角形
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	D3D12_VERTEX_BUFFER_VIEW vbViews[] = {
		m_pVertexBuffer->View(),	// 4頂点クアッド
		m_InstanceBufferView		// パーティクルインスタンス
	};
	cmdList->IASetVertexBuffers(0, 2, vbViews);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(6, m_ActiveInstanceCount, 0, 0, 0);
}

void ParticleSystem_Splash::Uninit()
{
	m_Camera = nullptr;
	m_pVertexBuffer.reset();
	m_pIndexBuffer.reset();
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pRootSignature.reset();
	m_pPipelineState.reset();
	m_pTexHandle.reset();
}

Mesh ParticleSystem_Splash::CreateQuad()
{
	Mesh mesh;
	mesh.Vertices.resize(4);
	mesh.Vertices[0] = { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.3f, 0.8f, 1.0f, 1.0f) };
	mesh.Vertices[1] = { XMFLOAT3(1.0f,  1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.3f, 0.8f, 1.0f, 1.0f) };
	mesh.Vertices[2] = { XMFLOAT3(-1.0f,-1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.3f, 0.8f, 1.0f, 1.0f) };
	mesh.Vertices[3] = { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.3f ,0.8f, 1.0f, 1.0f) };

	mesh.Indices = { 0, 1, 2, 1, 3, 2 };

	return mesh;
}

bool ParticleSystem_Splash::CreateInstanceBuffer()
{
	// インスタンスバッファ生成
	UINT bufferSize = sizeof(InstanceData) * m_MaxParticles;

	// デフォルトヒープを使用(頻繁な更新をするから)
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = bufferSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto hr = g_DrawBase->Device()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_InstanceBuffer)
	);

	if (FAILED(hr))
	{
		printf("水しぶきパーティクル:インスタンスバッファ生成失敗\n");
		return false;
	}

	// バッファビュー設定
	m_InstanceBufferView.BufferLocation = m_InstanceBuffer->GetGPUVirtualAddress();
	m_InstanceBufferView.SizeInBytes = bufferSize;
	m_InstanceBufferView.StrideInBytes = sizeof(InstanceData);

	return true;
}

bool ParticleSystem_Splash::Init_Prop(Camera* _camera)
{
	m_Camera = _camera;
	if (!m_Camera) return false;

	// パーティクル用、4頂点ポリゴン生成
	auto mesh = CreateQuad();
	auto vertexSize = sizeof(VertexInstance) * std::size(mesh.Vertices);
	auto vertexStride = sizeof(VertexInstance);

	m_pVertexBuffer = std::make_unique<VertexBuffer>(vertexSize, vertexStride, mesh.Vertices.data());
	if (!m_pVertexBuffer->IsValid())
	{
		printf("水しぶきパーティクル:頂点バッファ生成失敗\n");
		return false;
	}

	auto indexSize = sizeof(uint32_t) * std::size(mesh.Indices);
	m_pIndexBuffer = std::make_unique<IndexBuffer>(indexSize, mesh.Indices.data());
	if (!m_pIndexBuffer->IsValid())
	{
		printf("水しぶきパーティクル:インデックスバッファ生成失敗\n");
		return false;
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("水しぶきパーティクル:コンスタントバッファ生成失敗\n");
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

	// ディスクリプタヒープ
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	auto particleTex = TextureManager::Instance().LoadTexture(L"Assets/Texture/Particle_Splash.png");
	if (!particleTex)
	{
		printf("水しぶきパーティクル:画像読み込み失敗\n");
		return false;
	}
	m_pTexHandle = m_pDescriptorHeap->Register(particleTex.get());

	m_pRootSignature = std::make_unique<RootSignature_Splash>();
	if (!m_pRootSignature->IsValid())
	{
		printf("水しぶきパーティクル:ルートシグネチャ生成失敗\n");
		return false;
	}

	m_pPipelineState = std::make_unique<PipelineState_Splash>();
	m_pPipelineState->SetInputLayout(VertexInstance::InputLayout);
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
#ifdef _DEBUG
	m_pPipelineState->SetVS(L"../x64/Debug/VS_Splash.cso");
	m_pPipelineState->SetPS(L"../x64/Debug/PS_Splash.cso");
#else
	m_pPipelineState->SetVS(L"../x64/Release/VS_Splash.cso");
	m_pPipelineState->SetPS(L"../x64/Release/PS_Splash.cso");
#endif 
	m_pPipelineState->Create();
	if (!m_pPipelineState->IsValid())
	{
		printf("水しぶきパーティクル:パイプラインステート生成失敗\n");
		return false;
	}

	return true;
}

bool ParticleSystem_Splash::Init_Param()
{
	m_MaxParticles = 2000;
	m_Particles.clear();
	m_Particles.reserve(m_MaxParticles);
	m_InstanceData.resize(m_MaxParticles);

	// インスタンスバッファ生成
	if (!CreateInstanceBuffer()) return false;

	return true;
}