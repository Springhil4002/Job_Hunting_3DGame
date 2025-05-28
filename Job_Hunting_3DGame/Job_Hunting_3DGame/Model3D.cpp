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
	// インポートに必要なパラメータ設定
	ImportSettings importSetting = {
		m_pModelFile,
		m_meshes,
		false,
		true	// アリシアはテクスチャV座標が反転してるためtrue	
	};
	
	// モデルローダー
	AssimpLoader loader;

	// モデルをロード
	if (!loader.Load(importSetting))
	{
		return false;
	}

	// メッシュの数だけ頂点バッファを用意する
	m_pVertexBuffers.reserve(m_meshes.size());
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		auto size = sizeof(Vertex) * m_meshes[i].Vertices.size();
		auto stride = sizeof(Vertex);
		auto vertices = m_meshes[i].Vertices.data();
		auto pVB = new VertexBuffer(size, stride, vertices);
		if (!pVB->IsValid())
		{
			printf("頂点バッファの生成に失敗\n");
			return false;
		}
		m_pVertexBuffers.push_back(pVB);
	}

	// メッシュの数だけインデックスバッファを用意する
	m_pIndexBuffers.reserve(m_meshes.size());
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		auto size = sizeof(uint32_t) * m_meshes[i].Indices.size();
		auto indices = m_meshes[i].Indices.data();
		auto pIB = new IndexBuffer(size, indices);

		if (!pIB->IsValid())
		{
			printf("インデックスバッファの生成に失敗\n");
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

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = new DescriptorHeap();

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
		auto handle = m_pDescriptorHeap->Register(mainTex);
		m_pMaterialHandles.push_back(handle);
	}

	m_pRootSignature = new RootSignature();
	if (!m_pRootSignature->IsValid())
	{
		printf("ルートシグネチャの生成に失敗\n");
		return false;
	}

	// パイプラインステートのインスタンス生成
	m_pPipelineState = new PipelineState();
	// 頂点レイアウトの設定
	m_pPipelineState->SetInputLayout(Vertex::InputLayout);
	// ルートシグネチャの設定
	m_pPipelineState->SetRootSignature(m_pRootSignature->Get());

#ifdef _DEBUG	// DEBUG
	// VSを設定
	m_pPipelineState->SetVS(L"../x64/Debug/VS_Simple.cso");
	// PSを設定
	m_pPipelineState->SetPS(L"../x64/Debug/PS_Simple.cso");
#else			// Release
	// VSを設定
	m_pPipelineState->SetVS(L"../x64/Release/VS_Simple.cso");
	// PSを設定
	m_pPipelineState->SetPS(L"../x64/Release/PS_Simple.cso");
#endif 

	// パイプラインステート作成
	m_pPipelineState->Create();

	if (!m_pPipelineState->IsValid())
	{
		printf("パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("モデル:Aliciaの初期化処理に成功\n");
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
		commandList->DrawIndexedInstanced(m_meshes[i].Indices.size(), 1, 0, 0, 0);
	}
}

void Model3D::Uninit()
{

}
