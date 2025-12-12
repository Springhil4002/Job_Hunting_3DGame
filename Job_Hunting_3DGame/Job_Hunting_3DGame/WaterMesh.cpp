#include "WaterMesh.h"
#include "Debug_New.h"
#include <algorithm>

using namespace DirectX;

std::unique_ptr<Object> WaterMesh::clone() const
{
	// 初回プロトタイプで生成時はInit()が呼ばれないため、
	// 使用するときはInit()を必ず呼び出すこと
	auto newObj = std::make_unique<WaterMesh>();
	newObj->g_Time = g_Time;
	newObj->m_WaveTime = m_WaveTime;
	newObj->m_GridX = m_GridX;
	newObj->m_GridZ = m_GridZ;
	newObj->m_GridSize = m_GridSize;
	return newObj;
}

Mesh WaterMesh::CreateGridMesh()
{
	Mesh mesh;
	mesh.Vertices.resize((m_GridX + 1) * (m_GridZ + 1));
	mesh.Indices.reserve(m_GridX * m_GridZ * 6);

	float halfSize = m_GridSize * 0.5f;
	float fGridX = static_cast<float>(m_GridX);
	float fGridZ = static_cast<float>(m_GridZ);
	float stepX = m_GridSize / fGridX;
	float stepZ = m_GridSize / fGridZ;
	
	for (int z = 0; z <= m_GridZ; ++z)
	{
		float fz = static_cast<float>(z);
		for (int x = 0; x <= m_GridX; ++x)
		{
			float fx = static_cast<float>(x);

			float px = fx * stepX - halfSize;
			float pz = fz * stepZ - halfSize;
			float u = fx / fGridX;
			float v = fz / fGridZ;

			int index = z * (m_GridX + 1) + x;
			mesh.Vertices[index] = {
				XMFLOAT3(px,0.0f,pz),
				XMFLOAT3(0.0f,1.0f,0.0f),
				XMFLOAT2(u,v),
				XMFLOAT3(1.0f,0.0f,0.0f),
				XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
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

	// 頂点データを移動
	m_Vertices = std::move(mesh.Vertices);
	mesh.Vertices.clear();

	return mesh;
}

bool WaterMesh::Init(Camera* _camera)
{
	m_Camera = _camera;
	
	if (!Init_SimulationResources()) return false;

	auto mesh = CreateGridMesh();
	auto vertexSize = sizeof(Vertex) * std::size(m_Vertices);
	auto vertexStride = sizeof(Vertex);
	m_pVertexBuffer = std::make_unique<VertexBuffer>(vertexSize, vertexStride, m_Vertices.data());
	if (!m_pVertexBuffer->IsValid())
	{
		printf("WaterMesh:頂点バッファ生成失敗\n");
		return false;
	}

	auto indexSize = sizeof(uint32_t) * std::size(mesh.Indices);
	m_pIndexBuffer = std::make_unique<IndexBuffer>(indexSize, mesh.Indices.data());
	if (!m_pIndexBuffer->IsValid())
	{
		printf("WaterMesh:インデックスバッファ生成失敗\n");
		return false;
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("WaterMesh:コンスタントバッファ生成失敗\n");
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

	m_pWaveBuffer = std::make_unique<ConstantBuffer>(sizeof(GerstnerParams));
	if (!m_pWaveBuffer->IsValid())
	{
		printf("WaterMesh:波用コンスタントバッファ生成失敗\n");
		return false;
	}

	// 4頂点の座標
	m_waveParams.amplitude[0]	= { 0.5f, 0,0,0 };
	m_waveParams.direction[0]	= { 1.0f, 0.2f, 0,0 };
	m_waveParams.waveLength[0]	= { 8.0f, 0,0,0 };
	m_waveParams.speed[0]		= { 0.5f, 0,0,0 };

	m_waveParams.amplitude[1]	= { 0.3f, 0,0,0 };
	m_waveParams.direction[1]	= { -0.7f,1.0f, 0,0 };
	m_waveParams.waveLength[1]	= { 7.0f, 0,0,0 };
	m_waveParams.speed[1]		= { 0.4f, 0,0,0 };

	m_waveParams.amplitude[2]	= { 0.25f, 0,0,0 };
	m_waveParams.direction[2]	= { 0.5f, -1.0f, 0,0 };
	m_waveParams.waveLength[2]	= { 6.0f, 0,0,0 };
	m_waveParams.speed[2]		= { 0.3f, 0,0,0 };

	m_waveParams.amplitude[3]	= { 0.2f, 0,0,0 };
	m_waveParams.direction[3]	= { -1.0f, -0.3f, 0,0 };
	m_waveParams.waveLength[3]	= { 5.0f, 0,0,0 };
	m_waveParams.speed[3]		= { 0.2f, 0,0,0 };

	// バッファにコピー
	std::memcpy(m_pWaveBuffer->GetPtr(), &m_waveParams, sizeof(GerstnerParams));

	m_pLightBuffer = std::make_unique<ConstantBuffer>(sizeof(LightPalams));
	if (!m_pLightBuffer->IsValid())
	{
		printf("WaterMesh:ライト用コンスタントバッファ生成失敗\n");
		return false;
	}
	
	LightPalams lightParams = {};
	lightParams.lightDir = XMFLOAT3(0.0f, -1.0f, 0.0f);
	lightParams.envStrength = 0.65f;
	lightParams.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// バッファにコピー
	std::memcpy(m_pLightBuffer->GetPtr(), &lightParams, sizeof(LightPalams));

	// WaterParamsコンスタントバッファ
	m_pWaterBuffer = std::make_unique<ConstantBuffer>(sizeof(WaterParams));
	if (!m_pWaterBuffer->IsValid())
	{
		printf("WaterMesh:WaterParams コンスタントバッファ生成失敗\n");
		return false;
	}

	m_waterParams.fbmScale = XMFLOAT2(0.02f, 0.02f);
	m_waterParams.fbmGain = 0.5f;
	m_waterParams.fbmLacunarity = 2.0f;
	m_waterParams.fbmOctaves = 6;
	m_waterParams.normalPerturb = 0.1f;
	m_waterParams.foamThreshold = 0.55f;
	m_waterParams.foamIntensity = 0.6f;
	m_waterParams.shallowColor = XMFLOAT4(0.03f, 0.10f, 0.18f, 1.0f);
	m_waterParams.deepColor = XMFLOAT4(0.005f, 0.02f, 0.05f, 1.0f);
	std::memcpy(m_pWaterBuffer->GetPtr(), &m_waterParams, sizeof(WaterParams));

	// ディスクリプタヒープ
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	auto cubeTex = TextureManager::Instance().GetCubeMap(L"Assets/Texture/SkyBox.dds");
	if (!cubeTex)
	{
		printf("WaterMesh:キューブマップ読み込み失敗\n");
		return false;
	}
	m_pSkyCubeTexHandle = m_pDescriptorHeap->Register(cubeTex.get());

	auto& rootManager = RootSignatureManager::GetInstance();
	m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_WATERMESH);
	if (!m_pRootSignature->IsValid())
	{
		printf("WaterMesh:ルートシグネチャ生成失敗\n");
		return false;
	}

	// マネージャー経由でパイプラインステートを取得
	auto& psoManager = PipelineState_Manager::GetInstance();
	m_pPipelineState = psoManager.GetPSO_General(PSO_Type::PSO_TYPE_WATERMESH);
	if(!m_pPipelineState->IsValid())
	{
		// 頂点レイアウトの設定
		m_pPipelineState->SetInputLayout(Vertex::InputLayout);
		// ルートシグネチャの設定
		m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
		// VS/PSの設定
#ifdef _DEBUG	// DEBUG
		m_pPipelineState->SetVS(L"../x64/Debug/VS_WaterMesh.cso");
		m_pPipelineState->SetPS(L"../x64/Debug/PS_WaterMesh.cso");
#else			// Release
		m_pPipelineState->SetVS(L"../x64/Release/VS_WaterMesh.cso");
		m_pPipelineState->SetPS(L"../x64/Release/PS_WaterMesh.cso");
#endif 
		// パイプラインステート作成
		m_pPipelineState->Create();
	}
	if (!m_pPipelineState->IsValid())
	{
		printf("WaterMesh:パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("WaterMesh:初期化処理に成功\n\n");
	return true;
}

void WaterMesh::Update()
{
	// 時間更新
	g_Time += 0.016f;
	m_WaveTime += 0.016f;

	ApplyPendingDrops();
	StepSimulation(0.016f);

	Update_Transform();
	Update_CameraMatrix();
	Update_Light();

	Update_VertexBuffer();
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
	cmdList->SetGraphicsRootConstantBufferView(3, m_pWaterBuffer->GetAddress());

	// ディスクリプタヒープをセット
	cmdList->SetDescriptorHeaps(1, &Heap);
	// キューブマップをセット
	cmdList->SetGraphicsRootDescriptorTable(4, m_pSkyCubeTexHandle->handleGPU);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}

void WaterMesh::Uninit()
{
	m_Camera = nullptr;
	m_pVertexBuffer.reset();
	m_pIndexBuffer.reset();
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	m_pWaveBuffer.reset();
	m_pLightBuffer.reset();
	m_pDescriptorHeap.reset();
	m_pSkyCubeTexHandle.reset();
}

void WaterMesh::Update_GridSize(float _newGridSize)
{
	// サイズを更新
	m_GridSize = _newGridSize;
	// メッシュ再生成
	auto mesh = CreateGridMesh();
	// 頂点バッファ再生成
	m_pVertexBuffer = std::make_unique<VertexBuffer>(
		sizeof(Vertex) * mesh.Vertices.size(),
		sizeof(Vertex),
		mesh.Vertices.data()
	);
	// インデックスバッファ再生成
	m_pIndexBuffer = std::make_unique<IndexBuffer>(
		sizeof(uint32_t) * mesh.Indices.size(),
		mesh.Indices.data()
	);
	// インデックス数更新
	m_IndexCount = static_cast<UINT>(mesh.Indices.size());
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

void WaterMesh::Update_Light()
{
	LightPalams lightParams;
	lightParams.lightDir = XMFLOAT3(0.0f, -1.0f, 0.0f);
	lightParams.envStrength = 0.6f;
	lightParams.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// 定数バッファに更新
	std::memcpy(m_pLightBuffer->GetPtr(), &lightParams, sizeof(LightPalams));
}

bool WaterMesh::Init_SimulationResources()
{
	const size_t total = static_cast<size_t>(m_GridX + 1) * 
						static_cast<size_t>(m_GridZ + 1);
	m_Height.assign(total, 0.0f);
	m_Velocity.assign(total, 0.0f);

	return true;
}

float WaterMesh::Sample(int _sx, int _sz)
{
	_sx = std::clamp(_sx, 0, m_GridX);
	_sz = std::clamp(_sz, 0, m_GridZ);

	return m_Height[Index(_sx, _sz)];
}

void WaterMesh::StepSimulation(float _deltaTime)
{
	const float gridWidth = m_GridSize;
	const float gridDepth = m_GridSize;
	if (gridWidth <= 0.0f || gridDepth <= 0.0f) return;

	// グリッドセルの間隔
	const float dx = gridWidth / float(m_GridX);
	const float dz = gridDepth / float(m_GridZ);

	const float heightCell = std::max(1e-5f, std::min(dx, dz));
	const float cell = std::max(1e-4f, m_WaveSpeed);

	const float dtCFL = 0.5f * (heightCell / cell);
	const float dtMax = std::min(_deltaTime, 0.033f);
	int steps = std::max(1, (int)std::ceil(dtMax / dtCFL));
	steps = std::min(steps, 8);
	const float sdt = dtMax / float(steps);
	const float dampPerStep = std::pow(m_Damping, sdt / (1.0f / 60.0f));
	
	const float coeff = (m_WaveSpeed * m_WaveSpeed);

	for (int s = 0; s < steps; ++s)
	{
		std::vector<float> newHeight(m_Height.size(), 0.0f);
		for (int z = 0; z <= m_GridZ; ++z)
		{
			for (int x = 0; x <= m_GridX; ++x)
			{
				size_t i = Index(x, z);
				float h = m_Height[i];
				
				float lap = Sample(x - 1, z)
						  + Sample(x + 1, z)
						  + Sample(x, z - 1)
						  + Sample(x, z + 1)
						  - 4.0f * h;
				float accel = coeff * lap;

				m_Velocity[i] += accel * sdt;
				m_Velocity[i] *= dampPerStep;
				newHeight[i] = h + m_Velocity[i] * sdt;
			}
		}
		m_Height.swap(newHeight);
	}
}

void WaterMesh::ApplyDrop(const DirectX::XMFLOAT2& _uv, float _strength, float _radius)
{
	m_PendingDrops.push_back({ _uv,_strength,_radius });
}

void WaterMesh::ApplyPendingDrops()
{
	for (const auto& drop : m_PendingDrops)
	{
		const int cx = static_cast<int>(drop.uv.x * m_GridX);
		const int cz = static_cast<int>(drop.uv.y * m_GridZ);

		const int radius = static_cast<int>(std::max(1.0f, drop.radius));
	
		for (int z = -radius; z <= radius; ++z)
		{
			for (int x = -radius; x <= radius; ++x)
			{
				int ix = std::clamp(cx + x, 0, m_GridX);
				int iz = std::clamp(cz + z, 0, m_GridZ);

				float dist = std::sqrt(float(x * x + z * z)) / std::max(1, radius);
				float w = 1.0f - dist;
				w = std::max(0.0f, w);

				size_t i = Index(ix, iz);

				m_Velocity[i] += drop.strength * w;
			}
		}
	}
	m_PendingDrops.clear();		// リストクリア
}

void WaterMesh::Update_VertexBuffer()
{
	if (m_Vertices.size() != m_Height.size()) return;

	float halfSize = m_GridSize * 0.5f;
	float gridX = static_cast<float>(m_GridX);
	float gridZ = static_cast<float>(m_GridZ);
	const float stepX = m_GridSize / gridX;
	const float stepZ = m_GridSize / gridZ;

	// 水面メッシュの高さを更新
	for (int z = 0; z <= m_GridZ; ++z)
	{
		for (int x = 0; x <= m_GridX; ++x)
		{
			const size_t i = Index(x, z);
			m_Vertices[i].position.y = m_Height[i];
		}
	}

	// 法線ベクトルを更新
	const float inv2dx = 1.0f / (2.0f * stepX);
	const float inv2dz = 1.0f / (2.0f * stepZ);
	for (int z = 0; z <= m_GridZ; ++z)
	{
		for (int x = 0; x <= m_GridX; ++x)
		{
			const int xm = std::max(0, x - 1);
			const int xp = std::min(m_GridX, x + 1);
			const int zm = std::max(0, z - 1);
			const int zp = std::min(m_GridZ, z + 1);
			
			// X方向の高さの差
			const float hx = m_Height[Index(xp, z)] - m_Height[Index(xm, z)];
			// Z方向の高さの差
			const float hz = m_Height[Index(x, zp)] - m_Height[Index(x, zm)];
		
			// 傾きを計算
			const float dhdx = hx * inv2dx;
			const float dhdz = hz * inv2dz;

			XMFLOAT3 nLocal(-dhdx, 1.0f, -dhdz);
			XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&nLocal));

			XMStoreFloat3(&m_Vertices[Index(x, z)].normal, n);
		}
	}

	// 頂点バッファをGPUに転送
	m_pVertexBuffer->Update(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
}

float WaterMesh::GetWaveHeight(float _x, float _z, float _time)
{
	// 波の高さ
	float waveHeight = 0.0f;
	for (int i = 0; i < 4; ++i)
	{
		XMFLOAT2 dir = XMFLOAT2(m_waveParams.direction[i].x, m_waveParams.direction[i].y);
		float len = m_waveParams.waveLength[i].x;
		float amp = m_waveParams.amplitude[i].x;
		float speed = m_waveParams.speed[i].x;

		float k = XM_2PI / len;
		float dot = _x * dir.x + _z * dir.y;
		float phase = k * dot + _time * speed;
		waveHeight += amp * sinf(phase);
	}

	return waveHeight;
}

float WaterMesh::GetHeightFieldHeight(float _x, float _z)
{
	// 高さ場の高さ
	float heightFieldHeight = 0.0f;

	const float halfSize = m_GridSize * 0.5f;
	float gridX_pos = (_x + halfSize) / (m_GridSize / m_GridX);
	float gridZ_pos = (_z + halfSize) / (m_GridSize / m_GridZ);

	// 最も近いグリッド点をサンプリング
	int ix = std::clamp(static_cast<int>(std::round(gridX_pos)), 0, m_GridX);
	int iz = std::clamp(static_cast<int>(std::round(gridZ_pos)), 0, m_GridZ);

	heightFieldHeight = m_Height[Index(ix, iz)];

	return heightFieldHeight;
}