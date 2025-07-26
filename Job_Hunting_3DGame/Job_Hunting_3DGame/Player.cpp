#include "Player.h"

using namespace DirectX;

Object* Player::clone() const
{
	return new Player(*this);
}

bool Player::Init(Camera* _camera)
{
	m_pModelFile = L"Assets/Player/Player.fbx";
	m_camera = _camera;
	// インポートに必要なパラメータ設定
	ImportSettings importSetting = {
		m_pModelFile,
		m_meshes,
		false,
		false
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
			printf("Player:頂点バッファの生成に失敗\n");
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
			printf("Player:インデックスバッファの生成に失敗\n");
			return false;
		}
		m_pIndexBuffers.push_back(pIB);
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pConstantBuffer[i] = new ConstantBuffer(sizeof(Matrix));
		if (!m_pConstantBuffer[i]->IsValid())
		{
			printf("Player:コンスタントバッファ生成失敗\n");
			return false;
		}

		// カメラの初期化
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();
	}

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = new DescriptorHeap();

	/*auto tex = TextureManager::Instance().LoadTexture(L"Assets/Texture/Player.png");
	if (!tex)
	{
		printf("Player:テクスチャ読み込み失敗\n");
		return false;
	}
	m_pTexHandle = m_pDescriptorHeap->Register(tex.get());*/

	m_pRootSignature = new RootSignature_Player();
	if (!m_pRootSignature->IsValid())
	{
		printf("Player:ルートシグネチャの生成に失敗\n");
		return false;
	}

	// パイプラインステートのインスタンス生成
	m_pPipelineState = new PipelineState_Player();
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
		printf("Player:パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("Player:初期化処理に成功\n");
	return true;
}
void Player::Update()
{
	Update_Transform();
	Update_CameraMatrix();
}

void Player::Draw()
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

		//// 使用するディスクリプタヒープをセット
		//commandList->SetDescriptorHeaps(1, &materialHeap);
		//// テクスチャをセット
		//commandList->SetGraphicsRootDescriptorTable(1, m_pTexHandle->handleGPU);
		// インデックスの数分描画
		commandList->DrawIndexedInstanced(m_meshes[i].Indices.size(), 1, 0, 0, 0);
	}
}
void Player::Uninit()
{
}

void Player::Update_Transform()
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

void Player::Update_CameraMatrix()
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