#include "Player.h"
#include "Debug_New.h"
#include "Debug_Msg.h"

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
		DEBUG_LOG_ERROR("Player:モデル読み込み失敗");
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
			DEBUG_LOG_ERROR("Player:コンスタントバッファ生成失敗");
			return false;
		}

		// カメラの初期化
		auto ptr = m_pConstantBuffer[i]->GetPtr<Matrix>();
		ptr->world = DirectX::XMMatrixIdentity();
		ptr->view = m_camera->GetViewMatrix();
		ptr->proj = m_camera->GetProjMatrix();
	}

	for (size_t i = 0; i < DrawBase::FRAME_BUFFER_COUNT; ++i)
	{
		m_pLightConstantBuffer[i] = std::make_unique<ConstantBuffer>(sizeof(DirectionalLightData));
		if (!m_pLightConstantBuffer[i]->IsValid())
		{
			DEBUG_LOG_ERROR("Player:ライトコンスタントバッファ生成失敗");
			return false;
		}
	}

	// ディスクリプタヒープの生成
	m_pDescriptorHeap = std::make_unique<DescriptorHeap>();

	for (const auto& mesh : m_meshes)
	{
		std::wstring texPath = mesh.DiffuseMap;

		if (texPath.find(L"..") != std::wstring::npos ||
			texPath.find(L"Assets/Player/") != std::wstring::npos)
		{
			size_t lastSeparator = texPath.find_last_of(L"\\/");
			std::wstring fileName = texPath.substr(lastSeparator == std::wstring::npos ? 0 : lastSeparator + 1);
			
			texPath = L"Assets/Texture/" + fileName;
			DEBUG_LOG("Player:テクスチャパス ({}) を正規化しました", texPath.c_str());
		}

		if (texPath.empty())
		{
			DEBUG_LOG_ERROR("Player:メッシュにテクスチャパスが設定されていません");
			m_pTexHandles.push_back(nullptr);
			continue;
		}

		auto tex = TextureManager::Instance().GetTexture(texPath);
		if (!tex)
		{
			DEBUG_LOG_ERROR("Player:テクスチャ読み込み失敗");
		}
		m_pTexHandles.push_back(m_pDescriptorHeap->Register(tex.get()));
	}

	// マネージャー経由でルートシグネチャを取得
	auto& rootManager = RootSignatureManager::GetInstance();
	m_pRootSignature = rootManager.GetRoot(Root_Type::ROOT_TYPE_PLAYER);

	if (!m_pRootSignature->IsValid())
	{
		DEBUG_LOG_ERROR("Player:ルートシグネチャの生成に失敗");
		return false;
	}

	// マネージャー経由でパイプラインステートを取得
	auto& psoManager = PipelineState_Manager::GetInstance();
	m_pPipelineState = psoManager.GetPSO_General(PSO_Type::PSO_TYPE_PLAYER);
	if (!m_pPipelineState->IsValid())
	{
		// 頂点レイアウトの設定
		m_pPipelineState->SetInputLayout(Vertex::InputLayout);
		// ルートシグネチャの設定
		m_pPipelineState->SetRootSignature(m_pRootSignature->Get());
		// VS/PSの設定
#ifdef _DEBUG	// DEBUG
		m_pPipelineState->SetVS(L"../x64/Debug/VS_Player.cso");
		m_pPipelineState->SetPS(L"../x64/Debug/PS_Player.cso");
#else			// Release
		m_pPipelineState->SetVS(L"../x64/Release/VS_Player.cso");
		m_pPipelineState->SetPS(L"../x64/Release/PS_Player.cso");
#endif 
		// パイプラインステート作成
		m_pPipelineState->Create();
	}

	if (!m_pPipelineState->IsValid())
	{
		DEBUG_LOG_ERROR("Player:パイプラインステートの生成に失敗");
		return false;
	}
	DEBUG_LOG("Player:初期化処理に成功");
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
	for (size_t i = 0; i < m_meshes.size(); ++i)
	{
		// メッシュに対応する頂点バッファ
		auto vbView = m_pVertexBuffers[i]->View();
		// メッシュに対応する頂点の順番バッファ
		auto ibView = m_pIndexBuffers[i]->View();

		commandList->SetGraphicsRootSignature(m_pRootSignature->Get());
		commandList->SetPipelineState(m_pPipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(
			0, m_pConstantBuffer[currentIndex]->GetAddress());
		commandList->SetGraphicsRootConstantBufferView(
			2, m_pLightConstantBuffer[currentIndex]->GetAddress());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);

		auto pCurrentTexHandle = m_pTexHandles[i];
		// 使用するディスクリプタヒープをセット
		commandList->SetDescriptorHeaps(1, &materialHeap);
		// テクスチャをセット
		if (pCurrentTexHandle && pCurrentTexHandle->handleGPU.ptr != 0)
		{
			commandList->SetGraphicsRootDescriptorTable(1, pCurrentTexHandle->handleGPU);
		}
		
		// インデックスの数分描画
		commandList->DrawIndexedInstanced(static_cast<UINT>(m_meshes[i].Indices.size()), 
			1, 0, 0, 0);
	}
}
void Player::UnInit()
{
	m_camera = nullptr;
	for(auto& cb : m_pConstantBuffer)
		cb.reset();
	for (auto& cb : m_pLightConstantBuffer)
		cb.reset();
	m_pDescriptorHeap.reset();
	m_pRootSignature.reset();
	m_pTexHandles.clear();
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

	// ライトデータの更新処理
	const auto& lightData = DirectionalLight::Instance().GetLightData();
	auto ptrLight = m_pLightConstantBuffer[currentIndex]->GetPtr<DirectionalLightData>();
	*ptrLight = lightData;
}