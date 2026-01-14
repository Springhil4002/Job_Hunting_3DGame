#include "SeaMesh.h"
#include "Debug_New.h"

using namespace DirectX;

bool SeaMesh::Init(Camera* _camera)
{
	m_Camera = _camera;

	if (!Init_SimulationResources()) return false;
	Init_Settings(m_Camera);

	printf("SeaMesh:初期化処理に成功\n\n");
	return true;
}

void SeaMesh::Update()
{
	m_Time += 0.016f;

	Apply_PendingDrops();
	StepSimulation(0.016f);
	
	Update_VertexBuffer();
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
	cmdList->SetGraphicsRootConstantBufferView(
		0, m_pConstantBuffer[currentIndex]->GetAddress());
	cmdList->SetGraphicsRootConstantBufferView(
		2, m_pLightConstantBuffer[currentIndex]->GetAddress());

	cmdList->SetDescriptorHeaps(1, &Heap);
	cmdList->SetGraphicsRootDescriptorTable(1, m_pSkyCubeTexHandle->handleGPU);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	cmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}

void SeaMesh::UnInit()
{
	m_Camera = nullptr;
	m_pVertexBuffer.reset();
	m_pIndexBuffer.reset();
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	for (auto& cb : m_pLightConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pSkyCubeTexHandle.reset();
	m_pRootSignature.reset();
}

std::unique_ptr<Object> SeaMesh::clone() const
{
	auto newObj = std::make_unique<SeaMesh>();
	return newObj;
}

size_t SeaMesh::Index(int _x, int _z) const
{
	return static_cast<size_t>(_z) * (m_GridX + 1) + static_cast<size_t>(_x);
}

float SeaMesh::Sample(int _sx, int _sz)
{
	_sx = std::clamp(_sx, 0, m_GridX);
	_sz = std::clamp(_sz, 0, m_GridZ);
	
	return m_Height[Index(_sx, _sz)];
}

void SeaMesh::StepSimulation(float _deltaTime)
{
	const float gridWidth = m_GridSize;
	const float gridDepth = m_GridSize;
	if (gridWidth <= 0.0f || gridDepth <= 0.0f) return;

	// グリッドセルの間隔
	const float dx = gridWidth / float(m_GridX);
	const float dz = gridDepth / float(m_GridZ);

	// 時間ステップの計算
	const float heightCell = std::max(1e-5f, std::min(dx, dz));
	const float cell = std::max(1e-4f, m_WaveSpeed);

	const float dtCFL = 0.5f * (heightCell / cell);
	const float dtMax = std::min(_deltaTime, 0.033f);
	int steps = std::max(1, (int)std::ceil(dtMax / dtCFL));
	steps = std::min(steps, 8);
	const float sdt = dtMax / float(steps);
	
	// 減衰率の調整
	const float dampPerStep = std::pow(m_Damping, sdt / (1.0f / 60.0f));

	const float coeff = (m_WaveSpeed * m_WaveSpeed);

	for (int s = 0; s < steps; ++s)
	{
		// 次のフレームの高さ
		std::vector<float> newHeight(m_Height.size(), 0.0f);
		for (int z = 0; z <= m_GridZ; ++z)
		{
			for (int x = 0; x <= m_GridX; ++x)
			{
				size_t i = Index(x, z);
				float h = m_Height[i];

				// 高さの計算
				float lap = Sample(x - 1, z)
						  + Sample(x + 1, z)
						  + Sample(x, z - 1)
						  + Sample(x, z + 1)
						  - 4.0f * h;
				float accel = coeff * lap;

				// 速度と高さの更新
				m_Velocity[i] += accel * sdt;
				m_Velocity[i] *= dampPerStep;
				newHeight[i] = h + m_Velocity[i] * sdt;
			}
		}
		m_Height.swap(newHeight);
	}
}

void SeaMesh::Apply_Drop(const XMFLOAT2& _uv, float _strength, float _radius)
{
	m_PendingDrops.push_back({ _uv, _strength, _radius });
}

void SeaMesh::Apply_PendingDrops()
{
	for (const auto& drop : m_PendingDrops)
	{
		// UV座標をグリッド状のインデックスに変換
		const int cx = static_cast<int>(drop.uv.x * m_GridX);
		const int cz = static_cast<int>(drop.uv.y * m_GridZ);

		const int radius = static_cast<int>(std::max(1.0f, drop.radius));

		// 範囲内のグリッドを処理
		for (int z = -radius; z <= radius; ++z)
		{
			for (int x = -radius; x <= radius; ++x)
			{
				int ix = std::clamp(cx + x, 0, m_GridX);
				int iz = std::clamp(cz + z, 0, m_GridZ);

				// 中心からの距離に応じた強度を決定
				float dist = std::sqrt(float(x * x + z * z)) / std::max(1, radius);
				float w = 1.0f - dist;
				w = std::max(0.0f, w);

				size_t i = Index(ix, iz);

				// 速度場に衝撃として加算
				m_Velocity[i] += drop.strength * w;
			}
		}
	}
	// リストクリア
	m_PendingDrops.clear();		
}

Mesh SeaMesh::CreateMesh()
{
	Mesh mesh;
	mesh.Vertices.resize((m_GridX + 1) * (m_GridZ + 1));
	mesh.Indices.reserve(m_GridX * m_GridZ * 6);
	
	float halfSize = m_GridSize * 0.5f;
	float fGridX = static_cast<float>(m_GridX);
	float fGridZ = static_cast<float>(m_GridZ);
	float stepX = m_GridSize / fGridX;
	float stepZ = m_GridSize / fGridZ;
	
	// 頂点生成
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
				XMFLOAT3(px,0.0f,pz),			// 位置
				XMFLOAT3(0.0f,1.0f,0.0f),		// 法線
				XMFLOAT2(u,v),					// UV
				XMFLOAT3(1.0f,0.0f,0.0f),		// 接線
				XMFLOAT4(1.0f,1.0f,1.0f,1.0f)	// カラー
			};
		}
	}

	// インデックス生成
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

bool SeaMesh::Init_SimulationResources()
{
	m_GridX = 256;			// X
	m_GridZ = 256;			// Z
	m_GridSize = 512.0f;	// 全体の大きさ
	m_WaveSpeed = 10.0f;	// 波紋の速度
	m_Damping = 0.98f;		// 減衰率
	
	const size_t total = static_cast<size_t>(m_GridX + 1) *
		static_cast<size_t>(m_GridZ + 1);

	m_Height.assign(total, 0.0f);
	m_Velocity.assign(total, 0.0f);

	printf("SeaMesh:高さ場の初期化設定に成功\n");
	return true;
}

bool SeaMesh::Init_Settings(Camera* _camera)
{
	Mesh mesh = CreateMesh();
	auto vertexSize = sizeof(Vertex) * std::size(m_Vertices);
	auto vertexStride = sizeof(Vertex);
	m_pVertexBuffer = std::make_unique<VertexBuffer>(vertexSize, vertexStride, m_Vertices.data());
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

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pLightConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(DirectionalLightData));
		if (!m_pLightConstantBuffer[i]->IsValid())
		{
			printf("Goal:ライトコンスタントバッファ生成失敗\n");
			return false;
		}
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
	return true;
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

	// ライトデータの更新処理
	const auto& lightData = DirectionalLight::Instance().GetLightData();
	auto ptrLight = m_pLightConstantBuffer[currentIndex]->GetPtr<DirectionalLightData>();
	*ptrLight = lightData;
}

void SeaMesh::Update_VertexBuffer()
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
			// 頂点の高さを更新
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

float SeaMesh::GetHeightFieldHeight(float _x, float _z)
{
	// 高さ場の高さ
	float heightFieldHeight = 0.0f;

	// ワールド座標をグリッド状の座標に変換
	const float halfSize = m_GridSize * 0.5f;
	float gridX_pos = (_x + halfSize) / (m_GridSize / m_GridX);
	float gridZ_pos = (_z + halfSize) / (m_GridSize / m_GridZ);

	// 最も近いグリッド点をサンプリング
	int ix = std::clamp(static_cast<int>(std::round(gridX_pos)), 0, m_GridX);
	int iz = std::clamp(static_cast<int>(std::round(gridZ_pos)), 0, m_GridZ);

	heightFieldHeight = m_Height[Index(ix, iz)];

	return heightFieldHeight;
}