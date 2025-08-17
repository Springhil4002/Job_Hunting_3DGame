#include "WaterMesh.h"

using namespace DirectX;
// 共通テクスチャハンドル
DescriptorHandle* WaterMesh::s_pSharedTexHandle = nullptr;

Object* WaterMesh::clone() const
{
	return new WaterMesh(*this);
}

Mesh WaterMesh::CreateGridMesh()
{
	Mesh mesh;
	mesh.Vertices.resize((m_GridX + 1) * (m_GridZ + 1));
	mesh.Indices.reserve(m_GridX * m_GridZ * 6);

	float halfSize = m_GridSize * 0.5f;
	float stepX = m_GridSize / m_GridX;
	float stepZ = m_GridSize / m_GridZ;
	
	for (int z = 0; z <= m_GridZ; ++z)
	{
		for (int x = 0; x <= m_GridX; ++x)
		{
			float px = x * stepX - halfSize;
			float pz = z * stepZ - halfSize;
			float u = (float)x / m_GridX;
			float v = (float)z / m_GridZ;

			int index = z * (m_GridX + 1) + x;
			mesh.Vertices[index] = {
				XMFLOAT3(px,0.0f,pz),
				XMFLOAT3(0.0f,1.0f,0.0f),
				XMFLOAT2(u,v),
				XMFLOAT3(1.0f,0.0f,0.0f),
				XMFLOAT4(0.3f,0.8f,1.0f,1.0f)	// 水色
			};
		}
	}
	
	for (int z = 0; z < m_GridZ; ++z)
	{
		for (int x = 0; x < m_GridX; ++x)
		{
			int i0 = z * (m_GridX + 1) + x;
			int i1 = i0 + 1;
			int i2 = i0 + m_GridX + 1;
			int i3 = i2 + 1;

			mesh.Indices.push_back(i0);
			mesh.Indices.push_back(i1);
			mesh.Indices.push_back(i3);

			mesh.Indices.push_back(i0);
			mesh.Indices.push_back(i3);
			mesh.Indices.push_back(i2);
		}
	}

	m_IndexCount = static_cast<UINT>(mesh.Indices.size());
	return mesh;
}

bool WaterMesh::Init(Camera* _camera)
{
	m_Camera = _camera;
	
	auto mesh = CreateGridMesh();
	auto vertexSize = sizeof(Vertex) * std::size(mesh.Vertices);
	auto vertexStride = sizeof(Vertex);
	m_pVertexBuffer = new VertexBuffer(vertexSize, vertexStride, mesh.Vertices.data());
	if (!m_pVertexBuffer->IsValid())
	{
		printf("水面メッシュ:頂点バッファ生成失敗\n");
		return false;
	}

	auto indexSize = sizeof(uint32_t) * std::size(mesh.Indices);
	m_pIndexBuffer = new IndexBuffer(indexSize, mesh.Indices.data());
	if (!m_pIndexBuffer->IsValid())
	{
		printf("水面メッシュ:インデックスバッファ生成失敗\n");
		return false;
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("水面メッシュ:コンスタントバッファ生成失敗\n");
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

	m_pWaveBuffer = new ConstantBuffer(sizeof(GerstnerParams));
	if (!m_pWaveBuffer->IsValid())
	{
		printf("水面メッシュ:波用コンスタントバッファ生成失敗\n");
		return false;
	}

	// 4つの波のパラメータ設定
	m_waveParams.amplitude[0]	= { 0.3f, 0,0,0 };
	m_waveParams.direction[0]	= { 1.0f, 0.2f, 0,0 };
	m_waveParams.waveLength[0]	= { 8.0f, 0,0,0 };
	m_waveParams.speed[0]		= { 0.5f, 0,0,0 };

	m_waveParams.amplitude[1]	= { 0.2f, 0,0,0 };
	m_waveParams.direction[1]	= { -0.7f,1.0f, 0,0 };
	m_waveParams.waveLength[1]	= { 7.0f, 0,0,0 };
	m_waveParams.speed[1]		= { 0.4f, 0,0,0 };

	m_waveParams.amplitude[2]	= { 0.15f, 0,0,0 };
	m_waveParams.direction[2]	= { 0.5f, -1.0f, 0,0 };
	m_waveParams.waveLength[2]	= { 6.0f, 0,0,0 };
	m_waveParams.speed[2]		= { 0.3f, 0,0,0 };

	m_waveParams.amplitude[3]	= { 0.1f, 0,0,0 };
	m_waveParams.direction[3]	= { -1.0f, -0.3f, 0,0 };
	m_waveParams.waveLength[3]	= { 5.0f, 0,0,0 };
	m_waveParams.speed[3]		= { 0.2f, 0,0,0 };

	// バッファにコピー
	std::memcpy(m_pWaveBuffer->GetPtr(), &m_waveParams, sizeof(GerstnerParams));

	m_pLightBuffer = new ConstantBuffer(sizeof(LightPalams));
	if (!m_pLightBuffer->IsValid())
	{
		printf("水面メッシュ:ライト用コンスタントバッファ生成失敗\n");
		return false;
	}

	// カメラ方向と反対方向にライト方向を設定
	XMVECTOR eye = m_Camera->GetPos();
	XMVECTOR target = m_Camera->GetTarget();
	XMVECTOR camDir = XMVector3Normalize(XMVectorSubtract(target, eye));

	XMVECTOR lightDirVec = XMVectorScale(camDir, -1.0f);
	XMFLOAT3 lightDir;
	XMStoreFloat3(&lightDir, lightDirVec);

	LightPalams lightParams;
	lightParams.lightDir = lightDir;
	lightParams.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// バッファにコピー
	std::memcpy(m_pLightBuffer->GetPtr(), &lightParams, sizeof(LightPalams));

	// ディスクリプタヒープ
	m_pDescriptorHeap = new DescriptorHeap();

	auto cubeTex = TextureManager::Instance().LoadCubeMap(L"Assets/Texture/SkyDome.dds");
	if (!cubeTex)
	{
		printf("水面メッシュ:キューブマップ読み込み失敗\n");
		return false;
	}
	m_pSkyCubeTexHandle = m_pDescriptorHeap->Register(cubeTex.get());

	m_pRootSignature = new RootSignature_WaterMesh();
	if (!m_pRootSignature->IsValid())
	{
		printf("水面メッシュ:ルートシグネチャ生成失敗\n");
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
		printf("水面メッシュ:パイプラインステートの生成に失敗\n");
		return false;
	}

	return true;
}

void WaterMesh::Update()
{
	// 時間更新
	g_Time += 0.016f;	
	m_WaveTime += 0.016f;
	Update_WaterWave(m_WaveTime);
	Update_Transform();
	Update_CameraMatrix();
	Update_Light();
}

void WaterMesh::Draw()
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
	cmdList->SetGraphicsRootConstantBufferView(1, m_pWaveBuffer->GetAddress());
	cmdList->SetGraphicsRootConstantBufferView(2, m_pLightBuffer->GetAddress());

	// ディスクリプタヒープをセット
	cmdList->SetDescriptorHeaps(1, &Heap);
	// キューブマップをセット
	cmdList->SetGraphicsRootDescriptorTable(3, m_pSkyCubeTexHandle->handleGPU);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}

void WaterMesh::Uninit()
{
}

void WaterMesh::Update_Transform()
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

void WaterMesh::Update_CameraMatrix()
{
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	auto ptr = m_pConstantBuffer[currentIndex]->GetPtr<Matrix>();
	ptr->world = m_worldMatrix;
	ptr->view = m_Camera->GetViewMatrix();
	ptr->proj = m_Camera->GetProjMatrix();
	ptr->time = g_Time;

	// カメラ位置を毎フレーム更新
	XMVECTOR camPosVec = m_Camera->GetPos();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, camPosVec);
	ptr->cameraPos = camPos;
}

void WaterMesh::Update_WaterWave(float _waveTime)
{
	if (m_WaveTime >= 3.0f)
	{
		for (int i = 0; i < 4; ++i)
		{
			float amp = GetRandomAmplitude(0.1f, 1.0f);
			m_waveParams.amplitude[i] = { amp,0,0,0 };
		}
	}

	// バッファに変更内容を反映
	std::memcpy(m_pWaveBuffer->GetPtr(), &m_waveParams, sizeof(GerstnerParams));
	// 時間のリセット
	m_WaveTime = 0.0f;
}

void WaterMesh::Update_Light()
{
	// カメラの前方向ベクトルを取得
	XMVECTOR camDir = m_Camera->GetForward();

	// カメラと向かい合うようにライト方向を求める
	XMVECTOR lightDirVec = XMVectorScale(camDir, -1.0f);
	XMFLOAT3 lightDir;
	XMStoreFloat3(&lightDir, lightDirVec);

	LightPalams lightParams;
	lightParams.lightDir = lightDir;
	lightParams.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


	// 定数バッファに更新
	std::memcpy(m_pLightBuffer->GetPtr(), &lightParams, sizeof(LightPalams));
}

float WaterMesh::GetRandomAmplitude(float _min, float _max)
{
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(_min, _max);
	return dist(mt);
}

float WaterMesh::GetWaveHeight(float _x, float _z, float _time)
{
	float height = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		XMFLOAT2 dir = XMFLOAT2(m_waveParams.direction[i].x, m_waveParams.direction[i].y);
		float len = m_waveParams.waveLength[i].x;
		float amp = m_waveParams.amplitude[i].x;
		float speed = m_waveParams.speed[i].x;

		float k = XM_2PI / len;
		float dot = _x * dir.x + _z * dir.y;
		float phase = k * dot + _time * speed;
		height += amp * sinf(phase);
	}
	return height;
}