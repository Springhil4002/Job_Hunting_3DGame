#include "Model3D.h"
#include "Debug_New.h"

std::wstring Model3D::ReplaceExtension(const std::wstring& _origin, const char* _ext)
{
	if (_origin.empty()) return L"";
	std::filesystem::path path = _origin.c_str();
	return path.replace_extension(_ext).wstring();
}

std::unique_ptr<Object> Model3D::clone() const
{
	auto newObj = std::make_unique<Model3D>();
	return newObj;
}

bool Model3D::Init(Camera* _camera)
{
	m_camera = _camera;

	// ModelManager からモデルデータを取得
	auto modelData = ModelManager::GetInstance().GetModel(L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX");
	if (!modelData)
	{
		printf("Model3D:モデル読み込み失敗\n");
		return false;
	}

	m_meshes = modelData->meshes;
	m_pVertexBuffers = modelData->vertexBuffers;
	m_pIndexBuffers = modelData->indexBuffers;

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid()) 
		{
			printf("コンスタントバッファ生成失敗\n");
			return false;
		}

		// カメラの初期化
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();
	}

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	// マテリアルの読み込み
	m_pMaterialHandles.clear();
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		if (m_meshes[i].DiffuseMap.empty()) continue;

		auto texPath = ReplaceExtension(m_meshes[i].DiffuseMap, "tga");
		auto mainTex = Texture2D::Get(texPath);
		if (!mainTex)
		{
			OutputDebugStringW((L"テクスチャが読み込めませんでした: " + texPath + L"\n").c_str());
			continue;
		}
		if (!m_pDescriptorHeap)
		{
			OutputDebugStringA("descriptorHeap が初期化されていません。\n");
			continue;
		}
		auto handle = m_pDescriptorHeap->Register(mainTex.get());
		m_pMaterialHandles.push_back(handle);
	}

	auto& rootManager = RootSignatureManager::GetInstance();
	m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_MODEL3D);
	if (!m_pRootSignature->IsValid())
	{
		printf("ルートシグネチャの生成に失敗\n");
		return false;
	}

	// パイプラインステートのインスタンス生成
	auto& psoManager = PipelineState_Manager::GetInstance();
	m_pPipelineState = psoManager.GetPSO_General(PSO_Type::PSO_TYPE_MODEL3D);
	if (!m_pPipelineState->IsValid())
	{
		// 頂点レイアウトの設定
		m_pPipelineState->SetInputLayout(Vertex::InputLayout);
		// ルートシグネチャの設定
		m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
		// VS/PSの設定
#ifdef _DEBUG	// DEBUG
		m_pPipelineState->SetVS(L"../x64/Debug/VS_Simple.cso");
		m_pPipelineState->SetPS(L"../x64/Debug/PS_Simple.cso");
#else			// Release
		m_pPipelineState->SetVS(L"../x64/Release/VS_Simple.cso");
		m_pPipelineState->SetPS(L"../x64/Release/PS_Simple.cso");
#endif 
		// パイプラインステート作成
		m_pPipelineState->Create();
	}
	if (!m_pPipelineState->IsValid())
	{
		printf("Model3D:パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("Model3Dの初期化処理に成功\n\n");
	return true;
}

void Model3D::Update()
{
	// カメラの更新処理
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
	// 現在のフレーム番号を取得
	auto currentIndex = g_DrawBase->CurrentBackBufferIndex();
	// コマンドリスト
	auto commandList = g_DrawBase->CommandList();
	// ディスクリプタヒープ
	auto materialHeap = m_pDescriptorHeap->GetHeap();

	//　メッシュの数だけインデックス分の描画を行う
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		// メッシュに対応する頂点バッファ
		auto vbView = m_pVertexBuffers[i]->View();
		// メッシュに対応する頂点の順番バッファ
		auto ibView = m_pIndexBuffers[i]->View();

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());
		commandList->SetPipelineState(m_pPipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(
			0, m_pConstantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);

		// 使用するディスクリプタヒープをセット
		commandList->SetDescriptorHeaps(1, &materialHeap);
		// メッシュに対応するディスクリプタテーブルをセット
		commandList->SetGraphicsRootDescriptorTable(1, m_pMaterialHandles[i]->handleGPU);

		// インデックスの数分描画
		commandList->DrawIndexedInstanced(static_cast<UINT>(m_meshes[i].Indices.size()),
			1, 0, 0, 0);
	}
}

void Model3D::Uninit()
{
	m_camera = nullptr;
	m_pVertexBuffer.reset();
	m_pIndexBuffer.reset();
	for (auto& cb : m_pConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pRootSignature.reset();
	m_meshes.clear();
	m_pVertexBuffers.clear();
	m_pIndexBuffers.clear();
	m_pMaterialHandles.clear();
}
