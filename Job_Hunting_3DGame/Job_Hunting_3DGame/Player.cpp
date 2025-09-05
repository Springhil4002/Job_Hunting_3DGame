#include "Player.h"
#include "Debug_New.h"

using namespace DirectX;

std::unique_ptr<Object> Player::clone() const
{
	auto newObj = std::make_unique<Player>();
	return newObj;
}

bool Player::Init(Camera* _camera)
{
	m_camera = _camera;

	auto modelData = ModelManager::GetInstance().GetModel(L"Assets/Player/Player.FBX");
	if (!modelData)
	{
		printf("Player:モデル読み込み失敗\n");
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
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	auto tex = TextureManager::Instance().LoadTexture(L"Assets/Texture/Player.png");
	if (!tex)
	{
		printf("Player:テクスチャ読み込み失敗\n");
		return false;
	}
	m_pTexHandle = m_pDescriptorHeap->Register(tex.get());

	m_pRootSignature = std::make_unique<RootSignature_Player>();
	if (!m_pRootSignature->IsValid())
	{
		printf("Player:ルートシグネチャの生成に失敗\n");
		return false;
	}

	// パイプラインステートのインスタンス生成
	m_pPipelineState = std::make_unique<PipelineState_Player>();
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

		// 使用するディスクリプタヒープをセット
		commandList->SetDescriptorHeaps(1, &materialHeap);
		// テクスチャをセット
		commandList->SetGraphicsRootDescriptorTable(1, m_pTexHandle->handleGPU);
		// インデックスの数分描画
		commandList->DrawIndexedInstanced(m_meshes[i].Indices.size(), 1, 0, 0, 0);
	}
}
void Player::Uninit()
{
	m_camera = nullptr;
	for(auto& cb : m_pConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pRootSignature.reset();
	m_pPipelineState.reset();
	m_pTexHandle.reset();
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